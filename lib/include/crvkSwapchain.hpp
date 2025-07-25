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
      bool  Create( 
                    const crvkContext* in_context,
                    const crvkDevice* in_device, 
                    const uint32_t in_frames, 
                    const VkExtent2D in_extent, 
                    const VkSurfaceFormatKHR in_surfaceformat, 
                    const VkPresentModeKHR in_presentMode,
                    const VkSurfaceTransformFlagBitsKHR in_surfaceTransformFlag,
                    const bool in_recreate = false );

    /// @brief Release swapchain structures
    /// @param  
    void    Destroy( void );

    /// @brief Begin frame, prepare swapchain and check semaphores 
    /// @return VK_SUCCESS if not fail
    VkResult    Begin( void );

    /// @brief Execute command buffers 
    /// @param in_commandBuffers 
    /// @param in_commandBufferCount 
    /// @return 
    VkResult    End( const VkCommandBuffer* in_commandBuffers, const uint32_t in_commandBufferCount );

    /// @brief This will present to screen
    /// @return 
    VkResult    SwapBuffers( void );

    /// @brief The total concurrent frames  
    uint32_t    FrameCount( void ) const { return m_frameCount; }

    /// @brief the current swapchain frame
    /// @return a unsigned integer value 
    uint32_t        CurrentFrame( void ) const { return m_currentFrame; }

    /// @brief Return the swapchain frame buffer count 
    uint32_t        ImageCount( void ) const { return m_imageCount; }

    /// @brief Return the swapchain handle 
    VkSwapchainKHR  Swapchain( void ) const { return m_swapChain; }

    /// @brief Return the render pass configuration from the current frame buffer 
    VkRenderPass    RenderPass( void ) const { return m_renderPass; }

    /// @brief Return the current swapchain image frame buffer  
    VkFramebuffer   CurrentFramebuffer( void ) const { return m_framebuffers[m_imageIndex]; }

private:
    uint32_t                        m_currentFrame;
    uint32_t                        m_imageIndex;
    uint32_t                        m_imageCount;
    uint32_t                        m_frameCount;
    VkSwapchainKHR                  m_swapChain;
    VkRenderPass                    m_renderPass;
    crvkDevice*                     m_device;
    crvkPointer<VkImage>            m_images;
    crvkPointer<VkImageView>        m_imageViews;
    crvkPointer<VkFramebuffer>      m_framebuffers;
    crvkPointer<VkSemaphore>        m_imageAvailableSemaphores;
    crvkPointer<VkSemaphore>        m_renderFinishedSemaphores;
};

#endif //!__CRVK_SWAPCHAIN_HPP__