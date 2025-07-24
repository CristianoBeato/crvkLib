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

class crvkCommandBuffer
{

public:
    crvkCommandBuffer( void );
    ~crvkCommandBuffer( void );

    bool        Create( void );
    void        Destroy( void );
    VkResult    Reset( const VkCommandBufferResetFlags in_flags );
    VkResult    Begin( const VkCommandBufferUsageFlags in_flags );

    /// @brief 
    /// @return 
    VkResult    End( void );

    void Execute( const uint32_t in_commandBufferCount, const VkCommandBuffer* in_commandBuffers );

    void SetViewport( void );
    void SetScissor( void );
    void SetLineWidth( void );
    void SetDepthBias( void );
    void SetBlendConstants( void );
    void SetDepthBounds( void );
    void SetStencilCompareMask( void );
    void SetStencilWriteMask( void );
    void SetStencilReference( void );
    void BindDescriptorSets( void );
    void BindIndexBuffer( void );
    void BindVertexBuffers( void );
    void Draw( void );
    void DrawIndexed( void );
    void DrawIndirect( void );
    void DrawIndexedIndirect( void );
    void Dispatch( void );
    void DispatchIndirect( void );
    void CopyBuffer( void );
    void CopyImage( void );
    void BlitImage( void );
    void CopyBufferToImage( void );
    void CopyImageToBuffer( void );
    void UpdateBuffer( void );
    void FillBuffer( void );
    void ClearColorImage( void );
    void ClearDepthStencilImage( void );
    void ResolveImage( void );
    void ExecuteCommands( void );
    void ClearAttachments( void );
    void PipelineBarrier( void );
    void WaitEvents( void );
    void ResetEvent( void );
    void SetEvent( void );
    void PushConstants( void );
    void CopyQueryPoolResults( void );
    void WriteTimestamp( void );
    void ResetQueryPool( void );
    void BeginQuery( const VkQueryPool in_queryPool, const uint32_t in_query, const VkQueryControlFlags in_flags );
    void EndQuery( void );

    /// @brief Bind the render pass and frame buffer to the current execution command
    /// @param in_renderPass 
    /// @param in_framebuffer 
    /// @param in_renderArea 
    /// @param in_clearValueCount 
    /// @param in_clearValues 
    /// @param in_contents 
    void        BeginRenderPass(
        const VkRenderPass in_renderPass,
        const VkFramebuffer in_framebuffer,
        const VkRect2D in_renderArea,
        const uint32_t in_clearValueCount,
        const VkClearValue* in_clearValues,
        const VkSubpassContents in_contents );

    /// @brief 
    /// @param  
    void        EndRenderPass( void );

    /// @brief 
    /// @param in_pipelineBindPoint 
    /// @param in_pipeline 
    void        BindPipeline( const VkPipelineBindPoint in_pipelineBindPoint, const VkPipeline in_pipeline );


    void        SetViewport();


private:
    VkDevice            m_device;
    VkCommandBuffer     m_commandBuffer;

};


#endif //__CRVK_COMMAND_BUFFER_HPP__