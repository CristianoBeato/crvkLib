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

class crvkFrameBuffer
{
public:
    virtual bool    Create( 
            const crvkDevice* in_device, 
            const uint32_t in_bufferCount,
            const VkFramebufferCreateFlags in_flags,
            const uint32_t in_attachmentCount,
            const VkImageView* in_attachments,
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
    virtual VkRenderPass    RenderPass( void ) const = 0;
    virtual VkFramebuffer   Framebuffer( void ) const = 0;

};

class crvkFrameBufferStatic : public crvkFrameBuffer
{
public:
    crvkFrameBufferStatic( void );
    ~crvkFrameBufferStatic( void );

    virtual bool    Create( 
        const crvkDevice* in_device, 
        const uint32_t in_bufferCount,
        const VkFramebufferCreateFlags in_flags,
        const uint32_t in_attachmentCount,
        const VkImageView* in_attachments,
        const uint32_t in_width,
        const uint32_t in_height,
        const uint32_t in_layers,
        const uint32_t in_attachmentDescriptionsCount,
        const VkAttachmentDescription* in_attachmentsDescriptions,
        const uint32_t in_subpassCount,
        const VkSubpassDescription* in_subpasses,
        const uint32_t in_dependencyCount,
        const VkSubpassDependency* in_dependencies,
        const bool in_recreate ) override;
    void    Destroy( void ) override;
    VkRenderPass    RenderPass( void ) const override { return m_renderPass; }
    VkFramebuffer   Framebuffer( void ) const override { return m_frameBuffer; }

protected:
    crvkDevice*     m_device;
    VkRenderPass    m_renderPass;
    VkFramebuffer   m_frameBuffer;
};

class crvkFrameBufferRoundRobin : public crvkFrameBuffer
{
public:
    crvkFrameBufferRoundRobin( void );
    ~crvkFrameBufferRoundRobin( void );
    virtual bool    Create( 
        const crvkDevice* in_device, 
        const uint32_t in_bufferCount,
        const VkFramebufferCreateFlags in_flags,
        const uint32_t in_attachmentCount,
        const VkImageView* in_attachments,
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
    virtual void    Destroy( void ) override;
    virtual VkRenderPass    RenderPass( void ) const override { return m_renderPass; }
    virtual VkFramebuffer   Framebuffer( void ) const override { return m_frameBuffers[m_frameCount]; }

private:
    uint32_t                            m_frame;
    uint32_t                            m_frameCount;
    crvkDevice*                         m_device;
    VkRenderPass                        m_renderPass;
    crvkDynamicVector<VkFramebuffer>    m_frameBuffers;
};

#endif //__CRVK_FRAME_BUFFER_HPP__