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

#ifndef __CRVK_CONTEXT_HPP__
#define __CRVK_CONTEXT_HPP__

class crvkDeviceProperties;
class crvkContext
{
public:
    crvkContext( void );
    ~crvkContext( void );

    bool    Create( 
        const SDL_Window *in_whdn, 
        const char* in_applicationName, 
        const char* in_engineName, 
        const char** in_layers, 
        const uint32_t in_layersCount 
    );
    
    bool    InitializeDevice( 
        const crvkDeviceProperties* in_device,
        const char* in_layers, 
        const uint32_t in_layersCount, 
        const char* in_deviceExtensions, 
        const uint32_t in_deviceExtensionsCount
    );

    void    Destroy( void );

    VkInstance                  Instance( void ) const { return m_instance; }
    VkSurfaceKHR                Surface( void ) const { return m_surface; }
    VkDevice                    Device( void ) const { return m_device; }
    VkQueue                     GraphicsQueue( void ) const { return m_graphicsQueue; }
    VkQueue                     PresentQueue( void ) const { return m_presentQueue; }
    VkCommandPool               CommandPool( void ) const { return m_commandPool; }
    VkAllocationCallbacks*      AllocationCallbacks( void ) const { return const_cast<VkAllocationCallbacks*>( &k_allocationCallbacks ); /*nullptr;*/ }

    const char*                 GetLastError( void ) ;

protected:
    void    AppendError( const char* in_error );

private:
    bool                                m_enableValidationLayers;
    VkInstance                          m_instance;
    VkDebugUtilsMessengerEXT            m_debugMessenger;
    VkSurfaceKHR                        m_surface;
    VkDevice                            m_device;
    VkQueue                             m_graphicsQueue;
    VkQueue                             m_presentQueue;
    VkCommandPool                       m_commandPool;
    crvkPointer<VkPhysicalDevice>       m_physicalDeviceList;
    crvkPointer<crvkDeviceProperties>   m_devicePropertiesList;
    static const VkAllocationCallbacks  k_allocationCallbacks;

    bool CheckValidationLayerSupport( const char** in_layers, const uint32_t in_layersCount );

    /// 
    static VKAPI_ATTR void* VKAPI_CALL      Allocation( void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope );
    static VKAPI_ATTR void* VKAPI_CALL      Reallocation( void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
    static VKAPI_ATTR void  VKAPI_CALL      Free( void* pUserData, void* pMemory );
    static VKAPI_ATTR void  VKAPI_CALL      InternalAllocation( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
    static VKAPI_ATTR void  VKAPI_CALL      InternalFree( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope );
    static VKAPI_ATTR VkBool32 VKAPI_CALL   DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );
};

#endif //!__CRVK_CONTEXT_HPP__