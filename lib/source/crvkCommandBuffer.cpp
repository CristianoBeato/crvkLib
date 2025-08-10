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
void crvkCommandBuffer::SetBlendConstants(const float in_blendConstants[4]) const
{
    vkCmdSetBlendConstants( m_commandBuffer, in_blendConstants );
}

/*
==============================================
crvkCommandBuffer::SetBlendConstants
==============================================
*/
void crvkCommandBuffer::SetDepthBounds( const float in_minDepthBounds, const float in_maxDepthBounds ) const
{
    vkCmdSetDepthBounds( m_commandBuffer, in_minDepthBounds, in_maxDepthBounds );
}

void crvkCommandBuffer::SetCullMode( const VkCullModeFlags in_cullMode ) const
{
    vkCmdSetCullMode( m_commandBuffer, in_cullMode );
}

/*
==============================================
crvkCommandBuffer::SetStencilCompareMask
==============================================
*/
void crvkCommandBuffer::SetStencilCompareMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_compareMask ) const
{
    vkCmdSetStencilCompareMask( m_commandBuffer, in_faceMask, in_compareMask );
}

/*
==============================================
crvkCommandBuffer::SetStencilWriteMask
==============================================
*/
void crvkCommandBuffer::SetStencilWriteMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_writeMask ) const
{
    vkCmdSetStencilWriteMask( m_commandBuffer, in_faceMask, in_writeMask );
}

/*
==============================================
crvkCommandBuffer::SetStencilReference
==============================================
*/
void crvkCommandBuffer::SetStencilReference( const VkStencilFaceFlags in_faceMask, const uint32_t in_reference ) const
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
void crvkCommandBuffer::BindIndexBuffer( const VkBuffer in_buffer, const VkDeviceSize in_offset, const VkIndexType in_indexType ) const
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
                                        ) const
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
                                const uint32_t in_firstInstance ) const
{
    vkCmdDraw( m_commandBuffer, in_vertexCount, in_instanceCount, in_firstVertex, in_firstInstance );
}

/*
==============================================
crvkCommandBuffer::DrawIndexed
==============================================
*/
void crvkCommandBuffer::DrawIndexed( const uint32_t in_indexCount, 
                                     const uint32_t in_instanceCount, 
                                     const uint32_t in_firstIndex, 
                                     const int32_t in_vertexOffset, 
                                     const uint32_t in_firstInstance ) const
{
    vkCmdDrawIndexed( m_commandBuffer, in_indexCount, in_instanceCount, in_firstIndex, in_vertexOffset, in_firstInstance );
}

/*
==============================================
crvkCommandBuffer::DrawIndirect
==============================================
*/
void crvkCommandBuffer::DrawIndirect(   const VkBuffer in_buffer, 
                                        const VkDeviceSize in_offset, 
                                        const uint32_t in_drawCount, 
                                        const uint32_t in_stride )  const
{
    vkCmdDrawIndirect( m_commandBuffer, in_buffer , in_offset, in_drawCount, in_stride );
}

/*
==============================================
crvkCommandBuffer::DrawIndexedIndirect
==============================================
*/
void crvkCommandBuffer::DrawIndexedIndirect(    const VkBuffer in_buffer, 
                                                const VkDeviceSize in_offset, 
                                                const uint32_t in_drawCount, 
                                                const uint32_t in_stride ) const
{
    vkCmdDrawIndexedIndirect( m_commandBuffer, in_buffer, in_offset, in_drawCount, in_stride );
}

/*
==============================================
crvkCommandBuffer::Dispatch
==============================================
*/
void crvkCommandBuffer::Dispatch(   const uint32_t in_groupCountX, 
                                    const uint32_t in_groupCountY, 
                                    const uint32_t in_groupCountZ ) const
{
    vkCmdDispatch( m_commandBuffer, in_groupCountX, in_groupCountY, in_groupCountZ );
}

/*
==============================================
crvkCommandBuffer::DispatchIndirect
==============================================
*/
void crvkCommandBuffer::DispatchIndirect(   const VkBuffer in_buffer, 
                                            const VkDeviceSize in_offset )  const
{
    vkCmdDispatchIndirect( m_commandBuffer, in_buffer, in_offset );
}

