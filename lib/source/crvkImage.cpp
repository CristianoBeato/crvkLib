/*
===========================================================================================
    This file is part of crvkLib Vulkan + SDL minimal framework.

    Copyright (c) 2025 Cristiano B. Santos <cristianobeato_dm@hotmail.com>
    Contributor(s): none yet.

-------------------------------------------------------------------------------------------

 This file is part of the crvkLib library and is licensed under the
 MIT License with Attribution Requirement.

 You are free to use, modify, and distribute this file (even commercially),
 as long as you give credit to the original author:

     “Based on crvkCore by Cristiano Beato – https://github.com/CristianoBeato/crvkLib”

 For full license terms, see the LICENSE file in the root of this repository.
===============================================================================================
*/
#include "crvkPrecompiled.hpp"
#include "crvkImage.hpp"

typedef struct crvkImageHandle_t
{
    uint16_t                levels = 1;
    uint16_t                layers = 1;
    uint32_t                queue = VK_QUEUE_FAMILY_IGNORED;
    VkFormat                format = VK_FORMAT_UNDEFINED;
    VkImageViewType         type = VK_IMAGE_VIEW_TYPE_1D;
    VkSampleCountFlagBits   samples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineStageFlags2   stage = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2          access = VK_ACCESS_2_NONE;
    VkImageAspectFlags      aspect = VK_IMAGE_ASPECT_NONE;
    VkImageLayout           layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImage                 image = nullptr;
    VkImageView             view = nullptr;
    VkDeviceMemory          memory = nullptr;
    VkDevice                device = nullptr;
}crvkImageHandle_t;

/*
==============================================
crvkImage::crvkImage
==============================================
*/
crvkImage::crvkImage( void ) : m_imageHandle( nullptr )
{
    m_imageHandle = new crvkImageHandle_t;
}

/*
==============================================
crvkImage::~crvkImage
==============================================
*/
crvkImage::~crvkImage( void )
{
    Destroy();
    
    if( m_imageHandle != nullptr )
    {
        delete m_imageHandle;
        m_imageHandle = nullptr;
    }
}

