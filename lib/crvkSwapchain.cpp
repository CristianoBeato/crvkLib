// ===============================================================================================
// crvkPointer - Vulkan + SDL minimal framework
// Copyright (c) 2025 Beato
//
// This file is part of the crvkCore library and is licensed under the
// MIT License with Attribution Requirement.
//
// You are free to use, modify, and distribute this file (even commercially),
// as long as you give credit to the original author:
//
//     “Based on crvkCore by Beato – https://github.com/seuusuario/crvkCore”
//
// For full license terms, see the LICENSE file in the root of this repository.
// ===============================================================================================

#include "crvkPrecompiled.hpp"
#include "crvkSwapchain.hpp"

/*
==============================================
crvkSwapchain::crvkSwapchain
==============================================
*/
crvkSwapchain::crvkSwapchain( void ) : 
    m_currentFrame( 0 ),
    m_imageIndex( 0 ),
    m_imageCount( 0 ),
    m_frameCount( 0 ),
    m_swapChain( nullptr ),
    m_renderPass( nullptr ),
    m_device( nullptr )
{
}

/*
==============================================
crvkSwapchain::~crvkSwapchain
==============================================
*/
crvkSwapchain::~crvkSwapchain( void )
{
    Destroy();
}

/*
==============================================
crvkSwapchain::Create
==============================================
*/
bool crvkSwapchain::Create( 
                    const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate )
{
    VkResult result = VK_SUCCESS;
    uint32_t i = 0;
    VkDevice device = nullptr;
    VkSwapchainKHR oldSwapchain = nullptr;
    uint32_t queueFamilyIndices[2] { UINT32_MAX, UINT32_MAX };
    m_frameCount = in_frames;

    m_device = const_cast<crvkDevice*>( in_device );
    device = m_device->Device();

    if ( in_recreate )
        oldSwapchain = m_swapChain;
    
    ///
    /// Create Swapchain and Imageview 
    /// ==========================================================================
    
    VkSwapchainCreateInfoKHR swapchainCI{};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.surface = in_context->Surface();
    
    swapchainCI.minImageCount = m_frameCount;
    swapchainCI.imageFormat = in_surfaceformat.format;
    swapchainCI.imageColorSpace = in_surfaceformat.colorSpace;
    swapchainCI.imageExtent = in_extent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto present = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT ); // get the present queue
    if ( present == nullptr )
    {
        crvkAppendError( "crvkSwapchain::Create::NO PRESENT QUEUE FOUND", VK_INCOMPLETE );
        return false;
    }
    
    auto graphic = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS ); // get the graphic qeue
    if ( graphic == nullptr )
    {
        crvkAppendError( "crvkSwapchain::Create::NO GRAPHIC QUEUE FOUND", VK_INCOMPLETE );
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
        crvkAppendError( "crvkSwapchain::Create::vkCreateSwapchainKHR", result );
        return false;
    }

    // Agora sim pode destruir a antiga
    if ( in_recreate && oldSwapchain != VK_NULL_HANDLE ) 
    {
        vkDestroySwapchainKHR( device, oldSwapchain, k_allocationCallbacks );
    }

    vkGetSwapchainImagesKHR( device, m_swapChain, &m_imageCount, nullptr );
    m_images.Alloc( m_imageCount );
    vkGetSwapchainImagesKHR( device, m_swapChain, &m_imageCount, &m_images );
    
    m_imageViews.Alloc( m_imageCount );
    for ( uint32_t i = 0; i < m_imageCount; i++) 
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
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

        // destroy old view 
        if ( in_recreate )
            vkDestroyImageView( device, m_imageViews[i], k_allocationCallbacks );
        
        result = vkCreateImageView( m_device->Device(), &createInfo, k_allocationCallbacks, &m_imageViews[i] ); 
        if ( result != VK_SUCCESS ) 
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateImageView", result );
            return false;
        }
    }

    ///
    /// Create Renderpass and Framebuffers 
    /// ==========================================================================
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

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if ( !in_recreate && !m_renderPass )
    {   
        result = vkCreateRenderPass( device, &renderPassInfo, nullptr, &m_renderPass ); 
        if ( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateFramebuffer", result );
            return false;
        }
    }

    m_framebuffers.Alloc( m_imageCount );
    for ( i = 0; i < m_imageCount; i++) 
    {
        VkImageView attachments[] = 
        {
            m_imageViews[i]
        };

        VkFramebufferCreateInfo framebufferCI{};
        framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCI.renderPass = m_renderPass;
        framebufferCI.attachmentCount = 1;
        framebufferCI.pAttachments = attachments;
        framebufferCI.width = in_extent.width;
        framebufferCI.height = in_extent.height;
        framebufferCI.layers = 1;

        if ( in_recreate )
            vkDestroyFramebuffer( device, m_framebuffers[i], nullptr );

        result = vkCreateFramebuffer( device, &framebufferCI, nullptr, &m_framebuffers[i] );
        if ( result != VK_SUCCESS ) 
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateFramebuffer", result );
            return false;
        }
    }

    ///
    /// Create Sync Objects 
    /// ==========================================================================
    m_imageAvailableSemaphores.Alloc( m_frameCount );
    m_renderFinishedSemaphores.Alloc( m_frameCount );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_frameCount; i++) 
    {
        result = vkCreateSemaphore( device, &semaphoreInfo, k_allocationCallbacks, &m_imageAvailableSemaphores[i] ); 
        if( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateSemaphore", result );
            return false;
        }
        
        result = vkCreateSemaphore( device, &semaphoreInfo, k_allocationCallbacks, &m_renderFinishedSemaphores[i]);
        if( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateSemaphore", result );
            return false;    
        }
    }

    return true;
}
  