/*
==============================================
crvkCommandBuffer::CopyBuffer
==============================================
*/
void crvkCommandBuffer::CopyBuffer( const VkBuffer in_srcBuffer, 
                                    const VkBuffer in_dstBuffer, 
                                    const uint32_t in_regionCount, 
                                    const VkBufferCopy2 *in_regions, 
                                    const void* in_next ) const
{
    VkCopyBufferInfo2   copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.srcBuffer = in_srcBuffer;
    copyBufferInfo.dstBuffer = in_dstBuffer;
    copyBufferInfo.regionCount = in_regionCount;
    copyBufferInfo.pRegions = in_regions;
    copyBufferInfo.pNext = in_next;
    vkCmdCopyBuffer2(m_commandBuffer, &copyBufferInfo );
}

/*
==============================================
crvkCommandBuffer::CopyImage
==============================================
*/
void crvkCommandBuffer::CopyImage(  const VkImage in_srcImage, 
                                    const VkImageLayout in_srcImageLayout, 
                                    const VkImage in_dstImage, 
                                    const VkImageLayout in_dstImageLayout, 
                                    const uint32_t in_regionCount, 
                                    const VkImageCopy2 *in_regions,
                                    const void* in_next ) const
{
    VkCopyImageInfo2    copyImageInfo{};
    copyImageInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
    copyImageInfo.srcImage = in_srcImage;
    copyImageInfo.srcImageLayout = in_srcImageLayout;
    copyImageInfo.dstImage = in_dstImage;
    copyImageInfo.dstImageLayout = in_dstImageLayout;
    copyImageInfo.regionCount = in_regionCount;
    copyImageInfo.pRegions = in_regions;
    copyImageInfo.pNext = in_next;
    vkCmdCopyImage2( m_commandBuffer, &copyImageInfo );   
}

/*
==============================================
crvkCommandBuffer::BlitImage
==============================================
*/
void crvkCommandBuffer::BlitImage(  const VkImage in_srcImage, 
                                    const VkImageLayout in_srcImageLayout, 
                                    const VkImage in_dstImage, 
                                    const VkImageLayout in_dstImageLayout, 
                                    const uint32_t in_regionCount, 
                                    const VkImageBlit2 *in_regions, 
                                    const VkFilter in_filter, 
                                    const void* in_next ) const
{
    VkBlitImageInfo2    blitImageInfo{};
    blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    blitImageInfo.srcImage = in_srcImage;
    blitImageInfo.srcImageLayout = in_srcImageLayout;
    blitImageInfo.dstImage = in_dstImage;
    blitImageInfo.dstImageLayout = in_dstImageLayout;
    blitImageInfo.regionCount = in_regionCount;
    blitImageInfo.pRegions = in_regions;
    blitImageInfo.filter = in_filter;
    blitImageInfo.pNext = in_next;
    vkCmdBlitImage2( m_commandBuffer, &blitImageInfo );
}

/*
==============================================
crvkCommandBuffer::CopyBufferToImage
==============================================
*/
void crvkCommandBuffer::CopyBufferToImage(  const VkBuffer in_srcBuffer, 
                                            const VkImage in_dstImage, 
                                            const VkImageLayout in_dstImageLayout, 
                                            const uint32_t in_regionCount, 
                                            const VkBufferImageCopy2 *in_regions,
                                            const void* in_next ) const
{
    VkCopyBufferToImageInfo2 copyBufferToImageInfo{};
    copyBufferToImageInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferToImageInfo.srcBuffer = in_srcBuffer;
    copyBufferToImageInfo.dstImage = in_dstImage;
    copyBufferToImageInfo.dstImageLayout = in_dstImageLayout;
    copyBufferToImageInfo.regionCount = in_regionCount;
    copyBufferToImageInfo.pRegions = in_regions;
    copyBufferToImageInfo.pNext = in_next;
    vkCmdCopyBufferToImage2( m_commandBuffer, &copyBufferToImageInfo );
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
                                            const void* in_next ) const
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
void crvkCommandBuffer::UpdateBuffer(   const VkBuffer in_dstBuffer, 
                                        const VkDeviceSize in_dstOffset, 
                                        const VkDeviceSize in_dataSize, 
                                        const void *in_data ) const
{
    vkCmdUpdateBuffer( m_commandBuffer, in_dstBuffer, in_dstOffset, in_dataSize, in_data );
}

/*
==============================================
crvkCommandBuffer::FillBuffer
==============================================
*/
void crvkCommandBuffer::FillBuffer( const VkBuffer in_dstBuffer, 
                                    const VkDeviceSize in_dstOffset, 
                                    const VkDeviceSize in_size, 
                                    const uint32_t in_data ) const
{
    vkCmdFillBuffer( m_commandBuffer, in_dstBuffer, in_dstOffset, in_size, in_data );
}