/*
==============================================
crvkImage::Create
==============================================
*/
bool crvkImage::Create( 
    const crvkDevice* in_device, 
    const VkImageViewType in_type, 
    const VkFormat in_format,
    const uint16_t in_levels,
    const uint16_t in_layers,
    const uint32_t in_width,
    const uint32_t in_height,
    const uint32_t in_depth,
    const VkSampleCountFlagBits in_samples
)
{
    VkResult result = VK_SUCCESS;
    VkMemoryRequirements memReq{};
    
    // fail proof 
    m_imageHandle->levels = std::max( in_levels, (unsigned short)1 ); // fail proof, this are never 0, we need atleast 1 level 
    m_imageHandle->layers = std::max( in_layers, (unsigned short)1 ); // fail proof, this are never 0, we need atleast 1 layer 
    m_imageHandle->type = in_type;  // texture dimension type 
    m_imageHandle->format = in_format; // pixel format 
    m_imageHandle->samples = in_samples; // samples 
    m_imageHandle->device = in_device->Device(); // device 
    
    ///
    /// Create the image handler 
    /// ==========================================================================
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.format = m_imageHandle->format;
    imageCI.extent = { in_width, in_height, in_depth };
    imageCI.mipLevels = in_levels;
    imageCI.arrayLayers = in_layers;
    imageCI.samples = m_imageHandle->samples; // todo implement multisampling 
    imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // todo:
    imageCI.initialLayout = m_imageHandle->layout;

    switch ( m_imageHandle->type )
    {
        case VK_IMAGE_VIEW_TYPE_1D: // 1d texture 
            imageCI.imageType = VK_IMAGE_TYPE_1D;
            break;
        case VK_IMAGE_VIEW_TYPE_2D: // 2d texture 
        case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            break;
        case VK_IMAGE_VIEW_TYPE_3D: // 3d texture 
        case VK_IMAGE_VIEW_TYPE_CUBE:
        case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
            imageCI.imageType = VK_IMAGE_TYPE_3D;
            break;
    }

    switch ( in_format )
    {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            m_imageHandle->aspect = VK_IMAGE_ASPECT_DEPTH_BIT;    
            break;
        case VK_FORMAT_S8_UINT:
            m_imageHandle->aspect = VK_IMAGE_ASPECT_STENCIL_BIT;    
            break;
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            m_imageHandle->aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;    
            break;
    default:
        m_imageHandle->aspect = VK_IMAGE_ASPECT_COLOR_BIT; // we assume that all other formats are color formats 
        break;
    }

    // create teh image handle
    result = vkCreateImage( m_imageHandle->device, &imageCI, k_allocationCallbacks, &m_imageHandle->image );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkImage::Create::vkCreateImage", result );
        return false;
    }
    
    vkGetImageMemoryRequirements( m_imageHandle->device, m_imageHandle->image, &memReq );
    
    ///
    /// Allocate image for the memory 
    /// ==========================================================================
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = in_device->FindMemoryType( memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
    result = vkAllocateMemory( m_imageHandle->device, &allocInfo, nullptr, &m_imageHandle->memory ); 
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkImage::Create::vkAllocateMemory", result );
        return false;
    }

    // bind image to memory 
    vkBindImageMemory( m_imageHandle->device, m_imageHandle->image, m_imageHandle->memory, 0 );

    ///
    /// Create the image view
    /// ==========================================================================
    VkImageSubresourceRange    subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = in_levels;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = in_layers;

    VkImageViewCreateInfo viewCI{};
    viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCI.image = m_imageHandle->image;
    viewCI.viewType = m_imageHandle->type;
    viewCI.format = m_imageHandle->format;
    viewCI.subresourceRange = subresourceRange;
    result = vkCreateImageView( m_imageHandle->device, &viewCI, k_allocationCallbacks, &m_imageHandle->view );
    if( result != VK_SUCCESS )
    {
        delete m_imageHandle;
        crvkAppendError( "crvkImage::Create::vkCreateImageView", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkImage::Create
==============================================
*/
void crvkImage::Destroy(void)
{
    // invalid image 
    if ( m_imageHandle == nullptr )
        return;

    // release image view 
    if ( m_imageHandle->view != nullptr )
    {
        vkDestroyImageView( m_imageHandle->device, m_imageHandle->view, k_allocationCallbacks );
        m_imageHandle->view = nullptr;
    }

    // release image memory
    if ( m_imageHandle->memory != nullptr )
    {
        vkFreeMemory( m_imageHandle->device, m_imageHandle->memory, k_allocationCallbacks );
        m_imageHandle->memory = nullptr;
    }
    
    // release image handler
    if ( m_imageHandle->image != nullptr )
    {
        vkDestroyImage( m_imageHandle->device, m_imageHandle->image, k_allocationCallbacks );
        m_imageHandle->image = nullptr;
    }
}

/*
==============================================
crvkImage::Handle
==============================================
*/
VkImage crvkImage::Handle( void ) const 
{
    if ( m_imageHandle == nullptr )
        return nullptr;

    return m_imageHandle->image;
}

/*
==============================================
crvkImage::View
==============================================
*/
VkImageView crvkImage::View( void ) const
{
    if ( m_imageHandle == nullptr )
        return nullptr;

    return m_imageHandle->view;
}

/*
==============================================
crvkImage::View
==============================================
*/
VkDeviceMemory crvkImage::Memory( void ) const
{
    if ( m_imageHandle == nullptr )
        return nullptr;

    return m_imageHandle->memory;
}

void crvkImage::StateTransition( 
        const VkCommandBuffer in_commandBuffer, 
        const crvkImageState_t in_state,
        const VkImageAspectFlags in_aspect, 
        const uint32_t in_dstQueue )
{
    VkPipelineStageFlags2   stage = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2          access = VK_ACCESS_2_NONE;
    VkImageLayout           layout = VK_IMAGE_LAYOUT_UNDEFINED;
    
    // of not a valid image, ignore
    if( m_imageHandle == nullptr || m_imageHandle->image == nullptr )
        return;
    
    switch ( in_state )
    {
        // Uso como Sampler em Shaders (gráfico ou compute)
        case CRVK_IMAGE_STATE_GRAPHIC_SHADER_SAMPLER:
        {
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
            access = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
        } break;
        case CRVK_IMAGE_STATE_GRAPHIC_SHADER_BINDING:
        {
            layout = VK_IMAGE_LAYOUT_GENERAL;
            stage = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
            access = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        } break;
        case CRVK_IMAGE_STATE_GRAPHIC_RENDER_TARGET:
        {
            layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
        } break;
        case CRVK_IMAGE_STAGE_GRAPHIC_RENDER_DEPTH:
        {
            layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
            access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
        } break;
        case CRVK_IMAGE_STAGE_GRAPHIC_RENDER_DEPTH_STENCIL:
        {
            layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
            access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
        } break;
        // Uso em Shader de Compute (leitura)
        case CRVK_IMAGE_STATE_COMPUTE_READ:
        {
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            access = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
        } break;
        // Uso em Shader de Compute (escrita / destino)
        case CRVK_IMAGE_STATE_COMPUTE_WRITE:
        {
            layout = VK_IMAGE_LAYOUT_GENERAL;
            stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
            access = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        } break;
        // Cópia de pixels da imagem (readback ou blit)
        case CRVK_IMAGE_STATE_GPU_COPY_SRC:
        {
            layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            access = VK_ACCESS_2_TRANSFER_READ_BIT;
        } break;
        // Cópia de pixels para imagem (upload → GPU)
        case CRVK_IMAGE_STATE_GPU_COPY_DST:
        {
            stage = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            access = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        } break;
    }

    // we update the whole image, since we map the current texture state change, we can't handle sections
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = ( in_aspect == VK_IMAGE_ASPECT_NONE ) ? m_imageHandle->aspect : in_aspect; // if no aspect set, use from image
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = m_imageHandle->levels;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = m_imageHandle->layers;

    VkImageMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = m_imageHandle->stage;
    barrier.srcAccessMask = m_imageHandle->access;
    barrier.dstStageMask = stage;
    barrier.dstAccessMask = access;
    barrier.oldLayout = m_imageHandle->layout;
    barrier.newLayout = layout;
    barrier.srcQueueFamilyIndex = m_imageHandle->queue;
    barrier.dstQueueFamilyIndex = in_dstQueue;
    barrier.image = m_imageHandle->image;
    barrier.subresourceRange = subresourceRange;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;
    depInfo.dependencyFlags = 0;
    depInfo.memoryBarrierCount = 0;
    depInfo.pMemoryBarriers = nullptr;
    depInfo.bufferMemoryBarrierCount = 0;
    depInfo.pBufferMemoryBarriers = nullptr;
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &barrier;
    vkCmdPipelineBarrier2( in_commandBuffer, &depInfo );

    //
    m_imageHandle->stage = stage;
    m_imageHandle->access = access;
    m_imageHandle->layout = layout;  
}

//=======================================================================================================================

/*
==============================================
crvkImage::crvkImageStatic
==============================================
*/
crvkImageStatic::crvkImageStatic(void) : crvkImage(), m_commandBuffer( nullptr )
{
}

/*
==============================================
crvkImage::~crvkImageStatic
==============================================
*/
crvkImageStatic::~crvkImageStatic(void)
{
}

/*
==============================================
crvkImage::Create
==============================================
*/
bool crvkImageStatic::Create(   const crvkDevice *in_device, 
                                const VkImageViewType in_type, 
                                const VkFormat in_format, 
                                const uint16_t in_levels, 
                                const uint16_t in_layers, 
                                const uint32_t in_width, 
                                const uint32_t in_height, 
                                const uint32_t in_depth,
                                const VkSampleCountFlagBits in_samples
                            )
{
    VkResult result = VK_SUCCESS;
    crvkDeviceQueue * queue = nullptr;
    m_device = const_cast<crvkDevice*>( in_device );

    if( in_device->HasTransferQueue() )
        queue = in_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = in_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );
    
    // get the command pool
    m_commandPool = queue->CommandPool();

    // assign the device to a queque
    if ( !crvkImage::Create( in_device, in_type, in_format, in_levels, in_layers, in_width, in_height, in_depth, in_samples ) )
        return false;

    ///
    /// Create the Command buffer to record the buffer transfer operations
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = m_commandPool;

    result = vkAllocateCommandBuffers( m_imageHandle->device, &allocInfo, &m_commandBuffer );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    ///
    /// Create semaphores 
    /// ==========================================================================
    VkSemaphoreTypeCreateInfo timelineCreateInfo{};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo copySemaphoreCI{};
    copySemaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    copySemaphoreCI.flags = 0;
    copySemaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( m_imageHandle->device, &copySemaphoreCI, k_allocationCallbacks, &m_copySemaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkCreateSemaphore::COPY", result );
        return false;
    }

    VkSemaphoreCreateInfo drawSemaphoreCI{};
    drawSemaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawSemaphoreCI.flags = 0;
    drawSemaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( m_imageHandle->device, &drawSemaphoreCI, k_allocationCallbacks, &m_useSemaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkCreateSemaphore::DRAW", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkImage::Destroy
==============================================
*/
void crvkImageStatic::Destroy( void )
{
    if ( m_useSemaphore != nullptr )
    {
        vkDestroySemaphore( m_imageHandle->device, m_useSemaphore, k_allocationCallbacks );
        m_useSemaphore = nullptr;
    }
    
    if ( m_copySemaphore != nullptr )
    {
        vkDestroySemaphore( m_imageHandle->device, m_copySemaphore, k_allocationCallbacks );
        m_copySemaphore = nullptr;
    }

    // release the buffer operation command buffer 
    if ( m_commandBuffer != nullptr )
    {
        vkFreeCommandBuffers( m_imageHandle->device, m_commandPool, 1, &m_commandBuffer );
        m_commandBuffer = nullptr;
    }

    crvkImage::Destroy();
}

/*
==============================================
crvkImage::CopyFromBuffer
==============================================
*/
bool crvkImageStatic::CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count )
{
    VkResult result = VK_SUCCESS;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;
    crvkDeviceQueue* queue = nullptr;

    // reset the command buffer 
    result = vkResetCommandBuffer( m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if (result != VK_SUCCESS) 
    {
        crvkAppendError("crvkImageStatic::CopyFromBuffer::vkResetCommandBuffer", result );
        return false;
    }

    // begin record image commands 
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if (result != VK_SUCCESS) 
    {
        crvkAppendError("crvkImageStatic::CopyFromBuffer::vkBeginCommandBuffer", result );
        return false;
    }

    // get the ranges
    for (uint32_t i = 0; i < in_count; ++i) 
    {
        aspectMask |= in_copyRegions[i].imageSubresource.aspectMask;
    }

    // Layout transition UNDEFINED → TRANSFER_DST_OPTIMAL
    StateTransition( m_commandBuffer, CRVK_IMAGE_STATE_GPU_COPY_DST, aspectMask, VK_QUEUE_FAMILY_IGNORED );

    // stream from buffer to the image
    VkCopyBufferToImageInfo2 copyBufferToImage{};
    copyBufferToImage.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2;
    copyBufferToImage.pNext = nullptr;
    copyBufferToImage.srcBuffer = in_srcBuffer;
    copyBufferToImage.dstImage = m_imageHandle->image;
    copyBufferToImage.dstImageLayout = m_imageHandle->layout;
    copyBufferToImage.regionCount = in_count;
    copyBufferToImage.pRegions = in_copyRegions;
    vkCmdCopyBufferToImage2( m_commandBuffer, &copyBufferToImage );

    // finish state transiotion and copy commands
    vkEndCommandBuffer( m_commandBuffer );

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        WaitLastCopy(),
        WaitLastUse(),
    };

    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();

    VkCommandBufferSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    submitInfo.commandBuffer = m_commandBuffer;
    submitInfo.pNext = nullptr;
    
    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );       

    result = queue->Submit( waitInfo, 2, &submitInfo, 1, &signalInfo, 1, nullptr );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkQueueSubmit2", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkImage::CopyToBuffer
==============================================
*/
bool crvkImageStatic::CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count )
{
    VkResult result = VK_SUCCESS;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;
    crvkDeviceQueue* queue = nullptr;
    
    // reset the command buffer 
    result = vkResetCommandBuffer( m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if (result != VK_SUCCESS) 
    {
        crvkAppendError("crvkImageStatic::CopyToBuffer::vkResetCommandBuffer", result );
        return false;
    }

    // begin record image commands 
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if (result != VK_SUCCESS) 
    {
        crvkAppendError("crvkImageStatic::CopyToBuffer::vkBeginCommandBuffer", result );
        return false;
    }
 
    // Layout transition UNDEFINED → TRANSFER_SRC_OPTIMAL
    StateTransition( m_commandBuffer, CRVK_IMAGE_STATE_GPU_COPY_SRC, aspectMask, VK_QUEUE_FAMILY_IGNORED );

    // copy from image to buffer
    VkCopyImageToBufferInfo2 copyImageToBuffer{};
    copyImageToBuffer.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
    copyImageToBuffer.pNext = nullptr;
    copyImageToBuffer.srcImage = m_imageHandle->image;
    copyImageToBuffer.srcImageLayout = m_imageHandle->layout;
    copyImageToBuffer.dstBuffer = in_dstBuffer;
    copyImageToBuffer.regionCount = in_count;
    copyImageToBuffer.pRegions = in_copyRegions;
    vkCmdCopyImageToBuffer2( m_commandBuffer, &copyImageToBuffer );

    // finish state transiotion and copy commands
    vkEndCommandBuffer( m_commandBuffer );

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        WaitLastCopy(),
        WaitLastUse(),
    };

    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();

    VkCommandBufferSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    submitInfo.commandBuffer = m_commandBuffer;
    submitInfo.pNext = nullptr;
    
    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );       

    result = queue->Submit( waitInfo, 2, &submitInfo, 1, &signalInfo, 1, nullptr );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkQueueSubmit2", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkImage::StateTransition
==============================================
*/
void crvkImageStatic::StateTransition( const VkCommandBuffer in_commandBuffer, const crvkImageState_t in_state, const VkImageAspectFlags in_aspect, const uint32_t in_dstQueue )
{
    crvkImage::StateTransition( m_commandBuffer, in_state, in_aspect, in_dstQueue );
}

//=======================================================================================================================

/*
==============================================
crvkImageStaging::Create
==============================================
*/
bool crvkImageStaging::Create(const crvkDevice *in_device, const VkImageViewType in_type, const VkFormat in_format, const uint16_t in_levels, const uint16_t in_layers, const uint32_t in_width, const uint32_t in_height, const uint32_t in_depth)
{
    VkMemoryRequirements memReq{}; 
    VkResult result = VK_SUCCESS;
    
    // create image and command buffer 
    crvkImageStatic::Create( in_device, in_type, in_format, in_levels, in_layers, in_width, in_height, in_depth );

    // get the image size    
    vkGetImageMemoryRequirements( m_imageHandle->device, m_imageHandle->image, &memReq );
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = memReq.size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    // we use tranfer queue for buffer content
    if( in_device->HasTransferQueue() )
        bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    else
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer( m_imageHandle->device, &bufferInfo, k_allocationCallbacks, &m_staging );
    if ( result != VK_SUCCESS) 
    {
        Destroy();
        crvkAppendError( "crvkBufferStatic::Create::vkCreateBuffer", result );
        return false;
    }

    vkGetBufferMemoryRequirements( m_imageHandle->device, m_staging, &memReq );

    // try find the required memory 
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = in_device->FindMemoryType( memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
    result = vkAllocateMemory( m_imageHandle->device, &allocInfo, k_allocationCallbacks, &m_memoryStaging    );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkBufferStatic::Create::vkAllocateMemory", result );
        return false;
    }

    result = vkBindBufferMemory( m_imageHandle->device, m_staging, m_memoryStaging, 0 );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkBindBufferMemory", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkImageStaging::crvkImageStaging
==============================================
*/
void crvkImageStaging::Destroy(void)
{
    if( m_staging != nullptr )
    {
        vkDestroyBuffer( m_imageHandle->device, m_staging, k_allocationCallbacks );
        m_staging = nullptr;
    };

    if ( m_memoryStaging != nullptr )
    {
        vkFreeMemory( m_imageHandle->device, m_memoryStaging, k_allocationCallbacks );
        m_memoryStaging = nullptr;
    }

    crvkImageStatic::Destroy();
}

/*
==============================================
crvkImageStaging::SubData
==============================================
*/
bool crvkImageStaging::SubData( const void* in_data, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count )
{
    VkResult result = VK_SUCCESS;
    VkDeviceSize offset = UINT64_MAX;
    VkDeviceSize size = 0;
    uint64_t pixelCount = 0;
    crvkFormat_t internalFormat = crvkFormat_t( m_imageHandle->format ); 
    void* buff = nullptr;

    // we need to calc the current source size
    for ( uint32_t i = 0; i < in_count; i++)
    {
        auto r = in_copyRegions[i];
        // get the minor offset 
        offset = std::min( r.bufferOffset, offset );
        uint32_t w = std::min( r.imageExtent.width, 1u );
        uint32_t h = std::min( r.imageExtent.height, 1u );
        uint32_t d = std::min( r.imageExtent.depth, 1u );
        pixelCount += ( w * h * d );  
    }
    
    if ( internalFormat.IsCompressed() )
    {
        // TODO: get the number blocks

    }
    else
    {
        // get the uncompressed image size
        size = pixelCount * internalFormat.BytesPerPixel();
    }    

    // copy data to our CPU buffer 
    result = vkMapMemory( m_imageHandle->device, m_memoryStaging, offset, size, 0, &buff );
    std::memcpy( buff, in_data, size );
    vkUnmapMemory( m_imageHandle->device, m_memoryStaging );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkImageStaging::SubData::vkMapMemory", result );
        return false;
    }    
    
    // upload from transfer buffer, to the image 
    if( !CopyFromBuffer( m_staging, in_copyRegions, in_count ) )
        return false;

    return true;
}

/*
==============================================
crvkImageStaging::GetSubData
==============================================
*/
bool crvkImageStaging::GetSubData( void* in_data, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count )
{
    VkResult result = VK_SUCCESS;
    VkDeviceSize offset = UINT64_MAX;
    VkDeviceSize size = 0;
    uint64_t pixelCount = 0;
    crvkFormat_t internalFormat = crvkFormat_t( m_imageHandle->format ); 
    void* buff = nullptr;

    // we need to calc the current source size
    for ( uint32_t i = 0; i < in_count; i++)
    {
        auto r = in_copyRegions[i];
        // get the minor offset 
        offset = std::min( r.bufferOffset, offset );
        uint32_t w = std::min( r.imageExtent.width, 1u );
        uint32_t h = std::min( r.imageExtent.height, 1u );
        uint32_t d = std::min( r.imageExtent.depth, 1u );
        pixelCount += ( w * h * d );  
    }
    
    if ( internalFormat.IsCompressed() )
    {
        // TODO: get the number blocks

    }
    else
        size = pixelCount * internalFormat.BytesPerPixel(); // get the uncompressed image size
   
    // now we copy from image to the buffer
    if( !CopyToBuffer( m_staging, in_copyRegions, in_count ) )
        return false;

    // wait for device end copy the image 
    VkSemaphoreWaitInfo smWaitInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO, nullptr, 0, 1, &m_copySemaphore }; 
    vkWaitSemaphores( m_imageHandle->device, &smWaitInfo, UINT64_MAX );

    // copy the content of the CPU buffer to the data pointer
    vkMapMemory( m_imageHandle->device, m_memoryStaging, offset, size, 0, &buff );
    std::memcpy( in_data, buff, size );
    vkUnmapMemory( m_imageHandle->device, m_memoryStaging ); 
}
