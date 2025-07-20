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

#include <SDL3/SDL_error.h>

class crvkDevice;
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
    
    void    Destroy( void );

    crvkDevice* const *         GetDeviceList( uint32_t* in_count ) const;
    VkInstance                  Instance( void ) const { return m_instance; }
    VkSurfaceKHR                Surface( void ) const { return m_surface; }

private:
    bool                                m_enableValidationLayers;
    VkInstance                          m_instance;
    VkDebugUtilsMessengerEXT            m_debugMessenger;
    VkSurfaceKHR                        m_surface;
    crvkPointer<VkPhysicalDevice>       m_physicalDeviceList;
    crvkPointer<crvkDevice*>            m_devicePropertiesList;

    bool CheckValidationLayerSupport( const char** in_layers, const uint32_t in_layersCount );

    static VKAPI_ATTR VkBool32 VKAPI_CALL   DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData );
};

#endif //!__CRVK_CONTEXT_HPP__