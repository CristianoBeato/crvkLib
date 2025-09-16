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

typedef struct crvkCommandBufferHandler_t
{
    uint32_t            count = 0;
    uint32_t            current = 0;
    VkDevice            device = nullptr;
    VkQueue             queues = nullptr;
    VkCommandPool       commandPool = nullptr;
    VkFence*            fences = nullptr;
    VkSemaphore*        semaphores = nullptr;
    VkCommandBuffer*    commandBuffers = nullptr;
} crvkCommandBufferHandler_t;

/*
==============================================
crvkCommandBuffer::crvkCommandBuffer
==============================================
*/
crvkCommandBuffer::crvkCommandBuffer( void ) : m_handler( nullptr )
{
}

/*
==============================================
crvkCommandBuffer::~crvkCommandBuffer
==============================================
*/
crvkCommandBuffer::~crvkCommandBuffer( void )
{
    Destroy();
}

/*
==============================================
crvkCommandBuffer::Create
==============================================
*/
bool crvkCommandBuffer::Create( const crvkDevice* in_device, const crvkDeviceQueue* in_queue, const uint32_t in_count, const VkCommandBufferLevel in_level, const bool in_createFences )
{
    VkResult result = VK_SUCCESS;

    //
    if( m_handler != nullptr )
        Destroy();

    // alloc handler structure
    m_handler = static_cast<crvkCommandBufferHandler_t*>( SDL_malloc( sizeof( crvkCommandBufferHandler_t ) ) );
    m_handler->device = in_device->Device();
    m_handler->commandPool = in_queue->CommandPool();
    m_handler->queues = in_queue->Queue();
    m_handler->count = in_count;
    m_handler->current = 0;

    // allocate command buffers
    VkCommandBufferAllocateInfo commandBufferAllocateCI{};
    commandBufferAllocateCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateCI.pNext = nullptr;
    commandBufferAllocateCI.commandPool = m_handler->commandPool;
    commandBufferAllocateCI.level = in_level;
    commandBufferAllocateCI.commandBufferCount = m_handler->count;
    result = vkAllocateCommandBuffers( m_handler->device, &commandBufferAllocateCI, m_handler->commandBuffers );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkCommandBuffer::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    if ( in_createFences )
    {
        // TODO:
    }
    

    return true;
}

/*
==============================================
crvkCommandBuffer::Destroy
==============================================
*/
void crvkCommandBuffer::Destroy( void )
{
    if( m_handler == nullptr )
        return;

    // destroy fences
    if ( m_handler->fences != nullptr )
    {
        // release fences
        for ( uint32_t i = 0; i < m_handler->count; i++)
        {
            vkDestroyFence( m_handler->device, m_handler->fences[i], k_allocationCallbacks );
        }
        
        SDL_free( m_handler->fences );
        m_handler->fences = nullptr;
    }
    
    // relase command buffers array     
    if( m_handler->commandBuffers != nullptr )
    {
        vkFreeCommandBuffers( m_handler->device, m_handler->commandPool, m_handler->count, m_handler->commandBuffers );
        m_handler->commandBuffers = nullptr;
    }
    
    m_handler->count = 0;
    m_handler->device = nullptr;
    m_handler->commandPool = nullptr;

    SDL_free( m_handler );
    m_handler = nullptr;
}

/*
==============================================
crvkCommandBuffer::SelectBuffer
==============================================
*/
void crvkCommandBuffer::SelectCurrentBuffer(const uint32_t in_index)
{
    if ( m_handler == nullptr )
        return;

    // current active
    m_handler->current = std::min( m_handler->count, in_index );
}

/*
==============================================
crvkCommandBuffer::Reset
==============================================
*/
VkResult crvkCommandBuffer::Reset( const VkCommandBufferResetFlags in_flags ) const
{
    return vkResetCommandBuffer( m_handler->commandBuffers[m_handler->current], in_flags );
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
    return vkBeginCommandBuffer( m_handler->commandBuffers[m_handler->current], &beginInfo );   
}

