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

typedef struct crvkDeviceHandle_t crvkDeviceHandle_t;
typedef struct glslang_resource_s glslang_resource_t;
class crvkDevice
{
public:
    crvkDevice( void );
    ~crvkDevice( void );

    bool    Create( const char** in_layers, const uint32_t in_layersCount, const char** in_deviceExtensions, const uint32_t in_deviceExtensionsCount );
    void    Destroy( void );

    VkPhysicalDevice            PhysicalDevice( void ) const;    
    VkDevice                    Device( void ) const;
    crvkQueueInfo_t*            GetQueueInfo( uint32_t* in_count ) const;
    VkSurfaceCapabilitiesKHR    SurfaceCapabilities( void ) const;
    uint32_t                    FindBestImageCount( const uint32_t in_frames );

    /// @brief Get the list of available present modes by the surface and device
    /// @param in_count if not NULL, return the count of available elements
    /// @return the array os present modes suported 
    const VkPresentModeKHR*     GetSuportedPresentModes( uint32_t *in_count ) const;
    
    /// @brief Get the list of device available surface formats
    /// @param in_count if not NULL, return the count of available elements 
    /// @return an array of suported surface formats 
    const VkSurfaceFormat2KHR*  GetSuportedSurfaceFormat( uint32_t *in_count ) const;

    VkExtent2D                  FindExtent( const uint32_t in_width, const uint32_t in_height ) const;
    uint32_t                    FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties ) const;
    const bool                  CheckExtensionSupport( const char* in_extension );
    const glslang_resource_t*   BuiltInShaderResource( void ) const;     

protected:
    friend class crvkContext;
    friend class crvkBufferStaging;
    bool    InitDevice( const crvkContext* in_context, const VkPhysicalDevice in_device );
 
private:
    crvkDeviceHandle_t*                             m_handle;    
    crvkDeviceSuportedFeatures_t                    m_deviceSuportedFeatures;

    void        InitializeBuiltInShaderResources( void );
    void        AquireDeviceProperties( void );
    bool        AquireDeviceQueues( const VkPhysicalDeviceSurfaceInfo2KHR in_deviceSurfaceInfo );
    bool        AquireDeviceSurfaceProperties( const VkPhysicalDeviceSurfaceInfo2KHR in_deviceSurfaceInfo );
    bool        AquireDeviceFeatures( const VkPhysicalDeviceSurfaceInfo2KHR in_deviceSurfaceInfo );

    // delete referencing 
    crvkDevice( const crvkDevice & ) = delete;
    crvkDevice operator=( const crvkDevice &) = delete;
};

#endif //!__CRVK_DEVICE_HPP__