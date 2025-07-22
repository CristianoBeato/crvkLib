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

#include "crvkPrecompiled.hpp"
#include "crvkDevice.hpp"

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

    return vkQueueSubmit2( m_queue, 1, &submitInfo, in_fence );
}

/*
==============================================
crvkDeviceQueue::Submit
==============================================
*/
VkResult crvkDeviceQueue::Submit( 
    const VkCommandBuffer* in_commandBuffers, 
    const uint32_t in_commandBuffersCount, 
    const VkSemaphore* in_waitSemaphores,
    const uint32_t in_waitSemaphoresCount,
    const VkSemaphore* in_signalSemaphores,
    const uint32_t in_signalSemaphoresCount
)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = in_commandBuffersCount;
    submitInfo.pCommandBuffers = in_commandBuffers;
    submitInfo.pWaitSemaphores = in_waitSemaphores;
    submitInfo.waitSemaphoreCount = in_waitSemaphoresCount;
    submitInfo.pSignalSemaphores = in_signalSemaphores;
    submitInfo.signalSemaphoreCount = in_signalSemaphoresCount;

    return vkQueueSubmit( m_queue, 1, &submitInfo, VK_NULL_HANDLE );
}

/*
==============================================
crvkDeviceQueue::WaitIdle
==============================================
*/
VkResult crvkDeviceQueue::WaitIdle( void ) const
{
    return vkQueueWaitIdle( m_queue );
}

/*
==============================================
crvkDeviceQueue::crvkDeviceQueue
==============================================
*/
crvkDeviceQueue::crvkDeviceQueue( const uint32_t in_family, const uint32_t in_index, const crvkQueueType in_type ) : 
    m_type( in_type ),
    m_family( in_family ), 
    m_index( in_index ),
    m_queue( nullptr ),
    m_commandPool( nullptr )
{
}

/*
==============================================
crvkDeviceQueue::~crvkDeviceQueue
==============================================
*/
crvkDeviceQueue::~crvkDeviceQueue( void )
{
    m_type = CRVK_DEVICE_QUEUE_NONE;
    m_family = UINT32_MAX;
    m_index = UINT32_MAX;
    m_queue = nullptr;

    if ( m_commandPool != nullptr )
    {
        vkDestroyCommandPool( m_device, m_commandPool, k_allocationCallbacks );
        m_commandPool = nullptr;
    }

    m_device = nullptr;
}

/*
==============================================
crvkDeviceQueue::InitQueue
==============================================
*/
bool crvkDeviceQueue::InitQueue(const VkDevice in_device)
{
    m_device = in_device;

    vkGetDeviceQueue( m_device, m_family, m_index, &m_queue );

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_family;
    auto result = vkCreateCommandPool( in_device, &poolInfo, k_allocationCallbacks, &m_commandPool ); 
    if ( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkDevice::Create::vkCreateCommandPool", result );
        return false;
    }

    return m_queue != nullptr;
}

/*
==============================================
crvkDevice::crvkDevice
==============================================
*/
crvkDevice::crvkDevice( void ) :
    m_physicalDevice( nullptr ),
    m_logicalDevice( nullptr ),
    m_context( nullptr )
{
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS] = nullptr;
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT] = nullptr;
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE] = nullptr;
    m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER] = nullptr;
}

/*
==============================================
crvkDevice::~crvkDevice
==============================================
*/
crvkDevice::~crvkDevice( void )
{   
    Destroy();
    m_physicalDevice = nullptr;
    m_context = nullptr;
}

/*
==============================================
crvkDevice::Create
==============================================
*/
bool crvkDevice::Create(const char **in_layers, const uint32_t in_layersCount, const char **in_deviceExtensions, const uint32_t in_deviceExtensionsCount)
{
    VkResult result = VK_SUCCESS;
    crvkDynamicVector<VkDeviceQueueCreateInfo> queueCreateInfos;

    VkDeviceCreateInfo deviceCI{};
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    
    FindQueues( queueCreateInfos );
    
    deviceCI.queueCreateInfoCount = queueCreateInfos.Count();
    deviceCI.pQueueCreateInfos = &queueCreateInfos;
    
    // configure device features
    VkPhysicalDeviceFeatures deviceFeatures{};
#if VK_VERSION_1_2  
    m_featuresv13.pNext = &m_featuresv12;
    m_features.pNext = &m_featuresv13;    
    deviceCI.pNext = &m_features;
#else
    deviceCI.pEnabledFeatures = &m_deviceSuportedFeatures;
#endif

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
    
    result = vkCreateDevice( m_physicalDevice, &deviceCI, k_allocationCallbacks, &m_logicalDevice );
    if ( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkDevice::Create::vkCreateDevice", result );
        return false;
    }

    // get queues
    for ( uint32_t i = 0; i < 4; i++)
    {
        // ignore if not avaidable 
        if( m_queues[i] == nullptr )
            continue;

        if( !m_queues[i]->InitQueue( m_logicalDevice ) )
            return false;
    }

    return true;
}

