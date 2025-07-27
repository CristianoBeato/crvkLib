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

// initialize shader compiler interface
#include <glslang/Include/glslang_c_interface.h>
#include "crvkPrecompiled.hpp"
#include "crvkContext.hpp"

/*
==============================================
crvkContext::crvkContext
==============================================
*/
crvkContext::crvkContext( void ) :
    m_enableValidationLayers( false ),
    m_instance( nullptr ),
    m_debugMessenger( nullptr ),
    m_surface( nullptr )
{
}

/*
==============================================
crvkContext::~crvkContext
==============================================
*/
crvkContext::~crvkContext( void )
{
    Destroy();
}

/*
==============================================
crvkContext::Create
==============================================
*/
bool crvkContext::Create( 
        const SDL_Window *in_whdn, 
        const char* in_applicationName, 
        const char* in_engineName, 
        const char**in_layers, 
        const uint32_t in_layersCount )
{
    VkResult result = VK_SUCCESS;
    uint32_t SDL3ExtensionCount = 0;
    crvkDynamicVector<const char*> enabledExtensions;
    const char* const* SDL3Extensions = nullptr;

    // enable layers
    if ( in_layers != 0 )
        m_enableValidationLayers = true;
    else
        m_enableValidationLayers = false;
    
    vkLoadVulkanInstanceProcs();

    if ( m_enableValidationLayers && !CheckValidationLayerSupport( in_layers, in_layersCount ) ) 
    {
        crvkAppendError( "crvkContext::Create: validation layers requested, but not available!", VK_INCOMPLETE  );
        return false; 
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = in_applicationName;
    appInfo.applicationVersion = VK_MAKE_VERSION( 0, 0, 1 );
    appInfo.pEngineName = in_engineName;
    appInfo.engineVersion = VK_MAKE_VERSION( 0, 0, 1 );
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCI{};
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pApplicationInfo = &appInfo;
    
    // copy and enable system extensios
    SDL3Extensions = SDL_Vulkan_GetInstanceExtensions( &SDL3ExtensionCount );
    enabledExtensions.Resize( SDL3ExtensionCount );
    enabledExtensions.Memcpy( const_cast<const char**>( SDL3Extensions ), 0, SDL3ExtensionCount );

#if VK_VERSION_1_2
    enabledExtensions.Append( VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME ); //
#endif 

    if ( m_enableValidationLayers ) // enable debug utils extension 
        enabledExtensions.Append( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );

    for ( uint32_t i = 0; i < enabledExtensions.Count(); i++)
    {
        printf( enabledExtensions[i] );
    }
    

    instanceCI.enabledExtensionCount = enabledExtensions.Count();
    instanceCI.ppEnabledExtensionNames = &enabledExtensions;

    VkDebugUtilsMessengerCreateInfoEXT debugCI{};
    debugCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCI.pfnUserCallback = DebugCallback;

    if (m_enableValidationLayers) 
    {
        instanceCI.enabledLayerCount = in_layersCount;
        instanceCI.ppEnabledLayerNames = const_cast<const char* const*>( in_layers );
        instanceCI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCI;
    } 
    else 
    {
        instanceCI.enabledLayerCount = 0;
        instanceCI.pNext = nullptr;
    }

    result = vkCreateInstance( &instanceCI, k_allocationCallbacks, &m_instance );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError("failed to create instance!", result );
        return false;
    }

    if ( m_enableValidationLayers )
    {
        vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr( m_instance, "vkCreateDebugUtilsMessengerEXT" );
        vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr( m_instance, "vkDestroyDebugUtilsMessengerEXT" );

        result = vkCreateDebugUtilsMessenger( m_instance, &debugCI, k_allocationCallbacks, &m_debugMessenger ); 
        if ( result != VK_SUCCESS ) 
        {
            crvkAppendError( "failed to set up debug messenger!", result );
            return false;
        }
 
    }
    
    if ( !SDL_Vulkan_CreateSurface( const_cast<SDL_Window*>( in_whdn ), m_instance, k_allocationCallbacks, &m_surface ) )
    {            
        crvkAppendError( SDL_GetError(), VK_INCOMPLETE );
        return false;
    }

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices( m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) 
    {
        crvkAppendError( "failed to find GPUs with Vulkan support!", result );
        return false;
    }

    m_physicalDeviceList.Resize( deviceCount );
    m_physicalDeviceList.Memset( 0x00000000 );
    vkEnumeratePhysicalDevices( m_instance, &deviceCount, &m_physicalDeviceList );

    m_devicePropertiesList.Resize( deviceCount );
    m_devicePropertiesList.Memset( 0x00000000 );
    for ( uint32_t i = 0; i < deviceCount; i++)
    {
        // aquire device properties
        m_devicePropertiesList[i] = new crvkDevice();
        m_devicePropertiesList[i]->InitDevice( this, m_physicalDeviceList[i] );
    }
    
    // initialize shader compiler
    glslang_initialize_process();

    return true;
}

