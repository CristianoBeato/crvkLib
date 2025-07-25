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

/*
==============================================
crvkCommandBuffer::crvkCommandBuffer
==============================================
*/
crvkCommandBuffer::crvkCommandBuffer( void ) : m_commandBuffer( nullptr )
{
}

/*
==============================================
crvkCommandBuffer::~crvkCommandBuffer
==============================================
*/
crvkCommandBuffer::~crvkCommandBuffer( void )
{
}

/*
==============================================
crvkCommandBuffer::Reset
==============================================
*/
VkResult crvkCommandBuffer::Reset( const VkCommandBufferResetFlags in_flags ) const
{
    return vkResetCommandBuffer( m_commandBuffer, in_flags );
}

/*
==============================================
crvkCommandBuffer::Begin
==============================================
*/
VkResult crvkCommandBuffer::Begin( const VkCommandBufferUsageFlags in_flags ) const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = in_flags;
    return vkBeginCommandBuffer( m_commandBuffer, &beginInfo );   
}

/*
==============================================
crvkCommandBuffer::End
==============================================
*/
VkResult crvkCommandBuffer::End( void ) const
{
    return vkEndCommandBuffer( m_commandBuffer );
}

/*
==============================================
crvkCommandBuffer::Execute
==============================================
*/
void crvkCommandBuffer::Execute( const uint32_t in_commandBufferCount, const VkCommandBuffer* in_commandBuffers ) const
{
    vkCmdExecuteCommands( m_commandBuffer, in_commandBufferCount, in_commandBuffers );
}

/*
==============================================
crvkCommandBuffer::SetViewport
==============================================
*/
void crvkCommandBuffer::SetViewport( const uint32_t in_firstViewport, const uint32_t in_viewportCount, const VkViewport *in_viewports ) const
{
    vkCmdSetViewport( m_commandBuffer, in_firstViewport, in_viewportCount, in_viewports );
}

/*
==============================================
crvkCommandBuffer::SetScissor
==============================================
*/
void crvkCommandBuffer::SetScissor( const uint32_t in_firstScissor, const uint32_t in_scissorCount, const VkRect2D *in_scissors ) const
{
    vkCmdSetScissor( m_commandBuffer, in_firstScissor, in_scissorCount, in_scissors );
}

/*
==============================================
crvkCommandBuffer::SetLineWidth
==============================================
*/
void crvkCommandBuffer::SetLineWidth( const float in_lineWidth ) const
{
    vkCmdSetLineWidth( m_commandBuffer, in_lineWidth );
}

/*
==============================================
crvkCommandBuffer::SetDepthBias
==============================================
*/
void crvkCommandBuffer::SetDepthBias( const float in_depthBiasConstantFactor, const float in_depthBiasClamp, const float in_depthBiasSlopeFactor ) const
{
    vkCmdSetDepthBias( m_commandBuffer, in_depthBiasConstantFactor, in_depthBiasClamp, in_depthBiasSlopeFactor );
}

/*
==============================================
crvkCommandBuffer::SetBlendConstants
==============================================
*/
void crvkCommandBuffer::SetBlendConstants(const float in_blendConstants[4])
{
    vkCmdSetBlendConstants( m_commandBuffer, in_blendConstants );
}

/*
==============================================
crvkCommandBuffer::SetBlendConstants
==============================================
*/
void crvkCommandBuffer::SetDepthBounds( const float in_minDepthBounds, const float in_maxDepthBounds )
{
    vkCmdSetDepthBounds( m_commandBuffer, in_minDepthBounds, in_maxDepthBounds );
}

/*
==============================================
crvkCommandBuffer::SetStencilCompareMask
==============================================
*/
void crvkCommandBuffer::SetStencilCompareMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_compareMask )
{
    vkCmdSetStencilCompareMask( m_commandBuffer, in_faceMask, in_compareMask );
}

/*
==============================================
crvkCommandBuffer::SetStencilWriteMask
==============================================
*/
void crvkCommandBuffer::SetStencilWriteMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_writeMask )
{
    vkCmdSetStencilWriteMask( m_commandBuffer, in_faceMask, in_writeMask );
}