/*
==============================================
crvkCommandBuffer::End
==============================================
*/
VkResult crvkCommandBuffer::End( void ) const
{
    return vkEndCommandBuffer( m_handler->commandBuffers[m_handler->current] );
}

/*
==============================================
crvkCommandBuffer::GetCommandBuffer
==============================================
*/
VkCommandBuffer crvkCommandBuffer::GetCommandBuffer( const uint32_t in_index ) const
{
    if( m_handler == nullptr )
        return nullptr;

    return m_handler->commandBuffers[std::min( m_handler->count, in_index )];
}

/*
==============================================
crvkCommandBuffer::GetCurrentCommandBuffer
==============================================
*/
VkCommandBuffer crvkCommandBuffer::GetCurrentCommandBuffer( void ) const
{
 if( m_handler == nullptr )
        return nullptr;

    return m_handler->commandBuffers[m_handler->current];
}

/*
==============================================
crvkCommandBuffer::GetCommandBufferArray
==============================================
*/
VkCommandBuffer* crvkCommandBuffer::GetCommandBufferArray( void ) const
{
    if( m_handler == nullptr )
        return nullptr;

    return m_handler->commandBuffers;
}

/*
==============================================
crvkCommandBuffer::Submit
==============================================
*/
VkResult crvkCommandBuffer::Submit( const VkSemaphoreSubmitInfo* in_semaphoresToWait, 
                                    const uint32_t in_semaphoresToWaitCount, 
                                    const VkSemaphoreSubmitInfo* in_semaphoresToSignal, 
                                    const uint32_t in_semaphoresToSingalCount )
{
    uint32_t index = 0;
    
    // invalid command buffer
    if( m_handler == nullptr && m_handler->commandBuffers == nullptr )
        return VK_INCOMPLETE;
    
    VkCommandBufferSubmitInfo commandBufferSI{};
    commandBufferSI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSI.pNext = nullptr;
    commandBufferSI.commandBuffer = m_handler->commandBuffers[index];
    commandBufferSI.deviceMask = 0;

    // submit 
    VkSubmitInfo2 submitInfo{}; 
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;;
    submitInfo.pNext = nullptr;
    submitInfo.flags = 0;
    submitInfo.waitSemaphoreInfoCount = in_semaphoresToWaitCount;
    submitInfo.pWaitSemaphoreInfos = in_semaphoresToWait;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferSI;
    submitInfo.signalSemaphoreInfoCount = in_semaphoresToSingalCount;
    submitInfo.pSignalSemaphoreInfos = in_semaphoresToSignal;

    return vkQueueSubmit2( m_handler->queues, 1, &submitInfo, ( m_handler->fences != nullptr ) ? m_handler->fences[index] : nullptr );
}

/*
==============================================
crvkCommandBuffer::Execute
==============================================
*/
void crvkCommandBuffer::Execute( const uint32_t in_commandBufferCount, const VkCommandBuffer* in_commandBuffers ) const
{
    vkCmdExecuteCommands( m_handler->commandBuffers[m_handler->current], in_commandBufferCount, in_commandBuffers );
}

/*
==============================================
crvkCommandBuffer::SetViewport
==============================================
*/
void crvkCommandBuffer::SetViewport( const uint32_t in_firstViewport, const uint32_t in_viewportCount, const VkViewport *in_viewports ) const
{
    vkCmdSetViewport( m_handler->commandBuffers[m_handler->current], in_firstViewport, in_viewportCount, in_viewports );
}

/*
==============================================
crvkCommandBuffer::SetScissor
==============================================
*/
void crvkCommandBuffer::SetScissor( const uint32_t in_firstScissor, const uint32_t in_scissorCount, const VkRect2D *in_scissors ) const
{
    vkCmdSetScissor( m_handler->commandBuffers[m_handler->current], in_firstScissor, in_scissorCount, in_scissors );
}