/*
==============================================
crvkSwapchain::Destroy
==============================================
*/
void crvkSwapchain::Destroy( void )
{
    uint32_t i = 0;

    // release buffers and images 
    for ( i = 0; i < m_imageCount; i++)
    {
        vkDestroyFramebuffer( m_device->Device(), m_framebuffers[i], k_allocationCallbacks );
        vkDestroyImageView( m_device->Device(), m_imageViews[i], k_allocationCallbacks );
    }
    
    m_framebuffers.Free();
    m_imageViews.Free();

    // release semaphores and fences
    for ( i = 0; i < m_frameCount; i++ )
    {
        vkDestroySemaphore( m_device->Device(), m_renderFinishedSemaphores[i], k_allocationCallbacks );
        vkDestroySemaphore( m_device->Device(), m_imageAvailableSemaphores[i], k_allocationCallbacks );
    }
    
    m_renderFinishedSemaphores.Free();
    m_imageAvailableSemaphores.Free();

    // destroy render pass configuration
    if ( m_renderPass != nullptr )
    {
        vkDestroyRenderPass( m_device->Device(), m_renderPass, k_allocationCallbacks );
        m_renderPass = nullptr;
    }
    
    // destoy the frander pass configuration
    if ( m_swapChain != nullptr )
    {
        vkDestroySwapchainKHR( m_device->Device(), m_swapChain, k_allocationCallbacks );
        m_swapChain = nullptr;
    }
    
    m_device = nullptr;
}

/*
==============================================
crvkSwapchain::Begin
==============================================
*/
VkResult crvkSwapchain::Begin( void )
{
    VkResult result = VK_SUCCESS;
    result = vkAcquireNextImageKHR( m_device->Device(), m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex );
    if ( result != VK_SUCCESS )
        return result;

    // Wait for previous render to finish ( replaced the fence )
    VkSemaphoreWaitInfo waitRender{};
    waitRender.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
    waitRender.semaphoreCount = 1,
    waitRender.pSemaphores = &m_renderFinishedSemaphores[m_currentFrame];
    waitRender.pValues = nullptr;
    result = vkWaitSemaphores( m_device->Device(), &waitRender, UINT64_MAX );

    return result;
}

/*
==============================================
crvkSwapchain::End
==============================================
*/
VkResult crvkSwapchain::End( const VkCommandBuffer* in_commandBuffers, const uint32_t in_commandBufferCount )
{
    VkResult result = VK_SUCCESS;
    auto graphic = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS ); // get the graphic qeue

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    VkPipelineStageFlags waitStages[] = {};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = in_commandBufferCount;
    submitInfo.pCommandBuffers = in_commandBuffers; 

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkSemaphoreSubmitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitInfo.semaphore = m_imageAvailableSemaphores[m_currentFrame];
    waitInfo.stageMask = /* VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT */ VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    waitInfo.value = 0;
    waitInfo.pNext = nullptr;

    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.semaphore = m_renderFinishedSemaphores[m_currentFrame];
    signalInfo.stageMask = /* VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT */ VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    signalInfo.value = 0;
    signalInfo.pNext = nullptr;

    crvkDynamicVector<VkCommandBufferSubmitInfo> commandBuffersSubmitInfo{};
    commandBuffersSubmitInfo.Resize( in_commandBufferCount );
    for ( uint32_t i = 0; i < in_commandBufferCount; i++)
    {
        commandBuffersSubmitInfo[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBuffersSubmitInfo[i].deviceMask = 0;
        commandBuffersSubmitInfo[i].commandBuffer = in_commandBuffers[i];
        commandBuffersSubmitInfo[i].pNext = nullptr;
    }
    
    result = graphic->Submit( &waitInfo, 1, &commandBuffersSubmitInfo, in_commandBufferCount, &signalInfo, 1, nullptr );
    if( result != VK_SUCCESS )
        return result;
}

/*
==============================================
crvkSwapchain::SwapBuffers
==============================================
*/
VkResult crvkSwapchain::SwapBuffers( void )
{
    VkResult result = VK_SUCCESS;
    auto present = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT ); // get the present queue

    // present to the window
    uint32_t swapChainsImageIndex[]{ m_imageIndex };
    VkSwapchainKHR swapChains[1]{ m_swapChain };
    present->Present( swapChains, swapChainsImageIndex, 1, &m_renderFinishedSemaphores[m_currentFrame], 1 );

    // Signals acquisition of the next image (now that it has been presented)
    VkSemaphoreSignalInfo signalAcquire{};
    signalAcquire.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalAcquire.semaphore = m_imageAvailableSemaphores[m_currentFrame];
    signalAcquire.value = 0;
    vkSignalSemaphore( m_device->Device(), &signalAcquire );

    // swap frame 
    m_currentFrame = ( m_currentFrame + 1) % m_frameCount;

    return result;
}
