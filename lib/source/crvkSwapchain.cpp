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
#include "crvkSwapchain.hpp"

crvkSwapchain::crvkSwapchain( void ) : 
    m_imageCount( 0 ),
    m_swapChain( nullptr )
{
}

crvkSwapchain::~crvkSwapchain( void )
{
}

bool crvkSwapchain::Create( 
                    const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate = false )
{
    VkResult result = VK_SUCCESS;
    uint32_t queueFamilyIndices[2] { UINT32_MAX, UINT32_MAX };
    VkSwapchainKHR oldSwapchain = nullptr;
    VkDevice device = nullptr;
    m_extent = in_extent;
    device = m_device->Device();
    m_device = const_cast<crvkDevice*>( in_device );

    if ( in_recreate )
        oldSwapchain = m_swapChain;
    
    ///
    /// Create Swapchain and Imageview 
    /// ==========================================================================
    VkSwapchainCreateInfoKHR swapchainCI{};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.surface = in_context->Surface();
    
    swapchainCI.minImageCount = in_frames;
    swapchainCI.imageFormat = in_surfaceformat.format;
    swapchainCI.imageColorSpace = in_surfaceformat.colorSpace;
    swapchainCI.imageExtent = m_extent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto present = m_device->GetQueue( CRVK_DEVICE_QUEUE_PRESENT ); // get the present queue
    if ( present == nullptr )
    {
        crvkAppendError( "crvkSwapchainDynamic::Create::NO PRESENT QUEUE FOUND", VK_INCOMPLETE );
        return false;
    }
    
    auto graphic = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS ); // get the graphic qeue
    if ( graphic == nullptr )
    {
        crvkAppendError( "crvkSwapchainDynamic::Create::NO GRAPHIC QUEUE FOUND", VK_INCOMPLETE );
        return false;
    }

    queueFamilyIndices[0] = present->Family();
    queueFamilyIndices[1] = graphic->Family();
    
    // we hava a independent present queue 
    if (queueFamilyIndices[0] != queueFamilyIndices[1] ) 
    {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCI.queueFamilyIndexCount = 2;
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices;
    } 
    else 
    {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 1;
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices;
    }

    swapchainCI.preTransform = in_surfaceTransformFlag;
    swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCI.presentMode = in_presentMode;
    swapchainCI.clipped = VK_TRUE;
    swapchainCI.oldSwapchain = oldSwapchain;

    result = vkCreateSwapchainKHR( device, &swapchainCI, k_allocationCallbacks, &m_swapChain );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkSwapchainDynamic::Create::vkCreateSwapchainKHR", result );
        return false;
    }

    // Agora sim pode destruir a antiga
    if ( in_recreate && oldSwapchain != VK_NULL_HANDLE ) 
    {
        vkDestroySwapchainKHR( device, oldSwapchain, k_allocationCallbacks );
    }

    vkGetSwapchainImagesKHR( device, m_swapChain, &m_imageCount, nullptr );
    m_images.Resize( m_imageCount );
    vkGetSwapchainImagesKHR( device, m_swapChain, &m_imageCount, &m_images );
    
    m_imageViews.Resize( m_imageCount );
    m_imageAvailable.Resize( m_imageCount );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = in_surfaceformat.format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    for ( uint32_t i = 0; i < m_imageCount; i++) 
    {
    
        // destroy old view 
        if ( in_recreate )
            vkDestroyImageView( device, m_imageViews[i], k_allocationCallbacks );
    
        createInfo.image = m_images[i];    
        result = vkCreateImageView( device, &createInfo, k_allocationCallbacks, &m_imageViews[i] ); 
        if ( result != VK_SUCCESS ) 
        {
            crvkAppendError( "crvkSwapchainDynamic::Create::vkCreateImageView", result );
            return false;
        }

        result = vkCreateSemaphore( m_device->Device(), &semaphoreInfo, k_allocationCallbacks, &m_imageAvailable[i] ); 
        if( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchainDynamic::Create::vkCreateSemaphore", result );
            return false;
        }

    }
}