/*
==============================================
crvkCommandBuffer::SetStencilReference
==============================================
*/
void crvkCommandBuffer::SetStencilReference( const VkStencilFaceFlags in_faceMask, const uint32_t in_reference )
{
    vkCmdSetStencilReference( m_commandBuffer, in_faceMask, in_reference );
}

/*
==============================================
crvkCommandBuffer::BindDescriptorSets
==============================================
*/
void crvkCommandBuffer::BindDescriptorSets( const VkPipelineBindPoint in_pipelineBindPoint, 
                                            const VkPipelineLayout in_layout, 
                                            const uint32_t in_firstSet, 
                                            const uint32_t in_descriptorSetCount, 
                                            const VkDescriptorSet* in_descriptorSets, 
                                            const uint32_t in_dynamicOffsetCount, 
                                            const uint32_t *in_dynamicOffsets )
{
    vkCmdBindDescriptorSets( m_commandBuffer, in_pipelineBindPoint, in_layout, in_firstSet, in_descriptorSetCount, in_descriptorSets, in_dynamicOffsetCount, in_dynamicOffsets );
}

/*
==============================================
crvkCommandBuffer::BindIndexBuffer
==============================================
*/
void crvkCommandBuffer::BindIndexBuffer( const VkBuffer in_buffer, const VkDeviceSize in_offset, const VkIndexType in_indexType )
{
    vkCmdBindIndexBuffer( m_commandBuffer, in_buffer, in_offset, in_indexType );
}

/*
==============================================
crvkCommandBuffer::BindVertexBuffers
==============================================
*/
void crvkCommandBuffer::BindVertexBuffers(  const uint32_t in_firstBinding, 
                                            const uint32_t in_bindingCount, 
                                            const VkBuffer *in_buffers, 
                                            const VkDeviceSize *in_offsets, 
                                            const VkDeviceSize* in_sizes,
                                            const VkDeviceSize* in_strides
                                        )
{


    vkCmdBindVertexBuffers2( m_commandBuffer, in_firstBinding, in_bindingCount, in_buffers, in_offsets, in_sizes, in_strides );
}

/*
==============================================
crvkCommandBuffer::Draw
==============================================
*/
void crvkCommandBuffer::Draw(   const uint32_t in_vertexCount, 
                                const uint32_t in_instanceCount, 
                                const uint32_t in_firstVertex, 
                                const uint32_t in_firstInstance)
{
    vkCmdDraw( m_commandBuffer, in_vertexCount, in_instanceCount, in_firstVertex, in_firstInstance );
}

/*
==============================================
crvkCommandBuffer::DrawIndexed
==============================================
*/
void crvkCommandBuffer::DrawIndexed( const uint32_t in_indexCount, const uint32_t in_instanceCount, const uint32_t in_firstIndex, const int32_t in_vertexOffset, const uint32_t in_firstInstance)
{
    vkCmdDrawIndexed( m_commandBuffer, in_indexCount, in_instanceCount, in_firstIndex, in_vertexOffset, in_firstInstance );
}

/*
==============================================
crvkCommandBuffer::DrawIndirect
==============================================
*/
void crvkCommandBuffer::DrawIndirect( const VkBuffer in_buffer, const VkDeviceSize in_offset, const uint32_t in_drawCount, const uint32_t in_stride)
{
}

/*
==============================================
crvkCommandBuffer::DrawIndexedIndirect
==============================================
*/
void crvkCommandBuffer::DrawIndexedIndirect( const VkBuffer in_buffer, const VkDeviceSize in_offset, const uint32_t in_drawCount, const uint32_t in_stride)
{
}

/*
==============================================
crvkCommandBuffer::Dispatch
==============================================
*/
void crvkCommandBuffer::Dispatch( const uint32_t in_groupCountX, const uint32_t in_groupCountY, const uint32_t in_groupCountZ)
{
}

/*
==============================================
crvkCommandBuffer::DispatchIndirect
==============================================
*/
void crvkCommandBuffer::DispatchIndirect( const VkBuffer in_buffer, const VkDeviceSize in_offset)
{
}

/*
==============================================
crvkCommandBuffer::CopyBuffer
==============================================
*/
void crvkCommandBuffer::CopyBuffer( const VkBuffer in_srcBuffer, const VkBuffer in_dstBuffer, const uint32_t in_regionCount, const VkBufferCopy *in_regions)
{
}

