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

#ifndef __CRVK_SWAPCHAIN_HPP__
#define __CRVK_SWAPCHAIN_HPP__

typedef struct crvkSwapchainHandle_t crvkSwapchainHandle_t;

///
/// @brief Basic swapchain implementation, based on frame counter and syncs
///
class crvkSwapchain
{
public:
    crvkSwapchain( void );
    ~crvkSwapchain( void );

    /// @brief Create the swapchain 
    /// @param in_device 
    /// @param in_frames the number of concurrent frames that we can roll whit the swapchain
    /// @param in_extent the swapchain images proportion
    /// @param in_surfaceformat the image format of the swapchain 
    /// @param in_presentMode the presentation mode 
    /// @param in_surfaceTransformFlag 
    /// @param in_recreate if we are reacreatin the swapchain
    /// @return true on sucess, false on error 
    virtual bool  Create(   const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate = false );

    /// @brief Release swapchain structures
    /// @param  
    virtual void    Destroy( void );

    VkResult        AcquireImage( void );

    /// @brief This will present to screen, and swapbuffers
    /// @return 
    VkResult    PresentImage( const VkSemaphore* in_waitSemaphores, const uint32_t in_waitSemaphoresCount );

    /// @brief Return the swapchain frame buffer count 
    const VkImage*      Images( void ) const;
    const VkImageView*  ImageViews( void ) const;
    const VkImage       CurrentImage( void ) const;
    const VkImageView   CurrentImageView( void ) const;
    const VkSemaphore   CurrentSemaphore( void ) const;
    uint32_t            FrameCount( void ) const;
    uint32_t            ImageCount( void ) const;
    uint32_t            CurrentImageID( void ) const;
    VkExtent2D          Extent( void ) const;

    /// @brief Return the swapchain handle 
    VkSwapchainKHR      Swapchain( void ) const;

protected:
    crvkSwapchainHandle_t*          m_handle;
};

#endif //!__CRVK_SWAPCHAIN_HPP__