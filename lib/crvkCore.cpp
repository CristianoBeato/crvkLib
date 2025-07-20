// ===============================================================================================
// crvkCore - Vulkan + SDL minimal framework
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
#include "crvkCore.hpp"


 /// 
static VKAPI_ATTR void* VKAPI_CALL crvkAllocation( void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope );
static VKAPI_ATTR void* VKAPI_CALL crvkReallocation( void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static VKAPI_ATTR void  VKAPI_CALL crvkFree( void* pUserData, void* pMemory );
static VKAPI_ATTR void  VKAPI_CALL crvkInternalAllocation( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
static VKAPI_ATTR void  VKAPI_CALL crvkInternalFree( void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope );
   
// our onw allocation structure using SDL_malloc
const VkAllocationCallbacks k_allocationCallbacks = 
{
    nullptr,
    crvkAllocation,
    crvkReallocation,
    crvkFree,
    crvkInternalAllocation,
    crvkInternalFree
};

// VK_EXT_debug_utils
#if VK_EXT_debug_utils
PFN_vkCreateDebugUtilsMessengerEXT          vkCreateDebugUtilsMessenger = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT         vkDestroyDebugUtilsMessenger = nullptr;
#endif // VK_EXT_debug_utils

#ifdef VK_NO_PROTOTYPES

PFN_vkGetInstanceProcAddr                   vkGetInstanceProcAddr = nullptr;
PFN_vkEnumerateInstanceLayerProperties      vkEnumerateInstanceLayerProperties = nullptr;
PFN_vkCreateInstance                        vkCreateInstance = nullptr;
PFN_vkDestroyInstance                       vkDestroyInstance = nullptr;
PFN_vkEnumeratePhysicalDevices              vkEnumeratePhysicalDevices = nullptr;

PFN_vkDestroySurfaceKHR                     vkDestroySurfaceKHR = nullptr;

//VK_KHR_get_surface_capabilities2
#if VK_KHR_get_surface_capabilities2
PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR  vkGetPhysicalDeviceSurfaceCapabilities2KHR = nullptr;
PFN_vkGetPhysicalDeviceSurfaceFormats2KHR       vkGetPhysicalDeviceSurfaceFormats2KHR = nullptr;
#elif defined( VK_NO_PROTOTYPES )
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR   vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR        vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
#endif //VK_KHR_get_surface_capabilities2

// VK_KHR_copy_commands2
#if VK_KHR_copy_commands2
PFN_vkCmdBlitImage2KHR                      vkCmdBlitImage2KHR = nullptr;
PFN_vkCmdCopyBuffer2KHR                     vkCmdCopyBuffer2KHR = nullptr;
PFN_vkCmdCopyBufferToImage2KHR              vkCmdCopyBufferToImage2KHR = nullptr;
PFN_vkCmdCopyImage2KHR                      vkCmdCopyImage2KHR = nullptr;
PFN_vkCmdCopyImageToBuffer2KHR              vkCmdCopyImageToBuffer2KHR = nullptr;
#endif // VK_KHR_copy_commands2

void vkLoadVulkanInstanceProcs( void )
{
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>( SDL_Vulkan_GetVkGetInstanceProcAddr() );

    //
    vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>( vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceLayerProperties" ) );
    vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>( vkGetInstanceProcAddr( nullptr , "vkCreateInstance" ) );
}

void vkLoadVulkanProcs( VkInstance in_instance )
{
    vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>( vkGetInstanceProcAddr( in_instance ,"vkDestroyInstance" ) );
    vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>( vkGetInstanceProcAddr( in_instance, "vkEnumeratePhysicalDevices" ) );

    vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>( vkGetInstanceProcAddr( in_instance, "vkDestroySurfaceKHR" ) );
}

void vkLoadDeviceProcs( VkDevice in_device )
{
#if VK_KHR_copy_commands2
    vkCmdBlitImage2KHR = reinterpret_cast<PFN_vkCmdBlitImage2KHR>( vkGetDeviceProcAddr( in_device, "vkCmdBlitImage2KHR" ) );
    vkCmdCopyBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyBuffer2KHR>( vkGetDeviceProcAddr( in_device, "vkCmdCopyBuffer2KHR" ) );
    vkCmdCopyBufferToImage2KHR = reinterpret_cast<PFN_vkCmdCopyBufferToImage2KHR>( vkGetDeviceProcAddr( in_device, "vkCmdCopyBufferToImage2KHR" ) );
    vkCmdCopyImage2KHR = reinterpret_cast<PFN_vkCmdCopyImage2KHR>( vkGetDeviceProcAddr( in_device, "vkCmdCopyImage2KHR" ) );
    vkCmdCopyImageToBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2KHR>( vkGetDeviceProcAddr( in_device, "vkCmdCopyImageToBuffer2KHR" ) );
#endif // VK_KHR_copy_commands2 
}

#else

/*
==============================================
vkLoadVulkanInstanceProcs
==============================================
*/
void vkLoadVulkanInstanceProcs( void )
{
    // do nothing, static linked  
}

/*
==============================================
vkLoadVulkanProcs
==============================================
*/
void vkLoadVulkanProcs( VkInstance in_instance )
{
    // do nothing, static linked 
}

/*
==============================================
vkLoadDeviceProcs
==============================================
*/
void vkLoadDeviceProcs( VkDevice in_device )
{
    // do nothing, static linked 
}

#endif

/*
==============================================
vkLoadVulkanDebugUtilsProcs
==============================================
*/
void vkLoadVulkanDebugUtilsProcs( VkInstance in_instance )
{
#if VK_EXT_debug_utils
    vkCreateDebugUtilsMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>( vkGetInstanceProcAddr( in_instance, "vkCreateDebugUtilsMessengerEXT" ) );
    vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>( vkGetInstanceProcAddr( in_instance, "vkDestroyDebugUtilsMessengerEXT" ) );
#endif //VK_EXT_debug_utils
}

const char* crvkGetLastError( void )
{
    return nullptr;
}

void crvkAppendError( const char* in_error, const VkResult in_code )
{
    std::printf( "VkError: %s -> %s\n", in_error, crvkGetVulkanError( in_code ) );
}

/*
==============================================
crvkAllocation
==============================================
*/
void *VKAPI_ATTR crvkAllocation( void * in_userData, size_t in_size, size_t in_alignment, VkSystemAllocationScope in_allocationScope )
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
crvkReallocation
==============================================
*/
void* VKAPI_CALL crvkReallocation( void* in_userData, void* in_original, size_t in_size, size_t in_alignment, VkSystemAllocationScope in_allocationScope )
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
crvkFree
==============================================
*/
void VKAPI_CALL crvkFree( void* in_userData, void* in_memory )
{
    if ( in_memory ) 
    {
        void* original = static_cast<void**>( in_memory )[-1];
        SDL_free( original );
    }
}

/*
==============================================
crvkInternalAllocation
==============================================
*/
void VKAPI_CALL crvkInternalAllocation( void* in_userData, size_t in_size, VkInternalAllocationType in_allocationType, VkSystemAllocationScope in_allocationScope )
{
    //vkCtx.allocedMemory += size;
    std::printf("[Vulkan] Internal allocation of %zu bytes, total %i\n", in_size, 0 );
}

/*
==============================================
crvkInternalFree
==============================================
*/
void VKAPI_CALL crvkInternalFree( void* in_userData, size_t in_size, VkInternalAllocationType in_allocationType, VkSystemAllocationScope in_allocationScope )
{
    // vkCtx.allocedMemory -= size; 
    std::printf("[Vulkan] Internal free of %zu bytes, total %i\n", in_size, 0 );
}