/*
==============================================
crvkCommandBuffer::SetLineWidth
==============================================
*/
void crvkCommandBuffer::SetLineWidth( const float in_lineWidth ) const
{
    vkCmdSetLineWidth( m_handler->commandBuffers[m_handler->current], in_lineWidth );
}

/*
==============================================
crvkCommandBuffer::SetDepthBias
==============================================
*/
void crvkCommandBuffer::SetDepthBias( const float in_depthBiasConstantFactor, const float in_depthBiasClamp, const float in_depthBiasSlopeFactor ) const
{
    vkCmdSetDepthBias( m_handler->commandBuffers[m_handler->current], in_depthBiasConstantFactor, in_depthBiasClamp, in_depthBiasSlopeFactor );
}

/*
==============================================
crvkCommandBuffer::SetBlendConstants
==============================================
*/
void crvkCommandBuffer::SetBlendConstants(const float in_blendConstants[4]) const
{
    vkCmdSetBlendConstants( m_handler->commandBuffers[m_handler->current], in_blendConstants );
}

/*
==============================================
crvkCommandBuffer::SetBlendConstants
==============================================
*/
void crvkCommandBuffer::SetDepthBounds( const float in_minDepthBounds, const float in_maxDepthBounds ) const
{
    vkCmdSetDepthBounds( m_handler->commandBuffers[m_handler->current], in_minDepthBounds, in_maxDepthBounds );
}

void crvkCommandBuffer::SetCullMode( const VkCullModeFlags in_cullMode ) const
{
    vkCmdSetCullMode( m_handler->commandBuffers[m_handler->current], in_cullMode );
}

/*
==============================================
crvkCommandBuffer::SetStencilCompareMask
==============================================
*/
void crvkCommandBuffer::SetStencilCompareMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_compareMask ) const
{
    vkCmdSetStencilCompareMask( m_handler->commandBuffers[m_handler->current], in_faceMask, in_compareMask );
}

/*
==============================================
crvkCommandBuffer::SetStencilWriteMask
==============================================
*/
void crvkCommandBuffer::SetStencilWriteMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_writeMask ) const
{
    vkCmdSetStencilWriteMask( m_handler->commandBuffers[m_handler->current], in_faceMask, in_writeMask );
}

/*
==============================================
crvkCommandBuffer::SetStencilReference
==============================================
*/
void crvkCommandBuffer::SetStencilReference( const VkStencilFaceFlags in_faceMask, const uint32_t in_reference ) const
{
    vkCmdSetStencilReference( m_handler->commandBuffers[m_handler->current], in_faceMask, in_reference );
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
    vkCmdBindDescriptorSets( m_handler->commandBuffers[m_handler->current], in_pipelineBindPoint, in_layout, in_firstSet, in_descriptorSetCount, in_descriptorSets, in_dynamicOffsetCount, in_dynamicOffsets );
}

/*
==============================================
crvkCommandBuffer::BindIndexBuffer
==============================================
*/
void crvkCommandBuffer::BindIndexBuffer( const VkBuffer in_buffer, const VkDeviceSize in_offset, const VkIndexType in_indexType ) const
{
    vkCmdBindIndexBuffer( m_handler->commandBuffers[m_handler->current], in_buffer, in_offset, in_indexType );
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


    vkCmdBindVertexBuffers2( m_handler->commandBuffers[m_handler->current], in_firstBinding, in_bindingCount, in_buffers, in_offsets, in_sizes, in_strides );
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
    vkCmdDraw( m_handler->commandBuffers[m_handler->current], in_vertexCount, in_instanceCount, in_firstVertex, in_firstInstance );
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
    vkCmdDrawIndexed( m_handler->commandBuffers[m_handler->current], in_indexCount, in_instanceCount, in_firstIndex, in_vertexOffset, in_firstInstance );
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
    vkCmdDrawIndirect( m_handler->commandBuffers[m_handler->current], in_buffer , in_offset, in_drawCount, in_stride );
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
    vkCmdDrawIndexedIndirect( m_handler->commandBuffers[m_handler->current], in_buffer, in_offset, in_drawCount, in_stride );
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
    vkCmdDispatch( m_handler->commandBuffers[m_handler->current], in_groupCountX, in_groupCountY, in_groupCountZ );
}