/*
==============================================
crvkCommandBuffer::CopyImage
==============================================
*/
void crvkCommandBuffer::CopyImage( const VkImage in_srcImage, const VkImageLayout in_srcImageLayout, const VkImage in_dstImage, const VkImageLayout in_dstImageLayout, const uint32_t in_regionCount, const VkImageCopy *in_regions)
{
}

/*
==============================================
crvkCommandBuffer::BlitImage
==============================================
*/
void crvkCommandBuffer::BlitImage( const VkImage in_srcImage, const VkImageLayout in_srcImageLayout, const VkImage in_dstImage, const VkImageLayout in_dstImageLayout, const uint32_t in_regionCount, const VkImageBlit *in_pRegions, const VkFilter in_filter)
{
}

/*
==============================================
crvkCommandBuffer::CopyBufferToImage
==============================================
*/
void crvkCommandBuffer::CopyBufferToImage( const VkBuffer in_srcBuffer, const VkImage in_dstImage, const VkImageLayout in_dstImageLayout, const uint32_t in_regionCount, const VkBufferImageCopy *in_regions)
{
}

/*
==============================================
crvkCommandBuffer::CopyImageToBuffer
==============================================
*/
void crvkCommandBuffer::CopyImageToBuffer(  const VkImage in_srcImage, 
                                            const VkImageLayout in_srcImageLayout, 
                                            const VkBuffer in_dstBuffer, 
                                            const uint32_t in_regionCount, 
                                            const VkBufferImageCopy2 *in_regions,
                                            const void* in_next )
{
    VkCopyImageToBufferInfo2 copyImageToBufferInfo{};
    copyImageToBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
    copyImageToBufferInfo.srcImage = in_srcImage;
    copyImageToBufferInfo.srcImageLayout = in_srcImageLayout;
    copyImageToBufferInfo.dstBuffer = in_dstBuffer;
    copyImageToBufferInfo.regionCount = in_regionCount;
    copyImageToBufferInfo.pRegions = in_regions;
    copyImageToBufferInfo.pNext = in_next;
    vkCmdCopyImageToBuffer2( m_commandBuffer, &copyImageToBufferInfo );
}

/*
==============================================
crvkCommandBuffer::UpdateBuffer
==============================================
*/
void crvkCommandBuffer::UpdateBuffer( const VkBuffer in_dstBuffer, const VkDeviceSize in_dstOffset, const VkDeviceSize in_dataSize, const void *in_data)
{
    vkCmdUpdateBuffer( m_commandBuffer, in_dstBuffer, in_dstOffset, in_dataSize, in_data );
}

/*
==============================================
crvkCommandBuffer::FillBuffer
==============================================
*/
void crvkCommandBuffer::FillBuffer( const VkBuffer in_dstBuffer, const VkDeviceSize in_dstOffset, const VkDeviceSize in_size, const uint32_t in_data)
{
    vkCmdFillBuffer( m_commandBuffer, in_dstBuffer, in_dstOffset, in_size, in_data );
}

/*
==============================================
crvkCommandBuffer::ClearColorImage
==============================================
*/
void crvkCommandBuffer::ClearColorImage( const VkImage in_image, const VkImageLayout in_imageLayout, const VkClearColorValue *in_color, const uint32_t in_rangeCount, const VkImageSubresourceRange *in_ranges)
{
    vkCmdClearColorImage( m_commandBuffer, in_image, in_imageLayout, in_color, in_rangeCount, in_ranges );
}

/*
==============================================
crvkCommandBuffer::ClearDepthStencilImage
==============================================
*/
void crvkCommandBuffer::ClearDepthStencilImage( const VkImage in_image, const VkImageLayout in_imageLayout, const VkClearDepthStencilValue *in_depthStencil, const uint32_t in_rangeCount, const VkImageSubresourceRange *in_ranges)
{
    vkCmdClearDepthStencilImage( m_commandBuffer, in_image, in_imageLayout, in_depthStencil, in_rangeCount, in_ranges );
}

