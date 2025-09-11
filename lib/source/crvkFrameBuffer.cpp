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

typedef struct crvkFrameBufferHandle_t 
{
    uint32_t            numFrambebuffers = 0;
    VkExtent2D          extent;
    VkRenderPass        renderpass = nullptr;
    VkFramebuffer*      framebufferArray = nullptr;
    VkDevice            device = nullptr;
} crvkFrameBufferHandle_t;

crvkFrameBuffer::crvkFrameBuffer( void ) : m_handle( nullptr ) 
{
    m_handle = new crvkFrameBufferHandle_t();
}

crvkFrameBuffer::~crvkFrameBuffer( void )
{
    Destroy();
    delete m_handle;
    m_handle = nullptr;
}

bool crvkFrameBuffer::Create( 
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
    const bool in_recreate )
{
    VkResult result = VK_SUCCESS;

    m_handle->device = in_device->Device();

    ///
    /// Create render pass configuration 
    /// ==========================================================================
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = in_attachmentDescriptionsCount;
    renderPassInfo.pAttachments = in_attachmentsDescriptions;
    renderPassInfo.subpassCount = in_subpassCount;
    renderPassInfo.pSubpasses = in_subpasses;
    renderPassInfo.dependencyCount = in_dependencyCount;
    renderPassInfo.pDependencies = in_dependencies;
    result = vkCreateRenderPass( m_handle->device, &renderPassInfo, nullptr, &m_handle->renderpass ); 
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkSwapchain::Create::vkCreateFramebuffer", result );
        return false;
    }

    VkFramebufferCreateInfo framebufferCI{};
    framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCI.renderPass = m_handle->renderpass;
    framebufferCI.width = in_width;
    framebufferCI.height = in_height;
    framebufferCI.layers = in_layers;

    if ( !in_recreate )
        m_handle->framebufferArray = static_cast<VkFramebuffer*>( SDL_malloc( sizeof( VkFramebuffer ) * m_handle->numFrambebuffers ) );
    
    for ( uint32_t i = 0; i < m_handle->numFrambebuffers; i++)
    {
        framebufferCI.attachmentCount = in_attachaments[i].count;
        framebufferCI.pAttachments = in_attachaments[i].attachaments;

        if ( in_recreate &&  m_handle->framebufferArray[i] != nullptr )
            vkDestroyFramebuffer( m_handle->device, m_handle->framebufferArray[i], k_allocationCallbacks );
        
        result = vkCreateFramebuffer( m_handle->device, &framebufferCI, nullptr, &m_handle->framebufferArray[i] );
        if ( result != VK_SUCCESS ) 
        {
            crvkAppendError( "crvkSwapchain::Create::vkCreateFramebuffer", result );
            return false;
        }
    }
    return true;
}

void crvkFrameBuffer::Destroy( void )
{
    if ( m_handle == nullptr )
        return;

    for ( uint32_t i = 0; i < m_handle->numFrambebuffers; i++)
    {
        if ( m_handle->framebufferArray[i] == nullptr )
            continue;

        vkDestroyFramebuffer( m_handle->device, m_handle->framebufferArray[i], k_allocationCallbacks );
        m_handle->framebufferArray[i] = nullptr;
    }
    
    if ( m_handle->framebufferArray != nullptr )
    {
        SDL_free( m_handle->framebufferArray );
        m_handle->framebufferArray;
    }

    if( m_handle->renderpass != nullptr )
    {
        vkDestroyRenderPass( m_handle->device, m_handle->renderpass, k_allocationCallbacks );
        m_handle->renderpass = nullptr;
    }
}

uint32_t crvkFrameBuffer::FrameBufferCount(void) const
{
    if ( m_handle == nullptr )
        return 0;
    
    return m_handle->numFrambebuffers;
}

VkRenderPass crvkFrameBuffer::RenderPass( void ) const
{
    if( m_handle == nullptr )
        return nullptr;

    return m_handle->renderpass;
}

VkFramebuffer* crvkFrameBuffer::Framebuffers( void ) const
{
    if( m_handle == nullptr )
        return nullptr;

    return m_handle->framebufferArray;
}

VkFramebuffer crvkFrameBuffer::GetFrameBuffer(const uint32_t in_index ) const
{
    uint32_t index = 0;
    if ( m_handle == nullptr )
        return nullptr;
    index = std::min( m_handle->numFrambebuffers - 1, in_index );
    return m_handle->framebufferArray[index];
}