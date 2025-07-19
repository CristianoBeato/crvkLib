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

crvkDevice::crvkDevice( void ) : m_physicalDevice( nullptr )
{
}

crvkDevice::~crvkDevice( void )
{
    
}


void crvkDevice::Destroy( void )
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

VkExtent2D crvkDevice::FindExtent( const uint32_t in_width, const uint32_t in_height )
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

VkPresentModeKHR crvkDevice::FindPresentMode( const VkPresentModeKHR in_presentMode )
{
    for ( uint32_t i = 0; i < m_presentModes.Count(); i++)
    {
        if ( m_presentModes[i] == in_presentMode )
            return in_presentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR crvkDevice::FindSurfaceFormat( const VkFormat in_format, const VkColorSpaceKHR in_colorSpace )
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

uint32_t crvkDevice::FindMemoryType( const uint32_t typeFilter, const VkMemoryPropertyFlags properties )
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
