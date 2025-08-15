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

#include "crvkPrecompiled.hpp"
#include "crvkFrameBuffer.hpp"

crvkFrameBufferStatic::crvkFrameBufferStatic( void ) : m_renderPass( nullptr ), m_frameBuffer( nullptr ) 
{
}

crvkFrameBufferStatic::~crvkFrameBufferStatic( void )
{
}

bool crvkFrameBufferStatic::Create( 
    const crvkDevice* in_device, 
    const uint32_t in_bufferCount,
    const VkFramebufferCreateFlags in_flags,
    const uint32_t in_attachmentCount,
    const VkImageView* in_attachments,
    const uint32_t in_width,
    const uint32_t in_height,
    const uint32_t in_layers,
    const VkAttachmentDescription* in_attachmentsDescriptions,
    const uint32_t in_subpassCount,
    const VkSubpassDescription* in_subpasses,
    const uint32_t in_dependencyCount,
    const VkSubpassDependency* in_dependencies,
    const bool in_recreate )
{
    VkResult result = VK_SUCCESS;
    ///
    /// Create render pass configuration 
    /// ==========================================================================

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = in_attachmentCount;
    renderPassInfo.pAttachments = in_attachmentsDescriptions;
    renderPassInfo.subpassCount = in_subpassCount;
    renderPassInfo.pSubpasses = in_subpasses;
    renderPassInfo.dependencyCount = in_dependencyCount;
    renderPassInfo.pDependencies = in_dependencies;

    result = vkCreateRenderPass( m_device->Device(), &renderPassInfo, nullptr, &m_renderPass ); 
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkSwapchain::Create::vkCreateFramebuffer", result );
        return false;
    }

    VkFramebufferCreateInfo framebufferCI{};
    framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCI.renderPass = m_renderPass;
    framebufferCI.attachmentCount = 1;
    framebufferCI.pAttachments = in_attachments;
    framebufferCI.width = in_width;
    framebufferCI.height = in_height;
    framebufferCI.layers = in_layers;

    if ( in_recreate )
        vkDestroyFramebuffer( m_device->Device(), m_frameBuffer, k_allocationCallbacks );

    result = vkCreateFramebuffer( m_device->Device(), &framebufferCI, nullptr, &m_frameBuffer );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkSwapchain::Create::vkCreateFramebuffer", result );
        return false;
    }

    return true;
}

void crvkFrameBufferStatic::Destroy( void )
{
    if ( m_frameBuffer != nullptr )
    {
        vkDestroyFramebuffer( m_device->Device(), m_frameBuffer, k_allocationCallbacks );
        m_frameBuffer = nullptr;
    }

    if( m_renderPass != nullptr )
    {
        vkDestroyRenderPass( m_device, m_renderPass, k_allocationCallbacks );
        m_renderPass = nullptr;
    }
    
    m_device = nullptr;
}

crvkFrameBufferRoundRobin::crvkFrameBufferRoundRobin( void )
{
}

crvkFrameBufferRoundRobin::~crvkFrameBufferRoundRobin( void )
{
}