/*
==============================================
crvkSwapchain::Destroy
==============================================
*/
void crvkSwapchain::Destroy( void )
{
    // release buffers and images 
    for ( uint32_t i = 0; i < m_imageCount; i++)
    {
        vkDestroySemaphore( m_device->Device(), m_imageAvailable[i], k_allocationCallbacks );
        vkDestroyImageView( m_device->Device(), m_imageViews[i], k_allocationCallbacks );
    }

    // destoy the frander pass configuration
    if ( m_swapChain != nullptr )
    {
        vkDestroySwapchainKHR( m_device->Device(), m_swapChain, k_allocationCallbacks );
        m_swapChain = nullptr;
    }

    m_imageViews.Clear();
    m_images.Clear();
    m_device = nullptr;
}

/*
==============================================
crvkSwapchain::AcquireImage
==============================================
*/
VkResult crvkSwapchain::AcquireImage( void )
{    
    //
    // Aquire the current frame image idex
    //
    VkAcquireNextImageInfoKHR   acquireNextImageInfo{};
    acquireNextImageInfo.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
    acquireNextImageInfo.pNext = nullptr;
    acquireNextImageInfo.swapchain = Swapchain();
    acquireNextImageInfo.timeout = UINT64_MAX;
    acquireNextImageInfo.semaphore = m_imageAvailable[m_currentImage];
    acquireNextImageInfo.fence = nullptr;
    acquireNextImageInfo.deviceMask = 0;
    return vkAcquireNextImage2KHR( m_device->Device(), &acquireNextImageInfo, &m_currentImage );
}

/*
==============================================
crvkSwapchain::PresentImage
==============================================
*/
VkResult crvkSwapchain::PresentImage( const VkSemaphore* in_waitSemaphores, const uint32_t in_waitSemaphoresCount )
{
    VkResult result = VK_SUCCESS;
    auto present = m_device->GetQueue( CRVK_DEVICE_QUEUE_PRESENT ); // get the present queue

    // present to the window
    present->Present( &m_swapChain, &m_currentImage, 1, in_waitSemaphores, in_waitSemaphoresCount );
}

/*
==============================================
crvkSwapchain::Images
==============================================
*/
const VkImage* crvkSwapchain::Images( void ) const
{
    return m_images.Pointer();
}

/*
==============================================
crvkSwapchain::ImageViews
==============================================
*/
const VkImageView* crvkSwapchain::ImageViews( void ) const
{
    return m_imageViews.Pointer();
}

/*
==============================================
crvkSwapchainDynamic::crvkSwapchainDynamic
==============================================
*/
crvkSwapchainDynamic::crvkSwapchainDynamic( void ) : 
    m_frameCount( 0 )
{
}

/*
==============================================
crvkSwapchainDynamic::~crvkSwapchainDynamic
==============================================
*/
crvkSwapchainDynamic::~crvkSwapchainDynamic( void )
{
    Destroy();
}

