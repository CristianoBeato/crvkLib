// ===============================================================================================
// crvkContext - Vulkan + SDL minimal framework
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

#include "crvkImplement.hpp"
#include "crvkDevice.hpp"

#include <cstring>
#include <limits>
#include <algorithm>
#include <stdexcept>

crvkDeviceQueue::~crvkDeviceQueue( void )
{
    m_type = CRVK_DEVICE_QUEUE_NONE;
    m_family = UINT32_MAX;
    m_index = UINT32_MAX;
    m_queue = nullptr;
}

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

    return vkQueueSubmit2( m_queue, 1, &submitInfo, in_fence );
}

VkResult crvkDeviceQueue::Submit( const VkCommandBuffer* in_commandBuffers, const uint32_t in_commandBuffersCount )
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = in_commandBuffersCount;
    submitInfo.pCommandBuffers = in_commandBuffers;
    return vkQueueSubmit( m_queue, 1, &submitInfo, VK_NULL_HANDLE );
}

VkResult crvkDeviceQueue::WaitIdle( void ) const
{
    return vkQueueWaitIdle( m_queue );
}

crvkDeviceQueue::crvkDeviceQueue( const uint32_t in_family, const uint32_t in_index, const crvkQueueType in_type ) : 
    m_type( in_type ),
    m_family( in_family ), 
    m_index( in_index ) 
{
}

bool crvkDeviceQueue::InitQueue(const VkDevice in_device)
{
    vkGetDeviceQueue( in_device, m_family, m_index, &m_queue );
    return false;
}

crvkDevice::crvkDevice( void ) : m_physicalDevice( nullptr )
{
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS] = nullptr;
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT] = nullptr;
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE] = nullptr;
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER] = nullptr;
}

crvkDevice::~crvkDevice( void )
{   
}

bool crvkDevice::Create(const char **in_layers, const uint32_t in_layersCount, const char **in_deviceExtensions, const uint32_t in_deviceExtensionsCount)
{
    VkResult result = VK_SUCCESS;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCI{};
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCI.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCI.pQueueCreateInfos = queueCreateInfos.data();

    deviceCI.pEnabledFeatures = &deviceFeatures;

    deviceCI.enabledExtensionCount = in_deviceExtensionsCount;
    deviceCI.ppEnabledExtensionNames = const_cast<const char* const*>( in_deviceExtensions );

    if ( in_layersCount > 0 ) 
    {
        deviceCI.enabledLayerCount = in_layersCount;
        deviceCI.ppEnabledLayerNames = in_layers;
    } 
    else 
    {
        deviceCI.enabledLayerCount = 0;
        deviceCI.ppEnabledLayerNames = nullptr;
    }

    result = vkCreateDevice( m_physicalDevice, &deviceCI, &k_allocationCallbacks, &m_logicalDevice );
    if ( result != VK_SUCCESS) 
    {
        // throw std::runtime_error("failed to create logical context->Device()!");
        return false;
    }

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS]->Family();

    auto result = vkCreateCommandPool( m_logicalDevice, &poolInfo, &k_allocationCallbacks, &m_commandPool ); 
    if ( result != VK_SUCCESS) 
    {
        //throw std::runtime_error("failed to create command pool!");
        return false;
    }

    // get queues
    for ( uint32_t i = 0; i < 4; i++)
    {
        // ignore if not avaidable 
        if( m_queues[i] == nullptr )
            continue;

        m_queues[i]->InitQueue( m_logicalDevice );
    }

    return true;
}

void crvkDevice::Destroy(void)
{
    if ( m_commandPool != nullptr )
    {
        vkDestroyCommandPool( m_logicalDevice, m_commandPool, &k_allocationCallbacks );
        m_commandPool = nullptr;
    }
    
    if ( m_logicalDevice != nullptr )
    {
        vkDestroyDevice( m_logicalDevice, &k_allocationCallbacks );
        m_logicalDevice = nullptr;
    }
}

VkExtent2D crvkDevice::FindExtent( const uint32_t in_width, const uint32_t in_height ) const
{
#if VK_VERSION_1_2
    auto capabilities = m_surfaceCapabilities.surfaceCapabilities;
#else
    auto capabilities = m_surfaceCapabilities;
#endif

    VkExtent2D actualExtent = { in_width, in_height };

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return capabilities.currentExtent;
    } 
    else 
    {
        actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
    }
    
    return actualExtent;
}

