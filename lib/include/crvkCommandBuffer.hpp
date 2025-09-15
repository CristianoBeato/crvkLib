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

#ifndef __CRVK_COMMAND_BUFFER_HPP__
#define __CRVK_COMMAND_BUFFER_HPP__

typedef struct crvkCommandBufferHandler_t;

/// @brief command buffer array control structure
class crvkCommandBuffer
{
public:
    crvkCommandBuffer( void );
    ~crvkCommandBuffer( void );

    bool                Create( const crvkDevice* in_device, const uint32_t in_count, const VkCommandBufferLevel in_leve, const bool in_useFences = false );
    void                Destroy( void );
    // current active buffer
    void                SelectCurrentBuffer( const uint32_t in_index );
    VkCommandBuffer     GetCommandBuffer( const uint32_t in_index ) const;
    VkCommandBuffer     GetCurrentCommandBuffer( void ) const;
    VkCommandBuffer*    GetCommandBufferArray( void ) const;
    VkResult            Reset( const VkCommandBufferResetFlags in_flags ) const;
    VkResult            Begin( const VkCommandBufferUsageFlags in_flags ) const;
    VkResult            End( void ) const;
    VkResult            Submit(     const VkSemaphoreSubmitInfo* in_semaforesToWait, 
                                    const uint32_t in_semaforesToWaitCount, 
                                    const VkSemaphoreSubmitInfo* in_semaforesToSignal, 
                                    const uint32_t in_semaforesToSingalCount );

    void Execute( const uint32_t in_commandBufferCount, const VkCommandBuffer* in_commandBuffers ) const;

    void SetViewport( const uint32_t in_firstViewport, const uint32_t in_viewportCount, const VkViewport* in_viewports ) const;

    void SetScissor( const uint32_t in_firstScissor, const uint32_t in_scissorCount, const VkRect2D* in_scissors ) const;

    void SetLineWidth( const float in_lineWidth ) const;

    void SetDepthBias( const float in_depthBiasConstantFactor, const float in_depthBiasClamp, const float in_depthBiasSlopeFactor ) const;

    void SetBlendConstants( const float in_blendConstants[4] ) const;

    void SetDepthBounds( const float in_minDepthBounds, const float in_maxDepthBounds ) const;

    void SetCullMode( const VkCullModeFlags in_cullMode ) const;