/*
==============================================
crvkSwapchainDynamic::Create
==============================================
*/
bool crvkSwapchainDynamic::Create( 
                    const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate )
{
    uint32_t i = 0;
    VkResult result = VK_SUCCESS;
    m_frameCount = in_frames;
    
    if( crvkSwapchain::Create( 
        in_context, 
        in_device, 
        in_frames, 
        in_extent, 
        in_surfaceformat, 
        in_presentMode, 
        in_surfaceTransformFlag, 
        in_recreate ) )
        return false;

    ///
    /// Create Renderpass and Framebuffers 
    /// ==========================================================================
    
    uint32_t images = ImageCount();

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = in_surfaceformat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    ///
    /// Create Sync Objects 
    /// ==========================================================================
    m_imageAvailable.Resize( m_frameCount );
    m_renderFinished.Resize( m_frameCount );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (size_t i = 0; i < m_frameCount; i++) 
    {        
        result = vkCreateSemaphore( m_device->Device(), &semaphoreInfo, k_allocationCallbacks, &m_renderFinished[i]);
        if( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchainDynamic::Create::vkCreateSemaphore", result );
            return false;    
        }
    }

    ///
    /// Allocate command buffers
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_device->GetQueue(CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = in_frames;
    m_commandBuffers.Resize( in_frames );
    result = vkAllocateCommandBuffers( m_device->Device(), &allocInfo, &m_commandBuffers );
    if ( result != VK_SUCCESS ) 
    {   
        crvkAppendError( "crvkSwapchainDynamic::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    return true;
}
  
/*
==============================================
crvkSwapchainDynamic::Destroy
==============================================
*/
void crvkSwapchainDynamic::Destroy( void )
{
    uint32_t i = 0;

    if ( m_commandBuffers.Count() )
    {
        auto graphic = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );
        vkFreeCommandBuffers( m_device->Device(), graphic->CommandPool(), m_commandBuffers.Count(), &m_commandBuffers );
        m_commandBuffers.Clear();
    }

    for ( uint32_t i = 0; i < m_frameCount; i++)
    {
        if( m_renderFinished[i] != nullptr )
        {
            vkDestroySemaphore( m_device->Device(), m_renderFinished[i], k_allocationCallbacks );
        }
    }
    
    m_renderFinished.Clear();
    m_imageAvailable.Clear();
    m_commandBuffers.Clear();
    
    // release swapchains
    crvkSwapchain::Destroy();
}

/*
==============================================
crvkSwapchainDynamic::Begin
==============================================
*/
VkResult crvkSwapchainDynamic::Begin( const VkClearColorValue in_clearColor, const VkClearDepthStencilValue in_clearDepthStencil )
{
    VkResult result = VK_SUCCESS;
    
    result = AcquireImage();
    if ( result != VK_SUCCESS )
        return result;

    // Wait for previous render to finish ( replaced the fence )
    VkSemaphoreWaitInfo waitRender{};
    waitRender.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
    waitRender.semaphoreCount = 1,
    waitRender.pSemaphores = &m_renderFinished[m_frame];
    waitRender.pValues = nullptr;
    result = vkWaitSemaphores( m_device->Device(), &waitRender, UINT64_MAX );

    result = vkResetCommandBuffer( m_commandBuffers[m_frame], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if( result != VK_SUCCESS )
        crvkAppendError( "crvkCommandBufferRoundRobin::Begin::vkResetCommandBuffer", result );

    // begin register the commands
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffers[m_frame], &beginInfo );

    // clear color attachament
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = CurrentImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = in_clearColor;
    colorAttachment.clearValue.depthStencil = in_clearDepthStencil;

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = { {0, 0}, m_extent };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    vkCmdBeginRendering( m_commandBuffers[m_frame], &renderingInfo);

    return result;
}

/*
==============================================
crvkSwapchainDynamic::End
==============================================
*/
VkResult crvkSwapchainDynamic::End( void )
{
    VkResult result = VK_SUCCESS;
    auto graphic = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS ); // get the graphic qeue

    // register the frame end
    vkCmdEndRendering( m_commandBuffers[m_frame] );
    
    // submit swap chain command buffer 
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = { m_imageAvailable[m_frame] };
    VkPipelineStageFlags waitStages[] = {};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_frame]; 

    VkSemaphore signalSemaphores[] = { m_renderFinished[m_frame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkSemaphoreSubmitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitInfo.semaphore = m_imageAvailable[m_frame];
    waitInfo.stageMask = /* VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT */ VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    waitInfo.value = 0;
    waitInfo.pNext = nullptr;

    // signal wen we done the rendergin
    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.semaphore = m_renderFinished[m_frame];
    signalInfo.stageMask = /* VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT */ VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    signalInfo.value = 0;
    signalInfo.pNext = nullptr;

    // sumit draw command buffer 
    VkCommandBufferSubmitInfo commandBuffersSubmitInfo{};
    commandBuffersSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBuffersSubmitInfo.pNext = nullptr;
    commandBuffersSubmitInfo.commandBuffer = m_commandBuffers[m_frame];
    commandBuffersSubmitInfo.deviceMask = 0;
    result = graphic->Submit( &waitInfo, 1, &commandBuffersSubmitInfo, 1, &signalInfo, 1, nullptr );
    if( result != VK_SUCCESS )
        return result;
}

/*
==============================================
crvkSwapchainDynamic::SwapBuffers
==============================================
*/
VkResult crvkSwapchainDynamic::SwapBuffers( void )
{
    VkResult result = VK_SUCCESS;
    auto present = m_device->GetQueue( CRVK_DEVICE_QUEUE_PRESENT ); // get the present queue

    // present to the window
    PresentImage( &m_renderFinished[m_frame], 1 );

    // swap frame 
    m_frame = ( m_frame + 1) % m_frameCount;

    return result;
}
