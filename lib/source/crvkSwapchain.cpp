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

typedef struct crvkSwapchainHandle_t
{
    uint32_t                        numFrames = 0;              // available frames to render 
    uint32_t                        numImages = 0;              // availabe swapchain images
    uint32_t                        currentImage = 0;           // current frame buffer
    uint64_t                        frame = 0;                  // our internal frame count
    VkExtent2D                      extent;                     // swapchain image size ( screen frame buffer size )
    VkImage*                        imageArray = nullptr;       // images from the swapchain 
    VkImageView*                    viewArray = nullptr;        // image view from swapchain
    VkSemaphore*                    imageAvailable = nullptr;   // semaphore images
    VkFence*                        frameFences = nullptr;
    VkSwapchainKHR                  swapchain = nullptr;        // swapchain handle 
    VkQueue                         presentQueue = nullptr;     // device present queue
    VkDevice                        device = nullptr;           // device handle
} crvkSwapchainHandle_t;

crvkSwapchain::crvkSwapchain( void ) : m_handle( nullptr )
{
    m_handle = new crvkSwapchainHandle_t();
}

crvkSwapchain::~crvkSwapchain( void )
{
    Destroy();
    if ( m_handle != nullptr )
    {
        delete m_handle;
        m_handle = nullptr;
    }
    
}

bool crvkSwapchain::Create( 
                    const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const crvkDeviceQueue* in_graphic,
                    const crvkDeviceQueue* in_present,
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate = false )
{
    uint32_t i = 0;
    VkResult result = VK_SUCCESS;
    uint32_t queueFamilyIndices[2] { UINT32_MAX, UINT32_MAX };
    VkSwapchainKHR oldSwapchain = nullptr;
    
    m_handle->numFrames = in_frames;
    m_handle->extent = in_extent;
    m_handle->device = in_device->Device();

    if ( in_present == nullptr )
    {
        crvkAppendError( "crvkSwapchain::Create::NO PRESENT QUEUE FOUND", VK_INCOMPLETE );
        return false;
    }
    
    if ( in_graphic == nullptr )
    {
        crvkAppendError( "crvkSwapchain::Create::NO GRAPHIC QUEUE FOUND", VK_INCOMPLETE );
        return false;
    }

    queueFamilyIndices[0] = in_present->Family();
    queueFamilyIndices[1] = in_graphic->Family();
    m_handle->presentQueue = in_present->Queue();

    if ( in_recreate )
    {
        oldSwapchain = m_handle->swapchain;

        // wait we for finish evetirthing before we recreate the swap chain
        vkDeviceWaitIdle( m_handle->device );
    }
    
    ///
    /// Create Swapchain and Imageview 
    /// ==========================================================================
    VkSwapchainCreateInfoKHR swapchainCI{};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.surface = in_context->Surface();
    
    swapchainCI.minImageCount = in_frames;
    swapchainCI.imageFormat = in_surfaceformat.format;
    swapchainCI.imageColorSpace = in_surfaceformat.colorSpace;
    swapchainCI.imageExtent = m_handle->extent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
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

    result = vkCreateSwapchainKHR( m_handle->device, &swapchainCI, k_allocationCallbacks, &m_handle->swapchain );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkSwapchain::Create::vkCreateSwapchainKHR", result );
        return false;
    }

    // Agora sim pode destruir a antiga
    if ( in_recreate && oldSwapchain != VK_NULL_HANDLE )
        vkDestroySwapchainKHR( m_handle->device, oldSwapchain, k_allocationCallbacks );

    // Get the available image count 
    vkGetSwapchainImagesKHR( m_handle->device, m_handle->swapchain, &m_handle->numImages, nullptr );
    m_handle->imageArray = static_cast<VkImage*>( SDL_malloc( sizeof( VkImage ) * m_handle->numImages ) );
    m_handle->viewArray = static_cast<VkImageView*>( SDL_malloc( sizeof( VkImageView ) * m_handle->numImages ) );

    // Get the image array 
    vkGetSwapchainImagesKHR( m_handle->device, m_handle->swapchain, &m_handle->numImages, m_handle->imageArray );
    
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

    // create the swap chain views
    for ( i = 0; i < m_handle->numImages; i++) 
    {
    
        // destroy old view 
        if ( in_recreate )
            vkDestroyImageView( m_handle->device, m_handle->viewArray[i], k_allocationCallbacks );
    
        createInfo.image = m_handle->imageArray[i];    
        result = vkCreateImageView( m_handle->device, &createInfo, k_allocationCallbacks, &m_handle->viewArray[i] ); 
        if ( result != VK_SUCCESS ) 
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateImageView", result );
            return false;
        }
    }

    // if we are just recreating the swap chain we don't recreate fences and semaphores 
    if ( in_recreate && ( m_handle->imageAvailable != nullptr ) && ( m_handle->frameFences != nullptr) )
        return true;    

    //
    // Create race condition structures 
    //

    // Semaphore configuration
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    // Fence configuration
    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.pNext = nullptr;
    fenceCI.flags = 0;

    // alloc the structures arrays 
    m_handle->imageAvailable = static_cast<VkSemaphore*>( SDL_malloc( sizeof( VkSemaphore ) * m_handle->numFrames ) );
    m_handle->frameFences = static_cast<VkFence*>( SDL_malloc( sizeof( VkFence ) * m_handle->numFrames ) );
    for ( i = 0; i < m_handle->numFrames; i++)
    {
        // create the semaphore object
        result = vkCreateSemaphore( m_handle->device, &semaphoreInfo, k_allocationCallbacks, &m_handle->imageAvailable[i] ); 
        if( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateSemaphore", result );
            return false;
        }

        // create the fence object
        result = vkCreateFence( m_handle->device, &fenceCI, k_allocationCallbacks, &m_handle->frameFences[i] );
        if( result != VK_SUCCESS )
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateFence", result );
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
    
    if ( m_handle == nullptr )
        return;

    // release the semaphores
    for ( i = 0; i < m_handle->numFrames; i++)
    {
        vkDestroySemaphore( m_handle->device, m_handle->imageAvailable[i], k_allocationCallbacks );
        m_handle->imageArray[i] = nullptr;
    }
    
    // release image views
    for ( i = 0; i < m_handle->numImages; i++)
    {
        vkDestroyImageView( m_handle->device, m_handle->viewArray[i], k_allocationCallbacks );
        m_handle->viewArray[i] = nullptr;
    }

    // release swapchain 
    if ( m_handle->swapchain != nullptr )
    {
        vkDestroySwapchainKHR( m_handle->device, m_handle->swapchain, k_allocationCallbacks );
        m_handle->swapchain = nullptr;
    }

    // Release the view array
    if ( m_handle->viewArray != nullptr )
    {
        SDL_free( m_handle->viewArray );
        m_handle->viewArray = nullptr;
    }

    // Release the image array
    if ( m_handle->imageArray != nullptr )
    {
        SDL_free( m_handle->imageArray );
        m_handle->imageArray = nullptr;
    }    
}