/*
==============================================
crvkDevice::Destroy
==============================================
*/
void crvkDevice::Destroy(void)
{
    for ( uint32_t i = 0; i < 4; i++)
    {
        if ( m_queues[i] == nullptr )
            continue;

        delete m_queues[i];
        m_queues[i] = nullptr;
    }
    
    if ( m_logicalDevice != nullptr )
    {
        vkDestroyDevice( m_logicalDevice, k_allocationCallbacks );
        m_logicalDevice = nullptr;
    }

    m_context = nullptr;
}

/*
==============================================
crvkDevice::FindExtent
==============================================
*/
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

/*
==============================================
crvkDevice::FindSurfaceFormat
==============================================
*/
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

/*
==============================================
crvkDevice::FindMemoryType
==============================================
*/
uint32_t crvkDevice::FindMemoryType( const uint32_t typeFilter, const VkMemoryPropertyFlags properties ) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &memProperties );
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties ) == properties) 
                return i;
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

/*
==============================================
crvkDevice::GetQueue
==============================================
*/
crvkDeviceQueue* crvkDevice::GetQueue( const crvkDeviceQueue::crvkQueueType in_type ) const
{
    int i = (int)in_type;

    // out of range
    if ( i < 0 || i > 3 )
        return nullptr;
    
    // get queue
    return m_queues[i];
}

/*
==============================================
crvkDevice::HasPresentQueue
==============================================
*/
bool crvkDevice::HasPresentQueue(void) const
{
    return m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT];
}

/*
==============================================
crvkDevice::HasComputeQueue
==============================================
*/
bool crvkDevice::HasComputeQueue(void) const
{
    return m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE];
}

/*
==============================================
crvkDevice::HasTransferQueue
==============================================
*/
bool crvkDevice::HasTransferQueue(void) const
{
    return m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER];
}

/*
==============================================
crvkDevice::SurfaceCapabilities
==============================================
*/
VkSurfaceCapabilitiesKHR crvkDevice::SurfaceCapabilities(void) const
{
#if VK_VERSION_1_2
    return m_surfaceCapabilities.surfaceCapabilities;
#else
    return m_surfaceCapabilities;
#endif
}

uint32_t crvkDevice::FindBestImageCount(const uint32_t in_frames)
{
#if VK_VERSION_1_2
    if ( m_surfaceCapabilities.surfaceCapabilities.maxImageCount == 0 )
        return std::max( in_frames, m_surfaceCapabilities.surfaceCapabilities.minImageCount );

    return std::clamp( in_frames, m_surfaceCapabilities.surfaceCapabilities.minImageCount,  m_surfaceCapabilities.surfaceCapabilities.maxImageCount );
#else
    if ( m_surfaceCapabilities.surfaceCapabilities.maxImageCount == 0 )
        return std::max( in_frames, m_surfaceCapabilities.minImageCount );

    return std::clamp( in_frames, m_surfaceCapabilities.minImageCount,  m_surfaceCapabilities.maxImageCount );
#endif
}

/*
==============================================
crvkDevice::CheckExtensionSupport
==============================================
*/
const bool crvkDevice::CheckExtensionSupport( const char *in_extension )
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

