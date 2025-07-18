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
#include "crvkContext.hpp"

#include <iostream>
#include <cstring>
#include <SDL3/SDL_assert.h>

// our onw allocation structure using SDL_malloc
const VkAllocationCallbacks crvkContext::k_allocationCallbacks = 
{
    nullptr,
    crvkContext::Allocation,
    crvkContext::Reallocation,
    crvkContext::Free,
    crvkContext::InternalAllocation,
    crvkContext::InternalFree
};

/*
==============================================
crvkContext::crvkContext
==============================================
*/
crvkContext::crvkContext( void ) :
    m_enableValidationLayers( false ),
    m_instance( nullptr ),
    m_debugMessenger( nullptr ),
    m_surface( nullptr ),
    m_device( nullptr ),
    m_graphicsQueue( nullptr ),
    m_presentQueue( nullptr ),
    m_commandPool( nullptr )
{
}

/*
==============================================
crvkContext::~crvkContext
==============================================
*/
crvkContext::~crvkContext( void )
{
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
    crvkPointer<const char*> enabledExtensions = crvkPointer<const char*>();
    const char* const* SDL3Extensions = nullptr;

    // enable layers
    if ( in_layers != 0 )
        m_enableValidationLayers = true;
    else
        m_enableValidationLayers = false;
    
    vkLoadVulkanInstanceProcs();

    if ( m_enableValidationLayers && !CheckValidationLayerSupport( in_layers, in_layersCount ) ) 
    {
        AppendError( "crvkContext::Create: validation layers requested, but not available!" );
        return false; 
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = in_applicationName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = in_engineName;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCI{};
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pApplicationInfo = &appInfo;

    SDL3Extensions = SDL_Vulkan_GetInstanceExtensions( &SDL3ExtensionCount );

    if ( m_enableValidationLayers )
    {
        // enable debug utils extension 
        enabledExtensions.Alloc( SDL3ExtensionCount + 1 );
        enabledExtensions.Memcpy( const_cast<const char**>( SDL3Extensions ), 0, SDL3ExtensionCount );
        enabledExtensions[SDL3ExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }
    else
    {
        enabledExtensions.Alloc( SDL3ExtensionCount );
        enabledExtensions.Memcpy( const_cast<const char**>( SDL3Extensions ), 0, SDL3ExtensionCount );
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

    result = vkCreateInstance( &instanceCI, AllocationCallbacks(), &m_instance );
    if ( result != VK_SUCCESS ) 
    {
        AppendError( crvkGetVulkanError( result ) );
        AppendError("failed to create instance!");
        return false;
    }

    if ( m_enableValidationLayers )
    {
        vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr( m_instance, "vkCreateDebugUtilsMessengerEXT" );
        vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr( m_instance, "vkDestroyDebugUtilsMessengerEXT" );

        result = vkCreateDebugUtilsMessenger( m_instance, &debugCI, AllocationCallbacks(), &m_debugMessenger ); 
        if ( result != VK_SUCCESS ) 
        {
            AppendError( SDL_GetError() );
            AppendError( "failed to set up debug messenger!" );
            return false;
        }
 
    }
    
    if ( !SDL_Vulkan_CreateSurface( const_cast<SDL_Window*>( in_whdn ), m_instance, AllocationCallbacks(), &m_surface ) )
    {            
        AppendError( SDL_GetError() );
        AppendError( "failed to create window surface!" );
        return false;
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) 
    {
        AppendError( "failed to find GPUs with Vulkan support!" );
        return false;
    }

    m_physicalDeviceList.Alloc( deviceCount );
    vkEnumeratePhysicalDevices( m_instance, &deviceCount, &m_physicalDeviceList );

    m_devicePropertiesList.Alloc( deviceCount );
    for ( uint32_t i = 0; i < deviceCount; i++)
    {
        // aquire device properties 
        m_devicePropertiesList[i].InitDevice( m_physicalDeviceList[i] );
    }
    
    return true;
}

bool crvkContext::InitializeDevice(const crvkDeviceProperties *in_device, const char *in_layers, const uint32_t in_layersCount, const char *in_deviceExtensions, const uint32_t in_deviceExtensionsCount)
{
    return false;
}

void crvkContext::Destroy(void)
{
    if ( m_commandPool != nullptr )
    {
        vkDestroyCommandPool( m_device, m_commandPool, AllocationCallbacks() );
        m_commandPool = nullptr;
    }
    
    if ( m_device != nullptr )
    {
        vkDestroyDevice( m_device, AllocationCallbacks() );
        m_device = nullptr;
    }
    
    if( m_surface != nullptr )
    {
        vkDestroySurfaceKHR( m_instance, m_surface, AllocationCallbacks() );
        m_surface = nullptr;
    }

    if ( m_debugMessenger != nullptr )
    {
        vkDestroyDebugUtilsMessenger( m_instance, m_debugMessenger, AllocationCallbacks() );
        m_debugMessenger = nullptr;
    }
    
    if( m_instance != nullptr )
    {
        vkDestroyInstance( m_instance, AllocationCallbacks() );
        m_instance = nullptr;
    }
}

void crvkContext::AppendError(const char *in_error)
{
    //TODO: append a error to a queue 
    printf( in_error );
}

bool crvkContext::CheckValidationLayerSupport( const char ** in_layers, const uint32_t in_layersCount )
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
    crvkPointer<VkLayerProperties> availableLayers;
    vkEnumerateInstanceLayerProperties( &layerCount, &availableLayers );

    for ( uint32_t i = 0; i < in_layersCount; i++)
    {
        bool layerFound = false;

        for (size_t j = 0; j < layerCount; j++)
        {
            if ( std::strcmp( in_layers[i], availableLayers[j].layerName ) == 0) 
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
crvkContext::Allocation
==============================================
*/
void *VKAPI_ATTR crvkContext::Allocation( void * in_userData, size_t in_size, size_t in_alignment, VkSystemAllocationScope in_allocationScope )
{
    void* original = SDL_malloc( in_size + in_alignment - 1 + sizeof(void*) );
    uintptr_t aligned = ( reinterpret_cast<uintptr_t>( original ) + sizeof(void*) + in_alignment - 1) & ~( in_alignment - 1 );
    (reinterpret_cast<void**>(aligned))[-1] = original;
    void* memptr = reinterpret_cast<void*>( aligned );

    // check if memory is aligned 
    SDL_assert( memptr && ( (uintptr_t)memptr % in_alignment ) == 0 );
    return memptr;
}

/*
==============================================
crvkContext::Reallocation
==============================================
*/
void* VKAPI_CALL crvkContext::Reallocation( void* in_userData, void* in_original, size_t in_size, size_t in_alignment, VkSystemAllocationScope in_allocationScope )
{
    void* original = static_cast<void**>( in_original )[-1];
    if( original != nullptr )
        original = SDL_realloc( original, in_size + in_alignment - 1 + sizeof(void*) );
    else
        original = SDL_malloc( in_size + in_alignment - 1 + sizeof(void*) );

    uintptr_t aligned = ( reinterpret_cast<uintptr_t>( original ) + sizeof(void*) + in_alignment - 1) & ~( in_alignment - 1 );
    (reinterpret_cast<void**>(aligned))[-1] = original;
    void* memptr = reinterpret_cast<void*>( aligned );

    // check if memory is aligned 
    SDL_assert( memptr && ( (uintptr_t)memptr % in_alignment ) == 0 );
    return memptr;
}

/*
==============================================
crvkContext::Free
==============================================
*/
void VKAPI_CALL crvkContext::Free( void* in_userData, void* in_memory )
{
    if ( in_memory ) 
    {
        void* original = static_cast<void**>( in_memory )[-1];
        SDL_free( original );
    }
}

/*
==============================================
crvkContext::InternalAllocation
==============================================
*/
void VKAPI_CALL crvkContext::InternalAllocation( void* in_userData, size_t in_size, VkInternalAllocationType in_allocationType, VkSystemAllocationScope in_allocationScope )
{
    //vkCtx.allocedMemory += size;
    printf("[Vulkan] Internal allocation of %zu bytes, total %i\n", in_size, 0 );
}

/*
==============================================
crvkContext::InternalFree
==============================================
*/
void VKAPI_CALL crvkContext::InternalFree( void* in_userData, size_t in_size, VkInternalAllocationType in_allocationType, VkSystemAllocationScope in_allocationScope )
{
    // vkCtx.allocedMemory -= size; 
    printf("[Vulkan] Internal free of %zu bytes, total %i\n", in_size, 0 );
}

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