/*
==============================================
crvkCommandBuffer::ResolveImage
==============================================
*/
void crvkCommandBuffer::ResolveImage(   const VkImage in_srcImage,
                                        const VkImageLayout in_srcImageLayout,
                                        const VkImage in_dstImage,
                                        const VkImageLayout in_dstImageLayout,
                                        const uint32_t in_regionCount,
                                        const VkImageResolve2* in_regions, 
                                        const void* in_next )
{
    VkResolveImageInfo2 resolveImageInfo{};
    resolveImageInfo.sType = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2;
    resolveImageInfo.srcImage = in_srcImage;
    resolveImageInfo.srcImageLayout = in_srcImageLayout;
    resolveImageInfo.dstImage = in_dstImage;
    resolveImageInfo.dstImageLayout = in_dstImageLayout;
    resolveImageInfo.regionCount = in_regionCount;
    resolveImageInfo.pRegions = in_regions;
    resolveImageInfo.pNext = in_next;
    vkCmdResolveImage2( m_commandBuffer, &resolveImageInfo );
}

/*
==============================================
crvkCommandBuffer::ExecuteCommands
==============================================
*/
void crvkCommandBuffer::ExecuteCommands( const uint32_t in_commandBufferCount, const VkCommandBuffer *in_commandBuffers)
{
    vkCmdExecuteCommands( m_commandBuffer, in_commandBufferCount, in_commandBuffers );
}

/*
==============================================
crvkCommandBuffer::ClearAttachments
==============================================
*/
void crvkCommandBuffer::ClearAttachments( const uint32_t in_attachmentCount, const VkClearAttachment *in_attachments, const uint32_t in_rectCount, const VkClearRect *in_rects)
{
    vkCmdClearAttachments( m_commandBuffer, in_attachmentCount, in_attachments, in_rectCount, in_rects );
}

/*
==============================================
crvkCommandBuffer::BeginRenderPass
==============================================
*/
void crvkCommandBuffer::BeginRenderPass(
    const VkRenderPass in_renderPass,
    const VkFramebuffer in_framebuffer,
    const VkRect2D in_renderArea,
    const uint32_t in_clearValueCount,
    const VkClearValue *in_clearValues,
    const VkSubpassContents in_contents ) const
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

/*
==============================================
crvkCommandBuffer::EndRenderPass
==============================================
*/
void crvkCommandBuffer::EndRenderPass( void ) const
{
    vkCmdEndRenderPass( m_commandBuffer );
}

/*
==============================================
crvkCommandBuffer::BindPipeline
==============================================
*/
void crvkCommandBuffer::BindPipeline( const VkPipelineBindPoint in_pipelineBindPoint, const VkPipeline in_pipeline ) const
{
    vkCmdBindPipeline( m_commandBuffer, in_pipelineBindPoint, in_pipeline );
}

/*
==============================================
crvkCommandBuffer::PipelineBarrier
==============================================
*/
void crvkCommandBuffer::PipelineBarrier(    const VkDependencyFlags in_dependencyFlags, 
                                            const uint32_t in_memoryBarrierCount, 
                                            const VkMemoryBarrier2 *in_memoryBarriers, 
                                            const uint32_t in_bufferMemoryBarrierCount, 
                                            const VkBufferMemoryBarrier2 *in_bufferMemoryBarriers, 
                                            const uint32_t in_imageMemoryBarrierCount, 
                                            const VkImageMemoryBarrier2 *in_imageMemoryBarriers, 
                                            const void *in_nex ) const
{
    VkDependencyInfo dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.dependencyFlags = in_dependencyFlags;
    dependencyInfo.memoryBarrierCount = in_memoryBarrierCount;
    dependencyInfo.pMemoryBarriers = in_memoryBarriers;
    dependencyInfo.bufferMemoryBarrierCount = in_bufferMemoryBarrierCount;
    dependencyInfo.pBufferMemoryBarriers = in_bufferMemoryBarriers;
    dependencyInfo.imageMemoryBarrierCount = in_imageMemoryBarrierCount;
    dependencyInfo.pImageMemoryBarriers = in_imageMemoryBarriers;
    dependencyInfo.pNext = nullptr;
    vkCmdPipelineBarrier2( m_commandBuffer, &dependencyInfo );
}