/*
==============================================
crvkContext::Destroy
==============================================
*/
void crvkContext::Destroy(void)
{
    // release shader compiler
    glslang_finalize_process();

    for ( uint32_t i = 0; i < m_devicePropertiesList.Count(); i++)
    {
        delete m_devicePropertiesList[i];
    }
    
    m_physicalDeviceList.Clear();
    m_devicePropertiesList.Clear();

    if( m_surface != nullptr )
    {
        vkDestroySurfaceKHR( m_instance, m_surface, k_allocationCallbacks );
        m_surface = nullptr;
    }

    if ( m_debugMessenger != nullptr )
    {
        vkDestroyDebugUtilsMessenger( m_instance, m_debugMessenger, k_allocationCallbacks );
        m_debugMessenger = nullptr;
    }
    
    if( m_instance != nullptr )
    {
        vkDestroyInstance( m_instance, k_allocationCallbacks );
        m_instance = nullptr;
    }
}

/*
==============================================
crvkContext::GetDeviceList
==============================================
*/
crvkDevice* const * crvkContext::GetDeviceList( uint32_t* in_count ) const
{
    if ( in_count != nullptr )
        *in_count = m_devicePropertiesList.Count();
    
    return &m_devicePropertiesList;
}

/*
==============================================
crvkContext::CheckValidationLayerSupport
==============================================
*/
bool crvkContext::CheckValidationLayerSupport( const char ** in_layers, const uint32_t in_layersCount )
{
    uint32_t layerCount = 0;
    crvkDynamicVector<VkLayerProperties> availableLayers = crvkDynamicVector<VkLayerProperties>();
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
    availableLayers.Resize( layerCount );
    availableLayers.Memset( 0x00 );
    vkEnumerateInstanceLayerProperties( &layerCount, &availableLayers );

    for ( uint32_t i = 0; i < in_layersCount; i++)
    {
        bool layerFound = false;

        for (size_t j = 0; j < layerCount; j++)
        {
            auto layer = availableLayers[j];
            if ( std::strcmp( in_layers[i], layer.layerName ) == 0) 
            {
                layerFound = true;
                break;
            }
        }

        if ( !layerFound )
            return false;
    }

    return true;
}

/*
==============================================
crvkContext::DebugCallback
==============================================
*/
VKAPI_ATTR VkBool32 VKAPI_CALL crvkContext::DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT in_messageSeverity, VkDebugUtilsMessageTypeFlagsEXT in_messageType, const VkDebugUtilsMessengerCallbackDataEXT* in_callbackData, void* pUserData )
{
    //    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    //        return VK_FALSE;

    const char* color = "";
    const char* severityStr = "";
    switch ( in_messageSeverity) 
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: 
        color = "\033[90m"; // GRAY 
        severityStr = "VERBOSE"; 
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    
        color = "\033[36m"; 
        severityStr = "INFO";    
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: 
        color = "\033[33m"; 
        severityStr = "WARNING"; 
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   
        color = "\033[31m"; // RED
        severityStr = "ERROR";   
        break;
    default:
        break;
    }

    const char* typeStr = "";
    switch ( in_messageType ) 
    {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:     
            typeStr = "GENERAL"; 
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:  
            typeStr = "VALIDATION"; 
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: 
            typeStr = "PERFORMANCE"; 
            break;
    }

    std::cerr << color << "[VULKAN][" << severityStr << "][" << typeStr << "] " << in_callbackData->pMessage << "\033[0m" << std::endl;

    if ( in_callbackData->objectCount > 0) 
    {
        std::cerr << "  Objects involved:" << std::endl;
        for (uint32_t i = 0; i < in_callbackData->objectCount; ++i) 
        {
            std::cerr << "    - [" << in_callbackData->pObjects[i].objectType << "] "
                      << ( in_callbackData->pObjects[i].pObjectName ? in_callbackData->pObjects[i].pObjectName : "Unnamed")
                      << std::endl;
        }
    }

    return VK_FALSE;
}