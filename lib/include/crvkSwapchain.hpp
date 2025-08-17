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

    /// @brief This will present to screen
    /// @return 
    VkResult    PresentImage( const VkSemaphore* in_waitSemaphores, const uint32_t in_waitSemaphoresCount );

    /// @brief Return the swapchain frame buffer count 
    const VkImage*      Images( void ) const;
    const VkImageView*  ImageViews( void ) const;
    const VkImage       CurrentImage( void ) const { return m_images[m_currentImage]; }
    const VkImageView   CurrentImageView( void ) const { return m_imageViews[m_currentImage]; }
    uint32_t            ImageCount( void ) const { return m_imageCount; }
    uint32_t            CurrentImageID( void ) const { return m_imageCount; }
    VkExtent2D          Extent( void ) const { return m_extent; }

    /// @brief Return the swapchain handle 
    VkSwapchainKHR      Swapchain( void ) const { return m_swapChain; }

protected:
    uint32_t                        m_imageCount;   // total swapchain images 
    uint32_t                        m_currentImage;   // current frame buffer
    VkExtent2D                      m_extent;
    VkSwapchainKHR                  m_swapChain;
    crvkDevice*                     m_device;
    crvkDynamicVector<VkImage>      m_images;
    crvkDynamicVector<VkImageView>  m_imageViews;
    crvkDynamicVector<VkSemaphore>  m_imageAvailable;

};

class crvkSwapchainDynamic : public crvkSwapchain
{
public:
    crvkSwapchainDynamic( void );
    ~crvkSwapchainDynamic( void );

      /// @brief Create the swapchain 
      /// @param in_device 
      /// @param in_frames the number of concurrent frames that we can roll whit the swapchain
      /// @param in_extent the swapchain images proportion
      /// @param in_surfaceformat the image format of the swapchain 
      /// @param in_presentMode the presentation mode 
      /// @param in_surfaceTransformFlag 
      /// @param in_recreate if we are reacreatin the swapchain
      /// @return true on sucess, false on error 
      virtual bool  Create( 
                    const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate = false ) override;

    virtual void    Destroy( void ) override;

    /// @brief Begin frame, prepare swapchain and check semaphores 
    /// @return VK_SUCCESS if not fail
    VkResult    Begin( const VkClearColorValue in_clearColor, const VkClearDepthStencilValue in_clearDepthStencil );

    /// @brief Execute command buffers 
    /// @param in_commandBuffers 
    /// @param in_commandBufferCount 
    /// @return 
    VkResult    End( void );

    /// @brief This will present to screen
    /// @return 
    VkResult    SwapBuffers( void );

    /// @brief The total concurrent frames  
    uint32_t    FrameCount( void ) const { return m_frameCount; }

    /// @brief the current swapchain frame
    /// @return a unsigned integer value 
    uint32_t    CurrentFrame( void ) const { return m_frame; }
 
private:
    uint32_t                            m_frameCount;   // number of concurrent frames
    uint32_t                            m_frame;
    crvkDynamicVector<VkSemaphore>      m_renderFinished;
    crvkDynamicVector<VkCommandBuffer>  m_commandBuffers;
};

#endif //!__CRVK_SWAPCHAIN_HPP__