VkPresentModeKHR crvkDevice::FindPresentMode( const VkPresentModeKHR in_presentMode ) const
{
    for ( uint32_t i = 0; i < m_presentModes.Count(); i++)
    {
        if ( m_presentModes[i] == in_presentMode )
            return in_presentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR crvkDevice::FindSurfaceFormat( const VkFormat in_format, const VkColorSpaceKHR in_colorSpace ) const
{ 
    for ( uint32_t i = 0; i < m_surfaceFormats.Count(); i++)
    {
#if VK_VERSION_1_2
        auto availableFormat = m_surfaceFormats[i].surfaceFormat;
#else
        auto m_surfaceFormats[i];
#endif
        if (availableFormat.format == in_format && availableFormat.colorSpace == in_colorSpace ) 
            return availableFormat;
    }

#if VK_VERSION_1_2
    return m_surfaceFormats[0].surfaceFormat;
#else
    return m_surfaceFormats[0];
#endif 
}

uint32_t crvkDevice::FindMemoryType( const uint32_t typeFilter, const VkMemoryPropertyFlags properties ) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &memProperties );
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
                return i;
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

crvkDeviceQueue* crvkDevice::GetQueue( const crvkDeviceQueue::crvkQueueType in_type ) const
{
    int i = (int)in_type;

    // out of range
    if ( i < 0 || i > 3 )
        return nullptr;
    
    // get queue
    return m_queues[i];
}

VkSurfaceCapabilitiesKHR crvkDevice::SurfaceCapabilities(void) const
{
#if VK_VERSION_1_2
    return m_surfaceCapabilities.surfaceCapabilities;
#else
    return m_surfaceCapabilities;
#endif
}

const bool crvkDevice::CheckExtensionSupport(const char *in_extension)
{
    bool found = false;
    for ( uint32_t i = 0; i < m_availableExtensions.Count(); i++)
    {   
        if ( std::strcmp( in_extension, m_availableExtensions[i].extensionName ) == 0 )
        {
            found = true;
            break;
        }
    }
    
    // TODO: print in verbose mode the needed 
    return found;
}

bool crvkDevice::InitDevice( const VkPhysicalDevice in_device, const VkSurfaceKHR in_surface )
{
    uint32_t queueFamilyCount = 0;
    uint32_t formatCount = 0;
    uint32_t extensionCount = 0;
    uint32_t presentModeCount = 0;

    m_physicalDevice = in_device;
    SDL_assert( m_physicalDevice != nullptr );

#if VK_VERSION_1_2
    VkPhysicalDeviceSurfaceInfo2KHR deviceSurfaceInfo{}; 
    deviceSurfaceInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    deviceSurfaceInfo.surface = in_surface;
    deviceSurfaceInfo.pNext = nullptr;

    // query device properties
    vkGetPhysicalDeviceProperties2( m_physicalDevice, &m_properties );

    // query device surface  capabilities
    vkGetPhysicalDeviceSurfaceCapabilities2KHR( m_physicalDevice, &deviceSurfaceInfo, &m_surfaceCapabilities );
    
    //
    vkGetPhysicalDeviceMemoryProperties2( m_physicalDevice, &m_memoryProperties );
#else
    // query device properties 
    vkGetPhysicalDeviceProperties( m_physicalDevice, &m_properties );

    // query device surface  capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_physicalDevice, in_surface, &m_surfaceCapabilities );

    // query device suported surface formats
    vkGetPhysicalDeviceSurfaceFormatsKHR( m_physicalDevice, in_surface, &formatCount, nullptr );
    m_surfaceFormats.Alloc( formatCount );
    vkGetPhysicalDeviceSurfaceFormatsKHR( m_physicalDevice, in_surface, &formatCount, &m_surfaceFormats );

    //
    vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &m_memoryProperties);
#endif

    // query device extensions
    vkEnumerateDeviceExtensionProperties( m_physicalDevice, nullptr, &extensionCount, nullptr );
    m_availableExtensions.Alloc( extensionCount );
    vkEnumerateDeviceExtensionProperties( m_physicalDevice, nullptr, &extensionCount, &m_availableExtensions );

    // query device suported surface presenting mode
    vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, in_surface, &presentModeCount, nullptr );
    m_presentModes.Alloc( presentModeCount );
    vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, in_surface, &presentModeCount, &m_presentModes );
    
    // query device queues
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &queueFamilyCount, nullptr );
    m_queueFamilies.Alloc( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &queueFamilyCount, &m_queueFamilies );

#if VK_KHR_copy_commands2
    m_deviceSuportedFeatures.copyCommands2Enabled = CheckExtensionSupport( VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME );
#endif 
}