/*
==============================================
crvkDevice::InitDevice
==============================================
*/
bool crvkDevice::InitDevice( const crvkContext* in_context, const VkPhysicalDevice in_device )
{
    VkResult result = VK_SUCCESS; 
    uint32_t queueFamilyCount = 0;
    uint32_t formatCount = 0;
    uint32_t extensionCount = 0;
    uint32_t presentModeCount = 0;

    // clear from memory gargbage 
    std::memset( &m_properties, 0x00, sizeof( m_properties ) );
    std::memset( &m_features, 0x00, sizeof( m_features ) );
    std::memset( &m_featuresv12, 0x00, sizeof( m_featuresv12 ) );
    std::memset( &m_featuresv13, 0x00, sizeof( m_featuresv13 ) );
    std::memset( &m_surfaceCapabilities, 0x00, sizeof( m_surfaceCapabilities ) );
    std::memset( &m_memoryProperties, 0x00, sizeof( m_memoryProperties ) );

    m_context = const_cast<crvkContext*>( in_context );
    m_physicalDevice = in_device;
    SDL_assert( m_physicalDevice != nullptr );

#if VK_VERSION_1_2
    VkPhysicalDeviceSurfaceInfo2KHR deviceSurfaceInfo{}; 
    deviceSurfaceInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    deviceSurfaceInfo.surface = m_context->Surface();
    deviceSurfaceInfo.pNext = nullptr;

    // query device properties
    m_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    m_properties.pNext = nullptr;
    vkGetPhysicalDeviceProperties2( m_physicalDevice, &m_properties );

    // query vulkan 1.2 features
    m_featuresv12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    m_featuresv12.pNext = nullptr;

    // query vulkan 1.3 features
    m_featuresv13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    m_featuresv13.pNext = &m_featuresv12;

    // query vulkan 1.0 features
    m_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    m_features.pNext = &m_featuresv13;
    
    vkGetPhysicalDeviceFeatures2( m_physicalDevice, &m_features );

    // query device surface  capabilities
    m_surfaceCapabilities.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
    m_surfaceCapabilities.pNext = nullptr;
    result = vkGetPhysicalDeviceSurfaceCapabilities2KHR( m_physicalDevice, &deviceSurfaceInfo, &m_surfaceCapabilities );
    if( result != VK_SUCCESS )
    {
        return false;
    }

    // query device suported surface formats
    result = vkGetPhysicalDeviceSurfaceFormats2KHR( m_physicalDevice, &deviceSurfaceInfo, &formatCount, nullptr );
    m_surfaceFormats.Resize( formatCount );
    m_surfaceFormats.Memset( 0x00 );

    // you bealive that vulkan require this ?
    for ( uint32_t i = 0; i < m_surfaceFormats.Count(); i++)
    {
        m_surfaceFormats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        m_surfaceFormats[i].pNext = nullptr;
    }
    
    result = vkGetPhysicalDeviceSurfaceFormats2KHR( m_physicalDevice, &deviceSurfaceInfo, &formatCount, &m_surfaceFormats );
    if( result != VK_SUCCESS )
    {
        return false;
    }

    //
    m_memoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    m_memoryProperties.pNext = nullptr;

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
    result = vkEnumerateDeviceExtensionProperties( m_physicalDevice, nullptr, &extensionCount, nullptr );
    if( result != VK_SUCCESS )
    {
        return false;
    }

    m_availableExtensions.Resize( extensionCount );
    m_availableExtensions.Memset( 0x00 );
    result = vkEnumerateDeviceExtensionProperties( m_physicalDevice, nullptr, &extensionCount, &m_availableExtensions );
    if( result != VK_SUCCESS )
    {
        return false;
    }

    // query device suported surface presenting mode
    result = vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, m_context->Surface(), &presentModeCount, nullptr );
    if( result != VK_SUCCESS )
    {
        return false;
    }

    m_presentModes.Resize( presentModeCount );
    m_presentModes.Memset( 0x00 );
    result = vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, m_context->Surface(), &presentModeCount, &m_presentModes );
    if( result != VK_SUCCESS )
    {
        return false;
    }
    
    // query device queues
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &queueFamilyCount, nullptr );
    m_queueFamilies.Resize( queueFamilyCount );
    m_queueFamilies.Memset( 0x00 );
    vkGetPhysicalDeviceQueueFamilyProperties( m_physicalDevice, &queueFamilyCount, &m_queueFamilies );

#if VK_KHR_copy_commands2
    m_deviceSuportedFeatures.copyCommands2Enabled = CheckExtensionSupport( VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME );
#endif 

    return true;
}