/*
==============================================
crvkSwapchain::AcquireImage
==============================================
*/
VkResult crvkSwapchain::AcquireImage( void )
{
    VkResult result = VK_SUCCESS;
    uint32_t frameID = m_handle->frame % m_handle->numFrames;

    //
    // Wait for the device finish last render in previous match frame
    // 
    result = vkWaitForFences( m_handle->device, 1, &m_handle->frameFences[frameID], VK_TRUE, UINT64_MAX );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkSwapchain::AcquireImage::vkWaitForFences", result );
        return result;
    }

    //
    // Aquire the current frame image idex
    //
    VkAcquireNextImageInfoKHR   acquireNextImageInfo{};
    acquireNextImageInfo.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
    acquireNextImageInfo.pNext = nullptr;
    acquireNextImageInfo.swapchain = Swapchain();
    acquireNextImageInfo.timeout = UINT64_MAX;
    acquireNextImageInfo.semaphore = m_handle->imageAvailable[frameID];
    acquireNextImageInfo.fence = nullptr;
    acquireNextImageInfo.deviceMask = 0;
    result = vkAcquireNextImage2KHR( m_handle->device, &acquireNextImageInfo, &m_handle->currentImage );
    if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
    {
        crvkAppendError( "crvkSwapchain::AcquireImage::vkAcquireNextImage2KHR", result );
        return result;
    }

    // reset the fence only if we don't fid a problem in aquire the image  
    vkResetFences( m_handle->device, 1, &m_handle->frameFences[frameID] );
}

/*
==============================================
crvkSwapchain::PresentImage
==============================================
*/
VkResult crvkSwapchain::PresentImage( const VkSemaphore* in_waitSemaphores, const uint32_t in_waitSemaphoresCount )
{
    VkResult result = VK_SUCCESS;

    // present to the window
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = in_waitSemaphoresCount;
    presentInfo.pWaitSemaphores = in_waitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_handle->swapchain;
    presentInfo.pImageIndices = &m_handle->currentImage;
    vkQueuePresentKHR( m_handle->presentQueue, &presentInfo );

    // increment frame count
    m_handle->frame++; 
}

/*
==============================================
crvkSwapchain::Images
==============================================
*/
const VkImage* crvkSwapchain::Images( void ) const
{
    return m_handle->imageArray;
}

/*
==============================================
crvkSwapchain::ImageViews
==============================================
*/
const VkImageView* crvkSwapchain::ImageViews( void ) const
{
    return m_handle->viewArray;
}

/*
==============================================
crvkSwapchain::FrameCount
==============================================
*/
uint32_t crvkSwapchain::FrameCount( void ) const
{
    return m_handle->numFrames;
}

/*
==============================================
crvkSwapchain::ImageCount
==============================================
*/
uint32_t crvkSwapchain::ImageCount( void ) const
{
    return m_handle->numImages;
}

/*
==============================================
crvkSwapchain::CurrentImageID
==============================================
*/
uint32_t crvkSwapchain::CurrentImageID( void ) const
{
    return m_handle->currentImage;
}

/*
==============================================
crvkSwapchain::Extent
==============================================
*/
VkExtent2D crvkSwapchain::Extent( void ) const
{
    return m_handle->extent;
}