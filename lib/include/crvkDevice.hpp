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

#ifndef __CRVK_DEVICE_HPP__
#define __CRVK_DEVICE_HPP__

typedef struct crvkDeviceSuportedFeatures_t
{
    bool timelineSemaphore = false;
    bool copyCommands2Enabled = false;
} crvkDeviceSuportedFeatures_t;

typedef enum crvkQueueType
{
    CRVK_DEVICE_QUEUE_NONE = -1,
    CRVK_DEVICE_QUEUE_GRAPHICS,
    CRVK_DEVICE_QUEUE_PRESENT,
    CRVK_DEVICE_QUEUE_COMPUTE,
    CRVK_DEVICE_QUEUE_TRANSFER
} crvkQueueType;

class crvkDeviceQueue
{
public:


    VkResult Submit( 
                    const VkSemaphoreSubmitInfo* in_waitSemaphoreInfos,
                    const uint32_t in_waitSemaphoreInfoCount,
                    const VkCommandBufferSubmitInfo* in_commandBufferInfos,
                    const uint32_t in_commandBufferInfoCount,
                    const VkSemaphoreSubmitInfo* in_signalSemaphoreInfos,
                    const uint32_t in_signalSemaphoreInfoCount, 
                    const VkFence in_fence );

    VkResult Submit( 
        const VkCommandBuffer* in_commandBuffers, 
        const uint32_t in_commandBuffersCount, 
        const VkSemaphore* in_waitSemaphores,
        const uint32_t in_waitSemaphoresCount,
        const VkSemaphore* in_signalSemaphores,
        const uint32_t in_signalSemaphoresCount );

    VkResult    Present( 
        const VkSwapchainKHR* in_swapchains,
        const uint32_t* in_imageIndices,
        const uint32_t in_swapchainCount,
        const VkSemaphore* in_waitSemaphores,
        const uint32_t in_waitSemaphoresCount );

    VkResult    WaitIdle( void ) const;

    /// @brief Queue family index
    /// @return UINT32_MAX if not valid
    uint32_t    Family( void ) const { return m_family; }

    /// @brief Queue index 
    /// @return UINT32_MAX if not valid 
    uint32_t    Index( void ) const { return m_family; }

    /// @brief
    /// @return 
    VkQueue         Queue( void ) const { return m_queue; }

    /// @brief  
    /// @return 
    VkCommandPool   CommandPool( void ) const { return m_commandPool; }

protected:
    friend class crvkDevice;
    crvkDeviceQueue( const uint32_t in_family, const uint32_t in_index, const crvkQueueType in_type );
    ~crvkDeviceQueue( void );
    bool    InitQueue( const VkDevice in_device );

private:
    crvkQueueType   m_type;
    uint32_t        m_family;
    uint32_t        m_index;
    VkQueue         m_queue;
    VkCommandPool   m_commandPool;
    VkDevice        m_device;

    crvkDeviceQueue( const crvkDeviceQueue & ) = delete;
    crvkDeviceQueue operator=( const crvkDeviceQueue & ) = delete;
};

typedef struct glslang_resource_s glslang_resource_t;
class crvkDevice
{
public:
    bool    Create( const char** in_layers, const uint32_t in_layersCount, const char** in_deviceExtensions, const uint32_t in_deviceExtensionsCount );
    void    Destroy( void );

    VkPhysicalDevice            PhysicalDevice( void ) const { return m_physicalDevice; }    
    VkDevice                    Device( void ) const { return m_logicalDevice; }
    crvkDeviceQueue*            GetQueue( const crvkQueueType in_type ) const;
    bool                        HasPresentQueue( void ) const;
    bool                        HasComputeQueue( void ) const;
    bool                        HasTransferQueue( void ) const;
    VkSurfaceCapabilitiesKHR    SurfaceCapabilities( void ) const;
    uint32_t                    FindBestImageCount( const uint32_t in_frames );
    VkExtent2D                  FindExtent( const uint32_t in_width, const uint32_t in_height ) const;
    VkPresentModeKHR            FindPresentMode( const VkPresentModeKHR in_presentMode ) const;
    VkSurfaceFormatKHR          FindSurfaceFormat( const VkFormat in_format, const VkColorSpaceKHR in_colorSpace ) const;
    uint32_t                    FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const;
    const bool                  CheckExtensionSupport( const char* in_extension );
    const glslang_resource_t*   BuiltInShaderResource( void ) const;     

protected:
    friend class crvkContext;
    friend class crvkBufferStaging;
    // we can't create a new class reference outside from crvkContext
    crvkDevice( void );
    ~crvkDevice( void );
    bool    InitDevice( const crvkContext* in_context, const VkPhysicalDevice in_device );
    void    Clear( void );

    const bool TimelineSemaphoreAvailable( void ) const { return m_featuresv12.timelineSemaphore; }

private:
    crvkDeviceSuportedFeatures_t                    m_deviceSuportedFeatures;
    VkPhysicalDeviceProperties2                     m_propertiesv10;
    VkPhysicalDeviceVulkan11Properties              m_propertiesv11;
    VkPhysicalDeviceVulkan12Properties              m_propertiesv12;
    VkPhysicalDeviceVulkan13Properties              m_propertiesv13;
    VkPhysicalDeviceTransformFeedbackPropertiesEXT  m_propertiesTransformFeedback;
    VkPhysicalDeviceFeatures2                       m_featuresv10;
    VkPhysicalDeviceVulkan12Features                m_featuresv11;
    VkPhysicalDeviceVulkan12Features                m_featuresv12;
    VkPhysicalDeviceVulkan13Features                m_featuresv13;
    VkPhysicalDeviceTransformFeedbackFeaturesEXT    m_featuresTransformFeedback;
    VkSurfaceCapabilities2KHR                       m_surfaceCapabilities;
    VkPhysicalDeviceMemoryProperties2               m_memoryProperties;
    crvkDynamicVector<VkSurfaceFormat2KHR>          m_surfaceFormats;
    VkPhysicalDevice                                m_physicalDevice;
    VkDevice                                        m_logicalDevice;
    crvkPointer<glslang_resource_t>                 m_shaderBuiltInResource;
    crvkContext*                                    m_context;
    crvkDynamicVector<VkExtensionProperties>        m_availableExtensions;
    crvkDynamicVector<VkPresentModeKHR>             m_presentModes;
    crvkDynamicVector<VkQueueFamilyProperties>      m_queueFamilies;
    crvkDeviceQueue*                                m_queues[4];

    void    FindQueues( crvkDynamicVector<VkDeviceQueueCreateInfo> &queueCreateInfos );

    void    InitializeBuiltInShaderResources( void );

    // delete refernce 
    crvkDevice( const crvkDevice & ) = delete;
    crvkDevice operator=( const crvkDevice &) = delete;
};

#endif //!__CRVK_DEVICE_HPP__