/*
==============================================
crvkCommandBuffer::ClearColorImage
==============================================
*/
void crvkCommandBuffer::ClearColorImage( const VkImage in_image, 
                                         const VkImageLayout in_imageLayout, 
                                         const VkClearColorValue *in_color, 
                                         const uint32_t in_rangeCount, 
                                         const VkImageSubresourceRange *in_ranges ) const
{
    vkCmdClearColorImage( m_commandBuffer, in_image, in_imageLayout, in_color, in_rangeCount, in_ranges );
}

/*
==============================================
crvkCommandBuffer::ClearDepthStencilImage
==============================================
*/
void crvkCommandBuffer::ClearDepthStencilImage( const VkImage in_image, 
                                                const VkImageLayout in_imageLayout, 
                                                const VkClearDepthStencilValue *in_depthStencil, 
                                                const uint32_t in_rangeCount, 
                                                const VkImageSubresourceRange *in_ranges ) const
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
                                        const void* in_next ) const
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
void crvkCommandBuffer::ExecuteCommands( const uint32_t in_commandBufferCount, const VkCommandBuffer *in_commandBuffers) const
{
    vkCmdExecuteCommands( m_commandBuffer, in_commandBufferCount, in_commandBuffers );
}

/*
==============================================
crvkCommandBuffer::ClearAttachments
==============================================
*/
void crvkCommandBuffer::ClearAttachments(   const uint32_t in_attachmentCount, 
                                            const VkClearAttachment *in_attachments, 
                                            const uint32_t in_rectCount, 
                                            const VkClearRect *in_rects ) const
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
                                    const VkEvent* in_events, 
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

// ===================================================================================================================================================
crvkCommandBufferRoundRobin::crvkCommandBufferRoundRobin( void ) :
    m_numBuffers( 0 ),
    m_currentBuffer( 0 ),
    m_timelineValue( 0 ),
    m_commandBuffers( 0 ),
    m_doneSemaphore( 0 ),
    m_device( 0 )
{
}

crvkCommandBufferRoundRobin::~crvkCommandBufferRoundRobin( void )
{
}

bool crvkCommandBufferRoundRobin::Create( const crvkDevice *in_device, const crvkDeviceQueue *in_queue, const uint32_t in_bufferCount )
{
    VkResult result = VK_SUCCESS;
    m_device = const_cast<crvkDevice*>( in_device );
    m_queue = const_cast<crvkDeviceQueue*>( in_queue );
    m_numBuffers = in_bufferCount;

    VkCommandBufferAllocateInfo commandBufferAL{};
    commandBufferAL.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAL.pNext = nullptr;
    commandBufferAL.commandPool = m_queue->CommandPool();
    commandBufferAL.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAL.commandBufferCount = m_numBuffers;
    result = vkAllocateCommandBuffers( m_device->Device(), &commandBufferAL, m_commandBuffers );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkCommandBufferRoundRobin::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    ///
    /// Create semaphores 
    /// ==========================================================================
    VkSemaphoreTypeCreateInfo timelineCreateInfo{};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = 0;
    semaphoreCI.pNext = &timelineCreateInfo;
    result = vkCreateSemaphore( m_device->Device(), &semaphoreCI, k_allocationCallbacks, &m_doneSemaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkCommandBufferRoundRobin::Create::vkCreateSemaphore::COPY", result );
        return false;
    }

    return true;
}

void crvkCommandBufferRoundRobin::Destroy( void )
{
    if( m_doneSemaphore != nullptr )
    {
        vkDestroySemaphore( m_device->Device(), m_doneSemaphore, k_allocationCallbacks );
        m_doneSemaphore = nullptr;
    }

    if ( m_commandBuffers != nullptr )
    {
        vkFreeCommandBuffers( m_device->Device(), m_queue->CommandPool(), m_numBuffers, m_commandBuffers );
        m_commandBuffers = nullptr;
    }

    m_queue = nullptr;
    m_device = nullptr;
}

