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

#ifndef __CRVK_IMPLEMENTATION_HPP__
#define __CRVK_IMPLEMENTATION_HPP__

// c integer types
#include <cstdint>

//#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <SDL3/SDL_vulkan.h>

#define NO_SDL_VULKAN_TYPEDEFS
#include <SDL3/SDL_vulkan.h>

#define CR_VERBOSE 1

extern const VkAllocationCallbacks* k_allocationCallbacks;

// VK_EXT_debug_utils
#if VK_EXT_debug_utils
extern PFN_vkCreateDebugUtilsMessengerEXT       vkCreateDebugUtilsMessenger;
extern PFN_vkDestroyDebugUtilsMessengerEXT      vkDestroyDebugUtilsMessenger;
#endif // VK_EXT_debug_utils

#ifdef VK_NO_PROTOTYPES

extern PFN_vkGetInstanceProcAddr                vkGetInstanceProcAddr;
extern PFN_vkEnumerateInstanceLayerProperties   vkEnumerateInstanceLayerProperties;
extern PFN_vkCreateInstance                     vkCreateInstance;
extern PFN_vkDestroyInstance                    vkDestroyInstance;
extern PFN_vkEnumeratePhysicalDevices           vkEnumeratePhysicalDevices;

extern PFN_vkDestroySurfaceKHR                  vkDestroySurfaceKHR;


//VK_KHR_get_surface_capabilities2
#if VK_KHR_get_surface_capabilities2
extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR  vkGetPhysicalDeviceSurfaceCapabilities2KHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormats2KHR       vkGetPhysicalDeviceSurfaceFormats2KHR;
#elif defined ( VK_NO_PROTOTYPES )
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR         vkGetPhysicalDeviceSurfaceFormatsKHR;
#endif //VK_KHR_get_surface_capabilities2

// VK_KHR_copy_commands2
#if VK_KHR_copy_commands2
extern PFN_vkCmdBlitImage2KHR                   vkCmdBlitImage2KHR;
extern PFN_vkCmdCopyBuffer2KHR                  vkCmdCopyBuffer2KHR;
extern PFN_vkCmdCopyBufferToImage2KHR           vkCmdCopyBufferToImage2KHR;
extern PFN_vkCmdCopyImage2KHR                   vkCmdCopyImage2KHR;
extern PFN_vkCmdCopyImageToBuffer2KHR           vkCmdCopyImageToBuffer2KHR;
#endif // VK_KHR_copy_commands2

#endif // VK_NO_PROTOTYPES

extern void vkLoadVulkanInstanceProcs( void );
extern void vkLoadVulkanProcs( VkInstance in_instance );
extern void vkLoadDeviceProcs( VkDevice in_device );
extern void vkLoadVulkanDebugUtilsProcs( VkInstance in_instance );
extern const char* crvkGetLastError( void );
extern void crvkAppendError( const char* in_error, const VkResult in_code );

template<typename _t>
class crvkDynamicVector;

template< typename _t >
class crvkPointer;

//#include "crvkPointer.hpp"
//#include "crvkDynamicVector.hpp"
#include "crvkException.hpp"
#include "crvkContext.hpp"
#include "crvkDevice.hpp"
#include "crvkFence.hpp"
#include "crvkCommandBuffer.hpp"
#include "crvkSemaphore.hpp"
#include "crvkSwapchain.hpp"
#include "crvkBuffer.hpp"
#include "crvkShaderStage.hpp"
#include "crvkPipeline.hpp"

#endif //__CRVK_IMPLEMENTATION_HPP__