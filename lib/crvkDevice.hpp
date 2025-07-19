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

#ifndef __CRVK_DEVICE_HPP__
#define __CRVK_DEVICE_HPP__

typedef struct
{
    bool copyCommands2Enabled = false;
} crvkDeviceSuportedFeatures_t;

class crvkDevice
{
public:
    crvkDevice( void );
    ~crvkDevice( void );

    void    Create( const char* in_layers, const uint32_t in_layersCount, const char* in_deviceExtensions, const uint32_t in_deviceExtensionsCount );
    void    Destroy( void );

    VkPhysicalDevice        PhysicalDevice( void ) const { return m_physicalDevice; }    
    VkDevice                Device( void ) const { return m_logicalDevice; }
    VkCommandPool           CommandPool( void ) const { return m_commandPool; }
    VkQueue                 GraphicsQueue( void ) const { return m_graphicsQueue; }
    VkQueue                 PresentQueue( void ) const { return m_presentQueue; }
    VkExtent2D              FindExtent( const uint32_t in_width, const uint32_t in_height ) const;
    VkPresentModeKHR        FindPresentMode( const VkPresentModeKHR in_presentMode ) const;
    VkSurfaceFormatKHR      FindSurfaceFormat( const VkFormat in_format, const VkColorSpaceKHR in_colorSpace ) const;
    uint32_t                FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const;
    const bool              CheckExtensionSupport( const char* in_extension );

protected:
    friend class crvkContext;
    bool    InitDevice( const VkPhysicalDevice in_device, const VkSurfaceKHR in_surface );

private:
    crvkDeviceSuportedFeatures_t            m_deviceSuportedFeatures;
#ifdef VK_VERSION_1_2
    VkPhysicalDeviceProperties2             m_properties;
    VkSurfaceCapabilities2KHR               m_surfaceCapabilities;
    VkPhysicalDeviceMemoryProperties2       m_memoryProperties;
    crvkPointer<VkSurfaceFormat2KHR>        m_surfaceFormats;
#else
    VkPhysicalDeviceProperties              m_properties;
    VkSurfaceCapabilitiesKHR                m_surfaceCapabilities;
    VkPhysicalDeviceMemoryProperties        m_memoryProperties;
    crvkPointer<VkSurfaceFormatKHR>         m_surfaceFormats;
#endif // VK_VERSION_1_2
    VkQueue                                 m_graphicsQueue;
    VkQueue                                 m_presentQueue;
    VkCommandPool                           m_commandPool;
    VkPhysicalDevice                        m_physicalDevice;
    VkDevice                                m_logicalDevice;
    crvkPointer<VkExtensionProperties>      m_availableExtensions;
    crvkPointer<VkPresentModeKHR>           m_presentModes;
    crvkPointer<VkQueueFamilyProperties>    m_queueFamilies;

    // delete refernce 
    crvkDevice( const crvkDevice & ) = delete;
    crvkDevice operator=( const crvkDevice &) = delete;
};

#endif //!__CRVK_DEVICE_HPP__