bool crvkCommandBufferRoundRobin::Begin( const VkCommandBufferResetFlags in_resetFlags, const VkCommandBufferUsageFlags in_usageFlags, const bool in_waitFinish ) const
{
    VkResult result = VK_SUCCESS;

    // wait we finish last frame before begin this
    if ( in_waitFinish )
    {
        VkSemaphoreWaitInfo waitInfo{};
        waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        waitInfo.pNext = nullptr;
        waitInfo.flags = 0; // 0 = espera qualquer um (ALL), pode ser VK_SEMAPHORE_WAIT_ANY_BIT
        waitInfo.semaphoreCount = 1;
        waitInfo.pSemaphores = &m_doneSemaphore,
        waitInfo.pValues = &m_timelineValue;

        // Espera até o semáforo atingir waitValue ou mais
        result = vkWaitSemaphores( m_device->Device(), &waitInfo, UINT64_MAX ); // UINT64_MAX = espera infinita
        //if (result == VK_SUCCESS) 
        //{
        //    printf("Timeline atingiu o valor %llu\n", waitValue);
        //} 
        //if (result == VK_TIMEOUT) 
        //{
        //    printf("Timeout!\n");
        //}
    }

    result = vkResetCommandBuffer( m_commandBuffers[m_currentBuffer], in_resetFlags );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkCommandBufferRoundRobin::Begin::vkResetCommandBuffer", result );
        return false;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = in_usageFlags;
    result = vkBeginCommandBuffer( m_commandBuffers[m_currentBuffer], &beginInfo );
}

bool crvkCommandBufferRoundRobin::End( 
    const VkSemaphoreSubmitInfo* in_waitInfo,
    const uint32_t in_waitInfoCount,
    const VkSemaphoreSubmitInfo* in_singalInfo,
    const uint32_t in_singalInfoCount,
    const VkFence in_fence, 
    const bool in_waitFinish )
{
    VkResult result = VK_SUCCESS;
    uint64_t waitValue = m_timelineValue++; // last wait value 

    crvkDynamicVector<VkSemaphoreSubmitInfo> waitArray;
    if( in_waitInfoCount > 0 )
    {
        waitArray.Resize( in_waitInfoCount );
        std::memcpy( &waitArray, in_waitInfo, sizeof( VkSemaphoreSubmitInfo ) *  in_waitInfoCount );
    }

    crvkDynamicVector<VkSemaphoreSubmitInfo> signalArray;
    if( in_singalInfoCount > 0 )
    {
        signalArray.Resize( in_singalInfoCount );
        std::memcpy( &signalArray, in_singalInfo, sizeof( VkSemaphoreSubmitInfo ) *  in_singalInfoCount );
    }

    // end register commands
    result = vkEndCommandBuffer( m_commandBuffers[m_numBuffers] );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError("crvkCommandBufferRoundRobin::End::vkEndCommandBuffer", result ); 
        return false;
    }
    
    // Wait por semaphore
    VkTimelineSemaphoreSubmitInfo timeWait{};
    timeWait.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timeWait.waitSemaphoreValueCount = 0;
    timeWait.pWaitSemaphoreValues = nullptr;
    timeWait.signalSemaphoreValueCount = 1;
    timeWait.pSignalSemaphoreValues = &waitValue;
    
    VkSemaphoreSubmitInfo wait{};
    wait.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait.pNext = &timeWait;
    waitArray.Append( wait );
    
    // signal Semaphore
    VkTimelineSemaphoreSubmitInfo timeSignal{};
    timeSignal.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timeSignal.waitSemaphoreValueCount = 0;
    timeSignal.pWaitSemaphoreValues = nullptr;
    timeSignal.signalSemaphoreValueCount = 1;
    timeSignal.pSignalSemaphoreValues = &m_timelineValue;

    VkSemaphoreSubmitInfo signal{};
    signal.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal.pNext = &timeSignal;
    signalArray.Append( signal );

    //
    VkCommandBufferSubmitInfo commandBufferSubmit{};
    commandBufferSubmit.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmit.pNext = nullptr;
    commandBufferSubmit.commandBuffer = m_commandBuffers[m_currentBuffer];
    commandBufferSubmit.deviceMask = 0;

    //
    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.waitSemaphoreInfoCount = waitArray.Count();
    submitInfo.pWaitSemaphoreInfos = &waitArray;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferSubmit;
    submitInfo.signalSemaphoreInfoCount = signalArray.Count();
    submitInfo.pSignalSemaphoreInfos = &signalArray;
    result = vkQueueSubmit2( m_queue->Queue(), 1, &submitInfo, in_fence );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError("crvkCommandBufferRoundRobin::End::vkEndCommandBuffer", result ); 
        return false;
    }

    return true;
}