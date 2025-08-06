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

/*
==============================================
crvkImage::crvkImage
==============================================
*/
crvkImage::crvkImage( void ) : m_imageHandle( nullptr ), m_imageView( nullptr ), m_imageMemory( nullptr )
{
}

/*
==============================================
crvkImage::~crvkImage
==============================================
*/
crvkImage::~crvkImage( void )
{
    Destroy();
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
    const uint32_t in_depth
)
{
    VkResult result = VK_SUCCESS;
    VkMemoryRequirements memReq{};
    
    m_device = const_cast<crvkDevice*>( in_device );
    VkDevice device = in_device->Device();

    ///
    /// Create the image handler 
    /// ==========================================================================
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.format = in_format;
    imageCI.extent = { in_width, in_height, in_depth };
    imageCI.mipLevels = in_levels;
    imageCI.arrayLayers = in_layers;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // todo:
    imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    switch ( in_type )
    {
        case VK_IMAGE_VIEW_TYPE_1D:
            imageCI.imageType = VK_IMAGE_TYPE_1D;
            break;
        case VK_IMAGE_VIEW_TYPE_2D:
        case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            break;
        case VK_IMAGE_VIEW_TYPE_3D:
        case VK_IMAGE_VIEW_TYPE_CUBE:
        case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
            imageCI.imageType = VK_IMAGE_TYPE_3D;
            break;
    
    default:
        // cast a error:
        break;
    }

    // create teh image handle
    result = vkCreateImage( device, &imageCI, k_allocationCallbacks, &m_imageHandle );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkImage::Create::vkCreateImage", result );
        return false;
    }
    
    vkGetImageMemoryRequirements( device, m_imageHandle, &memReq );
    
    ///
    /// Allocate image for the memory 
    /// ==========================================================================
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device->FindMemoryType( memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    result = vkAllocateMemory( device, &allocInfo, nullptr, &m_imageMemory ); 
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkImage::Create::vkAllocateMemory", result );
        return false;
    }

    // bind image to memory 
    vkBindImageMemory( device, m_imageHandle, m_imageMemory, 0 );

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
    viewCI.image = m_imageHandle;
    viewCI.viewType = in_type;
    viewCI.format = in_format;

    result = vkCreateImageView( device, &viewCI, k_allocationCallbacks, &m_imageView );
    if( result != VK_SUCCESS )
    {
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
    if ( m_imageView != nullptr )
    {
        vkDestroyImageView( m_device->Device(), m_imageView, k_allocationCallbacks );
        m_imageView = nullptr;
    }

    if ( m_imageMemory != nullptr )
    {
        vkFreeMemory( m_device->Device(), m_imageMemory, k_allocationCallbacks );
        m_imageMemory = nullptr;
    }
    
    if ( m_imageHandle != nullptr )
    {
        vkDestroyImage( m_device->Device(), m_imageHandle, k_allocationCallbacks );
        m_imageHandle = nullptr;
    }
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
bool crvkImageStatic::Create(const crvkDevice *in_device, const VkImageViewType in_type, const VkFormat in_format, const uint16_t in_levels, const uint16_t in_layers, const uint32_t in_width, const uint32_t in_height, const uint32_t in_depth)
{
    VkResult result = VK_SUCCESS;
    if ( !crvkImage::Create( in_device, in_type, in_format, in_levels, in_layers, in_width, in_height, in_depth ) )
        return false;
    
    auto device = m_device->Device();

    ///
    /// Create the Command buffer to record the buffer transfer operations
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    if( m_device->HasTransferQueue() )
        allocInfo.commandPool = m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER)->CommandPool();
    else
        allocInfo.commandPool = m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool();

    result = vkAllocateCommandBuffers( device, &allocInfo, &m_commandBuffer );
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

    result = vkCreateSemaphore( device, &copySemaphoreCI, k_allocationCallbacks, &m_copySemaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkCreateSemaphore::COPY", result );
        return false;
    }

    VkSemaphoreCreateInfo drawSemaphoreCI{};
    drawSemaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawSemaphoreCI.flags = 0;
    drawSemaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( device, &drawSemaphoreCI, k_allocationCallbacks, &m_useSemaphore );
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
        vkDestroySemaphore( m_device->Device(), m_useSemaphore, k_allocationCallbacks );
        m_useSemaphore = nullptr;
    }
    
    if ( m_copySemaphore != nullptr )
    {
        vkDestroySemaphore( m_device->Device(), m_copySemaphore, k_allocationCallbacks );
        m_copySemaphore = nullptr;
    }

      // release the buffer operation command buffer 
    if ( m_commandBuffer != nullptr )
    {
        if ( m_device->HasComputeQueue() )
            vkFreeCommandBuffers( m_device->Device(), m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER)->CommandPool(), 1, &m_commandBuffer );
        else
            vkFreeCommandBuffers( m_device->Device(), m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool(), 1, &m_commandBuffer );
            
        m_commandBuffer = nullptr;
    }

    crvkImage::Destroy();
}