/*
==============================================
crvkCommandBuffer::WaitEvents
==============================================
*/
void crvkCommandBuffer::WaitEvents( const uint32_t in_eventCount,
                                    const const VkEvent* in_events, 
                                    const VkDependencyFlags in_dependencyFlags,
                                    const uint32_t in_memoryBarrierCount,
                                    const VkMemoryBarrier2* in_memoryBarriers,
                                    const uint32_t in_bufferMemoryBarrierCount,
                                    const VkBufferMemoryBarrier2* in_bufferMemoryBarriers,
                                    const uint32_t in_imageMemoryBarrierCount,
                                    const VkImageMemoryBarrier2* in_imageMemoryBarriers,
                                    const void* in_next ) const
{
    VkDependencyInfo dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.dependencyFlags = in_dependencyFlags;
    dependencyInfo.memoryBarrierCount = in_memoryBarrierCount;
    dependencyInfo.pMemoryBarriers = in_memoryBarriers;
    dependencyInfo.bufferMemoryBarrierCount = in_bufferMemoryBarrierCount;
    dependencyInfo.pBufferMemoryBarriers = in_bufferMemoryBarriers; 
    dependencyInfo.imageMemoryBarrierCount = in_imageMemoryBarrierCount;
    dependencyInfo.pImageMemoryBarriers = in_imageMemoryBarriers;
    dependencyInfo.pNext = in_next;
    vkCmdWaitEvents2( m_commandBuffer, in_eventCount, in_events, &dependencyInfo );
}

/*
==============================================
crvkCommandBuffer::SetEvent
==============================================
*/
void crvkCommandBuffer::SetEvent(const VkEvent in_event, const VkPipelineStageFlags in_stageMask ) const
{
    // TODO: vkCmdSetEvent2
    vkCmdSetEvent( m_commandBuffer, in_event, in_stageMask );
}

/*
==============================================
crvkCommandBuffer::ResetEvent
==============================================
*/
void crvkCommandBuffer::ResetEvent(const VkEvent in_event, const VkPipelineStageFlags in_stageMask ) const
{
    vkCmdResetEvent( m_commandBuffer, in_event, in_stageMask );
}

/*
==============================================
crvkCommandBuffer::PushConstants
==============================================
*/
void crvkCommandBuffer::PushConstants( const VkPipelineLayout in_layout, const VkShaderStageFlags in_stageFlags, const uint32_t in_offset, const uint32_t in_size, const void *in_values ) const
{
    vkCmdPushConstants( m_commandBuffer, in_layout, in_stageFlags, in_offset, in_size, in_values );
}

/*
==============================================
crvkCommandBuffer::CopyQueryPoolResults
==============================================
*/
void crvkCommandBuffer::CopyQueryPoolResults( const VkQueryPool in_queryPool, const uint32_t in_firstQuery, const uint32_t in_queryCount, const VkBuffer in_dstBuffer, const VkDeviceSize in_dstOffset, const VkDeviceSize in_stride, const VkQueryResultFlags in_flags ) const
{
    vkCmdCopyQueryPoolResults( m_commandBuffer, in_queryPool, in_firstQuery, in_queryCount, in_dstBuffer, in_dstOffset, in_stride, in_flags );
}

/*
==============================================
crvkCommandBuffer::WriteTimestamp
==============================================
*/
void crvkCommandBuffer::WriteTimestamp( const VkPipelineStageFlagBits in_pipelineStage, const VkQueryPool in_queryPool, const uint32_t in_query ) const
{
    vkCmdWriteTimestamp( m_commandBuffer, in_pipelineStage, in_queryPool, in_query );
}

/*
==============================================
crvkCommandBuffer::BeginQuery
==============================================
*/
void crvkCommandBuffer::BeginQuery( const VkQueryPool in_queryPool, const uint32_t in_query, const VkQueryControlFlags in_flags ) const
{
    vkCmdBeginQuery( m_commandBuffer, in_queryPool, in_query, in_flags );
}

/*
==============================================
crvkCommandBuffer::ResetQueryPool
==============================================
*/
void crvkCommandBuffer::ResetQueryPool( const VkQueryPool in_queryPool, const uint32_t in_firstQuery, const uint32_t in_queryCount ) const
{
    vkCmdResetQueryPool( m_commandBuffer, in_queryPool, in_firstQuery, in_queryCount ); 
}

/*
==============================================
crvkCommandBuffer::EndQuery
==============================================
*/
void crvkCommandBuffer::EndQuery( const VkQueryPool in_queryPool, const uint32_t in_query ) const
{
    vkCmdEndQuery( m_commandBuffer, in_queryPool,  in_query );
}
