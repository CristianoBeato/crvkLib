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

#ifndef __CRVK_FRAME_BUFFER_HPP__
#define __CRVK_FRAME_BUFFER_HPP__

typedef struct crvkFrameBufferHandle_t crvkFrameBufferHandle_t;

class crvkFrameBuffer
{
public:
    struct Attachament_t
    {
        uint32_t        count = 0;              // attachaments count 
        VkImageView*    attachaments = nullptr; // framebuffer attachaments
    };

    virtual bool    Create( 
            const crvkDevice* in_device, 
            const uint32_t in_bufferCount,
            const Attachament_t* in_attachaments,
            const VkFramebufferCreateFlags in_flags,
            const uint32_t in_width,
            const uint32_t in_height,
            const uint32_t in_layers,
            const uint32_t in_attachmentDescriptionsCount,
            const VkAttachmentDescription* in_attachmentsDescriptions,
            const uint32_t in_subpassCount,
            const VkSubpassDescription* in_subpasses,
            const uint32_t in_dependencyCount,
            const VkSubpassDependency* in_dependencies,
            const bool in_recreate );
    virtual void    Destroy( void );
    
    /// @brief The number of frame buffers objects
    /// @return the count of frame buffers 
    uint32_t        FrameBufferCount( void ) const;

    /// @brief Return the frame buffer renderpass configuration
    /// @return the handle for vulkan renderpass  
    VkRenderPass    RenderPass( void ) const;
    
    /// @brief Return the array of frame buffers objects 
    VkFramebuffer*  Framebuffers( void ) const;

    /// @brief Return a specific framebufer from array
    /// @param in_index the index of the framebuffer in the array
    VkFramebuffer   GetFrameBuffer( const uint32_t in_index ) const;

protected:
        crvkFrameBufferHandle_t*    m_handle;
};

#endif //__CRVK_FRAME_BUFFER_HPP__