/*
==============================================
crvkDevice::Clear
==============================================
*/
void crvkDevice::Clear( void )
{
    for ( uint32_t i = 0; i < 4; i++)
    {
        delete m_queues[i];
    }

    if( m_logicalDevice != nullptr )
    {
        vkDestroyDevice( m_logicalDevice, k_allocationCallbacks );
        m_logicalDevice = nullptr;
    }

    m_surfaceFormats.Clear();
    m_availableExtensions.Clear();
    m_presentModes.Clear();
    m_queueFamilies.Clear();
    m_physicalDevice = nullptr;
    m_context = nullptr;
}


/*
==============================================
crvkDevice::FindQueues
==============================================
*/
void crvkDevice::FindQueues( crvkDynamicVector<VkDeviceQueueCreateInfo> &queueCreateInfos )
{
    static const float k_QUEUE_PRIORITIES[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint32_t i = 0, j = 0;
    VkBool32 canPresent;
    crvkPointer<uint32_t> queueCount;
    queueCount.Alloc( m_queueFamilies.Count() );
    queueCount.Memset( 0x00 );

    // try find a queue 
    for ( i = 0; i < 4; i++)
    {
        // first pass try find a independent family
        for ( j = 0; j < m_queueFamilies.Count(); j++ )
        {
            auto q = m_queueFamilies[j];
            switch ( i )
            {
            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS:
            {
                // already in use, try the next one
                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER]->Family() == j )
                    continue;
                
                // if we don't have availabe queues 
                if ( !( q.queueFlags & VK_QUEUE_GRAPHICS_BIT ) || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );
            } break;
            
            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT:
            {                
                vkGetPhysicalDeviceSurfaceSupportKHR( m_physicalDevice, j, m_context->Surface(), &canPresent );

                // already in use, try the next one
                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER]->Family() == j )
                    continue;

                if( !canPresent || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );                
            } break;

            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE:
            {

                // already in use, try the next one
                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER]->Family() == j )
                    continue;
            
                // if we don't have availabe queues 
                if ( !( q.queueFlags & VK_QUEUE_COMPUTE_BIT ) || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );
            } break;

            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER:
            {
                // already in use, try the next one
                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE]->Family() == j )
                    continue;

                if ( ( m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT] != nullptr ) && m_queues[crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT]->Family() == j )
                    continue;
            
                // if we don't have availabe queues 
                if ( !( q.queueFlags & VK_QUEUE_TRANSFER_BIT ) || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );
            } break;
            }      
        }

        // find a shared one 
        for ( j = 0; j < m_queueFamilies.Count(); j++ )
        {
            auto q = m_queueFamilies[j];
            switch ( i )
            {
            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS:
            {
                // already found 
                if ( m_queues[i] != nullptr )
                    continue;

                 // if we don't have availabe queues 
                if ( !( q.queueFlags & VK_QUEUE_GRAPHICS_BIT ) || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );
            } break;
            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_PRESENT:
            {
                // already found 
                if ( m_queues[i] != nullptr )
                    continue;

                vkGetPhysicalDeviceSurfaceSupportKHR( m_physicalDevice, j, m_context->Surface(), &canPresent );
                if ( !canPresent || !( queueCount[j] < q.queueCount ) )    
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );
            } break;
            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_COMPUTE:
            {
                // already found 
                if ( m_queues[i] != nullptr )
                    continue;
                    
                // if we don't have availabe queues 
                if ( !( q.queueFlags & VK_QUEUE_COMPUTE_BIT ) || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );

            } break;
            case crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER:
            {
                // already found 
                if ( m_queues[i] != nullptr )
                    continue;
                    
                 // if we don't have availabe queues 
                if ( !( q.queueFlags & VK_QUEUE_TRANSFER_BIT ) || !( queueCount[j] < q.queueCount ) )
                    continue;

                m_queues[i] = new crvkDeviceQueue( j, queueCount[j]++, (crvkDeviceQueue::crvkQueueType)i );
            } break;
            
            default:
                break;
            }
        }
    }

    // setup queue initialization 
    for ( i = 0; i < m_queueFamilies.Count(); i++)
    {
        // no selected queue 
        if ( queueCount[i] == 0 )
            continue;

        VkDeviceQueueCreateInfo queueCI{};
        queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCI.pQueuePriorities = k_QUEUE_PRIORITIES;
        queueCI.queueFamilyIndex = i;
        queueCI.queueCount = queueCount[i];
        queueCI.flags = 0;

        queueCreateInfos.Append( queueCI );
    }
}