/*
==============================================
crvkCommandBuffer::DispatchIndirect
==============================================
*/
void crvkCommandBuffer::DispatchIndirect(   const VkBuffer in_buffer, 
                                            const VkDeviceSize in_offset )  const
{
    vkCmdDispatchIndirect( m_handler->commandBuffers[m_handler->current], in_buffer, in_offset );
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
    vkCmdCopyBuffer2(m_handler->commandBuffers[m_handler->current], &copyBufferInfo );
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
    vkCmdCopyImage2( m_handler->commandBuffers[m_handler->current], &copyImageInfo );   
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
    vkCmdBlitImage2( m_handler->commandBuffers[m_handler->current], &blitImageInfo );
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
    vkCmdCopyBufferToImage2( m_handler->commandBuffers[m_handler->current], &copyBufferToImageInfo );
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
    vkCmdCopyImageToBuffer2( m_handler->commandBuffers[m_handler->current], &copyImageToBufferInfo );
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
    vkCmdUpdateBuffer( m_handler->commandBuffers[m_handler->current], in_dstBuffer, in_dstOffset, in_dataSize, in_data );
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
    vkCmdFillBuffer( m_handler->commandBuffers[m_handler->current], in_dstBuffer, in_dstOffset, in_size, in_data );
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
    vkCmdClearColorImage( m_handler->commandBuffers[m_handler->current], in_image, in_imageLayout, in_color, in_rangeCount, in_ranges );
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
    vkCmdClearDepthStencilImage( m_handler->commandBuffers[m_handler->current], in_image, in_imageLayout, in_depthStencil, in_rangeCount, in_ranges );
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
    vkCmdResolveImage2( m_handler->commandBuffers[m_handler->current], &resolveImageInfo );
}

/*
==============================================
crvkCommandBuffer::ExecuteCommands
==============================================
*/
void crvkCommandBuffer::ExecuteCommands( const uint32_t in_commandBufferCount, const VkCommandBuffer *in_commandBuffers) const
{
    vkCmdExecuteCommands( m_handler->commandBuffers[m_handler->current], in_commandBufferCount, in_commandBuffers );
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
    vkCmdClearAttachments( m_handler->commandBuffers[m_handler->current], in_attachmentCount, in_attachments, in_rectCount, in_rects );
}

/*
==============================================
crvkCommandBuffer::BeginRenderPass
==============================================
*/
void crvkCommandBuffer::BeginRenderPass( const VkRenderPassBeginInfo* in_renderPassBegin, const VkSubpassBeginInfo* in_subpassBeginInfo ) const
{

    vkCmdBeginRenderPass2( m_handler->commandBuffers[m_handler->current], in_renderPassBegin, in_subpassBeginInfo );
}

/*
==============================================
crvkCommandBuffer::EndRenderPass
==============================================
*/
void crvkCommandBuffer::EndRenderPass( const VkSubpassEndInfo* in_subpassEndInfo ) const
{
    vkCmdEndRenderPass2( m_handler->commandBuffers[m_handler->current], in_subpassEndInfo );
}

/*
==============================================
crvkCommandBuffer::BindPipeline
==============================================
*/
void crvkCommandBuffer::BindPipeline( const VkPipelineBindPoint in_pipelineBindPoint, const VkPipeline in_pipeline ) const
{
    vkCmdBindPipeline( m_handler->commandBuffers[m_handler->current], in_pipelineBindPoint, in_pipeline );
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
    vkCmdPipelineBarrier2( m_handler->commandBuffers[m_handler->current], &dependencyInfo );
}