    void SetStencilCompareMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_compareMask ) const;

    void SetStencilWriteMask( const VkStencilFaceFlags in_faceMask, const uint32_t in_writeMask ) const;

    void SetStencilReference( const VkStencilFaceFlags in_faceMask, const uint32_t in_reference ) const;

    void BindDescriptorSets( const VkPipelineBindPoint in_pipelineBindPoint, 
                             const VkPipelineLayout in_layout, 
                             const uint32_t in_firstSet, 
                             const uint32_t in_descriptorSetCount, 
                             const VkDescriptorSet* in_descriptorSets, 
                             const uint32_t in_dynamicOffsetCount, 
                             const uint32_t* in_dynamicOffsets );

    void BindIndexBuffer(   const VkBuffer in_buffer, 
                            const VkDeviceSize in_offset, 
                            const VkIndexType in_indexType ) const;

    void BindVertexBuffers( const uint32_t in_firstBinding, 
                            const uint32_t in_bindingCount, 
                            const VkBuffer* in_buffers, 
                            const VkDeviceSize* in_offsets, 
                            const VkDeviceSize* in_sizes,
                            const VkDeviceSize* in_strides ) const;

    void Draw(  const uint32_t in_vertexCount, 
                const uint32_t in_instanceCount, 
                const uint32_t in_firstVertex, 
                const uint32_t in_firstInstance ) const;

    void DrawIndexed(   const uint32_t in_indexCount, 
                        const uint32_t in_instanceCount, 
                        const uint32_t in_firstIndex, 
                        const int32_t in_vertexOffset, 
                        const uint32_t in_firstInstance ) const;

    void DrawIndirect(  const VkBuffer in_buffer, 
                        const VkDeviceSize in_offset, 
                        const uint32_t in_drawCount, 
                        const uint32_t in_stride ) const;

    void DrawIndexedIndirect(   const VkBuffer in_buffer, 
                                const VkDeviceSize in_offset, 
                                const uint32_t in_drawCount, 
                                const uint32_t in_stride ) const;

    void Dispatch(  const uint32_t in_groupCountX, 
                    const uint32_t in_groupCountY, 
                    const uint32_t in_groupCountZ ) const ;

    void DispatchIndirect( const VkBuffer in_buffer, const VkDeviceSize in_offset ) const;

    void CopyBuffer(    const VkBuffer in_srcBuffer, 
                        const VkBuffer in_dstBuffer, 
                        const uint32_t in_regionCount, 
                        const VkBufferCopy2* in_regions, 
                        const void* in_next = nullptr ) const;

    void CopyImage( const VkImage in_srcImage, 
                    const VkImageLayout in_srcImageLayout, 
                    const VkImage in_dstImage, 
                    const VkImageLayout in_dstImageLayout, 
                    const uint32_t in_regionCount, 
                    const VkImageCopy2* in_regions,
                    const void* in_next = nullptr ) const;

    void BlitImage( const VkImage in_srcImage, 
                    const VkImageLayout in_srcImageLayout, 
                    const VkImage in_dstImage, 
                    const VkImageLayout in_dstImageLayout, 
                    const uint32_t in_regionCount, 
                    const VkImageBlit2* in_pRegions, 
                    const VkFilter in_filter, 
                    const void* in_next = nullptr ) const;

    void CopyBufferToImage( const VkBuffer in_srcBuffer, 
                            const VkImage in_dstImage, 
                            const VkImageLayout in_dstImageLayout, 
                            const uint32_t in_regionCount, 
                            const VkBufferImageCopy2* in_regions, 
                            const void* in_next = nullptr ) const;

    void CopyImageToBuffer( const VkImage in_srcImage,
                            const VkImageLayout in_srcImageLayout,
                            const VkBuffer in_dstBuffer,
                            const uint32_t in_regionCount,
                            const VkBufferImageCopy2* in_regions,
                            const void* in_next ) const;

    void UpdateBuffer(  const VkBuffer in_dstBuffer, 
                        const VkDeviceSize in_dstOffset, 
                        const VkDeviceSize in_dataSize, 
                        const void* in_data ) const;

    void FillBuffer(    const VkBuffer in_dstBuffer,
                        const VkDeviceSize in_dstOffset,
                        const VkDeviceSize in_size,
                        const uint32_t in_data ) const;
    
    void ClearColorImage(  const VkImage in_image, 
                            const VkImageLayout in_imageLayout, 
                            const VkClearColorValue* in_color, 
                            const uint32_t in_rangeCount, 
                            const VkImageSubresourceRange* in_ranges ) const;

    void ClearDepthStencilImage(    const VkImage in_image,
                                    const VkImageLayout in_imageLayout,
                                    const VkClearDepthStencilValue* in_depthStencil, 
                                    const uint32_t in_rangeCount, 
                                    const VkImageSubresourceRange* in_ranges ) const;

    void ResolveImage(  const VkImage in_srcImage,
                        const VkImageLayout in_srcImageLayout,
                        const VkImage in_dstImage,
                        const VkImageLayout in_dstImageLayout,
                        const uint32_t in_regionCount,
                        const VkImageResolve2* in_regions, 
                        const void* in_next = nullptr ) const;

    void ExecuteCommands( const uint32_t in_commandBufferCount, const VkCommandBuffer* in_commandBuffers ) const;

    void ClearAttachments(  const uint32_t in_attachmentCount, 
                            const VkClearAttachment* in_attachments, 
                            const uint32_t in_rectCount, 
                            const VkClearRect* in_rects ) const;

    /// @brief Bind the render pass and frame buffer to the current execution command
    /// @param in_renderPass 
    /// @param in_framebuffer 
    /// @param in_renderArea 
    /// @param in_clearValueCount 
    /// @param in_clearValues 
    /// @param in_contents 
    void        BeginRenderPass( const VkRenderPassBeginInfo* in_renderPassBegin, const VkSubpassBeginInfo* in_subpassBeginInfo ) const;

    /// @brief 
    /// @param  
    void        EndRenderPass( const VkSubpassEndInfo* in_subpassEndInfo ) const;

    /// @brief 
    /// @param in_pipelineBindPoint 
    /// @param in_pipeline 
    void        BindPipeline( const VkPipelineBindPoint in_pipelineBindPoint, const VkPipeline in_pipeline ) const;

    /// @brief 
    /// @param in_dependencyFlags 
    /// @param in_memoryBarrierCount 
    /// @param in_memoryBarriers 
    /// @param in_bufferMemoryBarrierCount 
    /// @param in_bufferMemoryBarriers 
    /// @param in_imageMemoryBarrierCount 
    /// @param in_imageMemoryBarriers 
    /// @param in_nex 
    void PipelineBarrier(   const VkDependencyFlags in_dependencyFlags, 
                            const uint32_t in_memoryBarrierCount,
                            const VkMemoryBarrier2* in_memoryBarriers,
                            const uint32_t in_bufferMemoryBarrierCount,
                            const VkBufferMemoryBarrier2* in_bufferMemoryBarriers,
                            const uint32_t in_imageMemoryBarrierCount,
                            const VkImageMemoryBarrier2* in_imageMemoryBarriers,
                            const void* in_nex = nullptr ) const;

    void SetEvent( const VkEvent in_event, const VkDependencyFlags in_dependencyFlags,
                                    const uint32_t in_memoryBarrierCount,
                                    const VkMemoryBarrier2* in_memoryBarriers,
                                    const uint32_t in_bufferMemoryBarrierCount,
                                    const VkBufferMemoryBarrier2* in_bufferMemoryBarriers,
                                    const uint32_t in_imageMemoryBarrierCount,
                                    const VkImageMemoryBarrier2* in_imageMemoryBarriers,
                                    const void* in_next ) const;

    void WaitEvents( const uint32_t in_eventCount,
                     const VkEvent* in_events, 
                     const VkDependencyFlags in_dependencyFlags,
                     const uint32_t in_memoryBarrierCount,
                     const VkMemoryBarrier2* in_memoryBarriers,
                     const uint32_t in_bufferMemoryBarrierCount,
                     const VkBufferMemoryBarrier2* in_bufferMemoryBarriers,
                     const uint32_t in_imageMemoryBarrierCount,
                     const VkImageMemoryBarrier2* in_imageMemoryBarriers,
                     const void* in_next ) const;

    void ResetEvent( const VkEvent in_event, const VkPipelineStageFlags2 in_stageMask );    

    void PushConstants( const VkPushConstantsInfo* in_pushConstantsInfo ) const;

    void CopyQueryPoolResults( const VkQueryPool in_queryPool, const uint32_t in_firstQuery, const uint32_t in_queryCount, const VkBuffer in_dstBuffer, const VkDeviceSize in_dstOffset, const VkDeviceSize in_stride, const VkQueryResultFlags in_flags ) const;    

    void WriteTimestamp( const VkPipelineStageFlagBits in_pipelineStage, const VkQueryPool in_queryPool, const uint32_t in_query ) const;    

    void BeginQuery( const VkQueryPool in_queryPool, const uint32_t in_query, const VkQueryControlFlags in_flags ) const;

    void ResetQueryPool( const VkQueryPool in_queryPool, const uint32_t in_firstQuery, const uint32_t in_queryCount ) const;

    void EndQuery( const VkQueryPool in_queryPool, const uint32_t in_query ) const;

    

private:
    crvkCommandBufferHandler_t* m_handler;
};

#endif //__CRVK_COMMAND_BUFFER_HPP__