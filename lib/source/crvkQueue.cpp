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
#include "crvkQueue.hpp"

typedef struct crvkQueueHandle_t
{
    crvkQueueType_t type;
    uint32_t        family = UINT32_MAX;
    uint32_t        index = UINT32_MAX;
    VkQueue         queue = nullptr;
    VkCommandPool   commandPool = nullptr;
    VkDevice        device = nullptr;
} crvkQueueHandle_t;

/*
==============================================
crvkDeviceQueue::crvkDeviceQueue
==============================================
*/
crvkDeviceQueue::crvkDeviceQueue( void ) : m_handle( nullptr )
{
}

/*
==============================================
crvkDeviceQueue::~crvkDeviceQueue
==============================================
*/
crvkDeviceQueue::~crvkDeviceQueue( void )
{
    Destroy();
}

/*
==============================================
crvkDeviceQueue::Create
==============================================
*/
bool crvkDeviceQueue::Create(const crvkDevice *in_device, const crvkQueueInfo_t *&in_deviceQueue)
{
    m_handle = static_cast<crvkQueueHandle_t*>( SDL_malloc( sizeof( crvkQueueHandle_t ) ) );
    m_handle->device = in_device->Device();
    m_handle->family = in_deviceQueue->family;
    m_handle->index = in_deviceQueue->index;

    // retrieve devique queue 
#if 0 
    vkGetDeviceQueue( m_handle->device, m_handle->family, m_handle->index, &m_handle->queue );
#else
    VkDeviceQueueInfo2 deviceQueueInfo{}; 
    deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    deviceQueueInfo.pNext = nullptr;
    deviceQueueInfo.flags = 0;
    deviceQueueInfo.queueFamilyIndex = m_handle->family;
    deviceQueueInfo.queueIndex = m_handle->index;
    vkGetDeviceQueue2( m_handle->device, &deviceQueueInfo, &m_handle->queue );
#endif 
    if ( m_handle->queue == nullptr )
        return false;

    // create queue command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_handle->family;
    auto result = vkCreateCommandPool( m_handle->device, &poolInfo, k_allocationCallbacks, &m_handle->commandPool ); 
    if ( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkDevice::Create::vkCreateCommandPool", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkDeviceQueue::
==============================================
*/
void crvkDeviceQueue::Destroy( void )
{
    if ( m_handle == nullptr )
        return;

    if ( m_handle->commandPool != nullptr )
    {
        vkDestroyCommandPool( m_handle->device, m_handle->commandPool, k_allocationCallbacks );
        m_handle->commandPool = nullptr;
    }

    // we just release queue handle
    if( m_handle->queue )
        m_handle->queue = nullptr;

    // free handle pointer
    SDL_free( m_handle );
    m_handle = nullptr;    
}

/*
==============================================
crvkDeviceQueue::Submit
==============================================
*/
VkResult crvkDeviceQueue::Submit(  
    const VkSemaphoreSubmitInfo* in_WaitSemaphoreInfos,
    const uint32_t in_waitSemaphoreInfoCount,
    const VkCommandBufferSubmitInfo* in_CommandBufferInfos,
    const uint32_t in_commandBufferInfoCount,
    const VkSemaphoreSubmitInfo* in_SignalSemaphoreInfos,
    const uint32_t in_signalSemaphoreInfoCount,
    const VkFence in_fence )
{
    // 
    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.waitSemaphoreInfoCount = in_waitSemaphoreInfoCount;
    submitInfo.pWaitSemaphoreInfos = in_WaitSemaphoreInfos;
    submitInfo.commandBufferInfoCount = in_commandBufferInfoCount;
    submitInfo.pCommandBufferInfos = in_CommandBufferInfos;
    submitInfo.signalSemaphoreInfoCount = in_signalSemaphoreInfoCount;
    submitInfo.pSignalSemaphoreInfos = in_SignalSemaphoreInfos;
    return vkQueueSubmit2( m_handle->queue, 1, &submitInfo, in_fence );
}

/*
==============================================
crvkDeviceQueue::Present
==============================================
*/
VkResult crvkDeviceQueue::Present( 
        const VkSwapchainKHR* in_swapchains,
        const uint32_t* in_imageIndices,
        const uint32_t in_swapchainCount,
        const VkSemaphore* in_waitSemaphores,
        const uint32_t in_waitSemaphoresCount )
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = in_waitSemaphoresCount;
    presentInfo.pWaitSemaphores = in_waitSemaphores;
    presentInfo.swapchainCount = in_swapchainCount;
    presentInfo.pSwapchains = in_swapchains;
    presentInfo.pImageIndices = in_imageIndices;
    return vkQueuePresentKHR( m_handle->queue, &presentInfo );
}

/*
==============================================
crvkDeviceQueue::WaitIdle
==============================================
*/
VkResult crvkDeviceQueue::WaitIdle( void ) const
{
    if ( m_handle == nullptr )
        return VK_ERROR_INITIALIZATION_FAILED;    

    return vkQueueWaitIdle( m_handle->queue );
}

/*
==============================================
crvkDeviceQueue::Family
==============================================
*/
uint32_t crvkDeviceQueue::Family( void ) const
{
    if ( m_handle == nullptr )
        return UINT32_MAX;

    // Devie Queue family
    return m_handle->family;
}

/*
==============================================
crvkDeviceQueue::Index
==============================================
*/
uint32_t crvkDeviceQueue::Index( void ) const
{
    if ( m_handle == nullptr )
        return UINT32_MAX;

    return m_handle->index;
}

/*
==============================================
crvkDeviceQueue::Queue
==============================================
*/
VkQueue crvkDeviceQueue::Queue( void ) const
{
    if ( m_handle == nullptr )
        return nullptr;

    return m_handle->queue;
}

/*
==============================================
crvkDeviceQueue::CommandPool
==============================================
*/
VkCommandPool crvkDeviceQueue::CommandPool( void ) const
{
    if ( m_handle == nullptr )
        return nullptr;

    return m_handle->commandPool;
}