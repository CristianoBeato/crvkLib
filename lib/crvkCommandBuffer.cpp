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
#include "crvkCommandBuffer.hpp"

crvkCommandBuffer::crvkCommandBuffer( void )
{
}

crvkCommandBuffer::~crvkCommandBuffer( void )
{
}

VkResult crvkCommandBuffer::Reset( const VkCommandBufferResetFlags in_flags )
{
    return vkResetCommandBuffer( m_commandBuffer, in_flags );
}

VkResult crvkCommandBuffer::Begin( const VkCommandBufferUsageFlags in_flags )
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = in_flags;
    return vkBeginCommandBuffer( m_commandBuffer, &beginInfo );   
}

VkResult crvkCommandBuffer::End( void )
{
    return vkEndCommandBuffer( m_commandBuffer );
}

void crvkCommandBuffer::Execute( const uint32_t in_commandBufferCount, const VkCommandBuffer* in_commandBuffers )
{
    vkCmdExecuteCommands( m_commandBuffer, in_commandBufferCount, in_commandBuffers );
}

void crvkCommandBuffer::BeginRenderPass( 
    const VkRenderPass in_renderPass,
    const VkFramebuffer in_framebuffer,
    const VkRect2D in_renderArea,
    const uint32_t in_clearValueCount,
    const VkClearValue* in_clearValues,
    const VkSubpassContents in_contents )
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = in_renderPass;
    renderPassInfo.framebuffer = in_framebuffer;
    renderPassInfo.renderArea = in_renderArea;
    renderPassInfo.clearValueCount = in_clearValueCount;
    renderPassInfo.pClearValues = in_clearValues;

    vkCmdBeginRenderPass( m_commandBuffer, &renderPassInfo, in_contents );
}

void crvkCommandBuffer::EndRenderPass( void )
{
    vkCmdEndRenderPass( m_commandBuffer );
}

void crvkCommandBuffer::BindPipeline( const VkPipelineBindPoint in_pipelineBindPoint, const VkPipeline in_pipeline )
{
    vkCmdBindPipeline( m_commandBuffer, in_pipelineBindPoint, in_pipeline );
}

void crvkCommandBuffer::WriteTimestamp(void)
{
}

void crvkCommandBuffer::ResetQueryPool(void)
{
}

void crvkCommandBuffer::BeginQuery( const VkQueryPool in_queryPool, const uint32_t in_query, const VkQueryControlFlags in_flags )
{
    vkCmdBeginQuery( m_commandBuffer, in_queryPool, in_query, in_flags );
}

void crvkCommandBuffer::EndQuery(void)
{
}