/*
==============================================
crvkCommandBuffer::SetEvent
==============================================
*/
void crvkCommandBuffer::SetEvent(const VkEvent in_event, const VkDependencyFlags in_dependencyFlags, const uint32_t in_memoryBarrierCount, const VkMemoryBarrier2 *in_memoryBarriers, const uint32_t in_bufferMemoryBarrierCount, const VkBufferMemoryBarrier2 *in_bufferMemoryBarriers, const uint32_t in_imageMemoryBarrierCount, const VkImageMemoryBarrier2 *in_imageMemoryBarriers, const void *in_next) const
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
    vkCmdSetEvent2( m_handler->commandBuffers[m_handler->current], in_event, &dependencyInfo );
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
    vkCmdWaitEvents2( m_handler->commandBuffers[m_handler->current], in_eventCount, in_events, &dependencyInfo );
}

/*
==============================================
crvkCommandBuffer::ResetEvent
==============================================
*/
void crvkCommandBuffer::ResetEvent(const VkEvent in_event, const VkPipelineStageFlags2 in_stageMask)
{
    vkCmdResetEvent2( m_handler->commandBuffers[m_handler->current], in_event, in_stageMask );
}

/*
==============================================
crvkCommandBuffer::PushConstants
==============================================
*/
void crvkCommandBuffer::PushConstants( const VkPushConstantsInfo* in_pushConstantsInfo ) const
{
    vkCmdPushConstants2( m_handler->commandBuffers[m_handler->current], in_pushConstantsInfo );
}

/*
==============================================
crvkCommandBuffer::CopyQueryPoolResults
==============================================
*/
void crvkCommandBuffer::CopyQueryPoolResults( const VkQueryPool in_queryPool, const uint32_t in_firstQuery, const uint32_t in_queryCount, const VkBuffer in_dstBuffer, const VkDeviceSize in_dstOffset, const VkDeviceSize in_stride, const VkQueryResultFlags in_flags ) const
{
    vkCmdCopyQueryPoolResults( m_handler->commandBuffers[m_handler->current], in_queryPool, in_firstQuery, in_queryCount, in_dstBuffer, in_dstOffset, in_stride, in_flags );
}

/*
==============================================
crvkCommandBuffer::WriteTimestamp
==============================================
*/
void crvkCommandBuffer::WriteTimestamp( const VkPipelineStageFlagBits in_pipelineStage, const VkQueryPool in_queryPool, const uint32_t in_query ) const
{
    vkCmdWriteTimestamp( m_handler->commandBuffers[m_handler->current], in_pipelineStage, in_queryPool, in_query );
}

/*
==============================================
crvkCommandBuffer::BeginQuery
==============================================
*/
void crvkCommandBuffer::BeginQuery( const VkQueryPool in_queryPool, const uint32_t in_query, const VkQueryControlFlags in_flags ) const
{
    vkCmdBeginQuery( m_handler->commandBuffers[m_handler->current], in_queryPool, in_query, in_flags );
}

/*
==============================================
crvkCommandBuffer::ResetQueryPool
==============================================
*/
void crvkCommandBuffer::ResetQueryPool( const VkQueryPool in_queryPool, const uint32_t in_firstQuery, const uint32_t in_queryCount ) const
{
    vkCmdResetQueryPool( m_handler->commandBuffers[m_handler->current], in_queryPool, in_firstQuery, in_queryCount ); 
}

/*
==============================================
crvkCommandBuffer::EndQuery
==============================================
*/
void crvkCommandBuffer::EndQuery( const VkQueryPool in_queryPool, const uint32_t in_query ) const
{
    vkCmdEndQuery( m_handler->commandBuffers[m_handler->current], in_queryPool,  in_query );
}