void crvkImageStatic::CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count )
{
    VkResult result = VK_SUCCESS;
    uint32_t minMip = UINT32_MAX;
    uint32_t maxMip = 0;
    uint32_t minLayer = UINT32_MAX;
    uint32_t maxLayer = 0;
    crvkDeviceQueue* queue = nullptr;

    vkResetCommandBuffer( m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );

    // begin record image commands 
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if (result != VK_SUCCESS) 
    {
        crvkAppendError("crvkImageStatic::CopyFromBuffer::vkBeginCommandBuffer", result );
        return;
    }

    // image barriers
    VkImageSubresourceRange imageSubresourceRange{};
    imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    // get the ranges
    for (uint32_t i = 0; i < in_count; ++i) 
    {
        const auto& s = in_copyRegions[i].imageSubresource;
        imageSubresourceRange.aspectMask |= s.aspectMask;
        minMip = std::min( minMip, s.mipLevel );
        maxMip = std::max( maxMip, s.mipLevel );
        minLayer = std::min( minLayer, s.baseArrayLayer );
        maxLayer = std::max( maxLayer, s.baseArrayLayer + s.layerCount );
    }

    imageSubresourceRange.baseMipLevel = minMip;
    imageSubresourceRange.levelCount = maxMip - minMip + 1;
    imageSubresourceRange.baseArrayLayer = minLayer;
    imageSubresourceRange.layerCount = std::max( 1u, maxLayer - minLayer );

    // Layout transition UNDEFINED → TRANSFER_DST_OPTIMAL
    VkImageMemoryBarrier2 imageBarrier{};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = m_imageHandle;
    imageBarrier.subresourceRange = imageSubresourceRange;  
    imageBarrier.srcAccessMask = 0;
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.pNext = nullptr;

    VkDependencyInfo    dependency{};
    dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency.dependencyFlags = 0;
    dependency.memoryBarrierCount = 0;
    dependency.pMemoryBarriers = nullptr;
    dependency.bufferMemoryBarrierCount = 0;
    dependency.pBufferMemoryBarriers = nullptr;
    dependency.imageMemoryBarrierCount = 1;
    dependency.pImageMemoryBarriers = &imageBarrier;
    dependency.pNext = nullptr;
    vkCmdPipelineBarrier2( m_commandBuffer, &dependency );
    
    // Copia do staging buffer para a imagem
    VkCopyBufferToImageInfo2 copyToImageInfo{};
    copyToImageInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2;
    copyToImageInfo.srcBuffer = in_srcBuffer;    
    copyToImageInfo.dstImage = m_imageHandle;
    copyToImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copyToImageInfo.regionCount = in_count;
    copyToImageInfo.pRegions = in_copyRegions;
    copyToImageInfo.pNext = nullptr;

    vkCmdCopyBufferToImage2( m_commandBuffer, &copyToImageInfo );

    // Layout transition LAYOUT_SHADER_READ_ONLY_OPTIMAL
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependency.imageMemoryBarrierCount = 1;
    dependency.pImageMemoryBarriers = &imageBarrier;
    vkCmdPipelineBarrier2( m_commandBuffer, &dependency );

    // Finaliza e envia o comando
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
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );       

    result = queue->Submit( waitInfo, 2, &submitInfo, 1, &signalInfo, 1, nullptr );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkQueueSubmit2", result );
        return;
    }
}

void crvkImageStatic::CopyToBuffer()
{
}

//=======================================================================================================================


/*
==============================================
crvkImageStaging::crvkImageStaging
==============================================
*/
void crvkImageStaging::Destroy(void)
{
    if( m_staging != nullptr )
    {
        vkDestroyBuffer( m_device->Device(), m_staging, k_allocationCallbacks );
        m_staging = nullptr;
    };

    if ( m_memoryStaging != nullptr )
    {
        vkFreeMemory( m_device->Device(), m_memoryStaging, k_allocationCallbacks );
        m_memoryStaging = nullptr;
    }

    crvkImageStatic::Destroy();
}

void crvkImageStaging::SubData(const void *in_data, const uintptr_t in_offset, const size_t in_size)
{
    // copy the content to the image stagin buffer 
    void* mappedData = nullptr;
    vkMapMemory( m_device->Device(), m_memoryStaging, in_offset, in_size, 0, &mappedData);
    memcpy(mappedData, in_data, in_size );
    vkUnmapMemory( m_device->Device(), m_memoryStaging );

    // create a tem
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer( m_commandBuffer, &beginInfo);


}

void crvkImageStaging::GetSubData(void *in_data, const uintptr_t in_offset, const size_t in_size)
{
}
