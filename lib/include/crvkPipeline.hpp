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

#ifndef __CRVK_PIPELINE_HPP__
#define __CRVK_PIPELINE_HPP__

/// @brief crvkGraphicPipeline is the most basic pipeline creation framework, 
/// with simple abstraction that allows you to have full control over pipeline management.
class crvkGraphicPipeline
{
public:
    crvkGraphicPipeline( void );
    ~crvkGraphicPipeline( void );

    bool    Create(         
        const crvkDevice* in_device,
        const uint32_t in_subpass,
        const uint32_t in_setLayoutCount,
        const VkDescriptorSetLayout* in_setLayouts,
        const uint32_t in_pushConstantRangeCount,  
        const VkPushConstantRange* in_pushConstantRanges,
        const uint32_t in_stageCount,
        const VkPipelineShaderStageCreateInfo* in_stagesCreateInfo,
        const VkPipelineVertexInputStateCreateInfo* in_vertexInputStateCreateInfo,
        const VkPipelineInputAssemblyStateCreateInfo* in_inputAssemblyStateCreateInfo,
        const VkPipelineTessellationStateCreateInfo* in_tessellationStateCreateInfo,
        const VkPipelineViewportStateCreateInfo* in_viewportStateCreateInfo,
        const VkPipelineRasterizationStateCreateInfo* in_rasterizationStateCreateInfo,
        const VkPipelineMultisampleStateCreateInfo* in_multisampleStateCreateInfo,
        const VkPipelineDepthStencilStateCreateInfo* in_depthStencilStateCreateInfo,
        const VkPipelineColorBlendStateCreateInfo* in_colorBlendStateCreateInfo,
        const VkPipelineDynamicStateCreateInfo* in_dynamicStateCreateInfo,
        const VkRenderPass in_renderPass,
        const VkPipeline in_basePipelineHandle );

    void    Destroy( void );

    VkPipeline  Pipeline( void ) const { return m_pipeline; }
    
protected:
    crvkDevice*                         m_device;
    VkPipelineLayout                    m_pipelineLayout;
    VkPipeline                          m_pipeline;
};

/// @brief crvkPipelineCommand is a complete pipeline structure, with its own buffers and command pools,
/// semaphores organized and structured in order to register and execute in the simplest and most functional way possible.
class crvkGraphicPipelineExecutor : public crvkGraphicPipeline
{
public:
    crvkGraphicPipelineExecutor( void );
    ~crvkGraphicPipelineExecutor( void );

    /// @brief Create the pipeline and comand record buffer 
    /// @param in_device 
    /// @param in_frames 
    /// @param in_subpass 
    /// @param in_flags 
    /// @param in_stageCount 
    /// @param in_stagesCreateInfo 
    /// @param in_vertexInputStateCreateInfo 
    /// @param in_inputAssemblyStateCreateInfo 
    /// @param in_tessellationStateCreateInfo 
    /// @param in_viewportStateCreateInfo 
    /// @param in_rasterizationStateCreateInfo 
    /// @param in_multisampleStateCreateInfo 
    /// @param in_depthStencilStateCreateInfo 
    /// @param in_colorBlendStateCreateInfo 
    /// @param in_dynamicStateCreateInfo 
    /// @param in_renderPass 
    /// @param in_basePipelineHandle 
    /// @return 
    bool    Create(
        const crvkDevice* in_device, 
        const uint32_t in_frames, 
        const uint32_t in_subpass,
        const uint32_t in_setLayoutCount,
        const VkDescriptorSetLayout* in_setLayouts,
        const uint32_t in_pushConstantRangeCount,  
        const VkPushConstantRange* in_pushConstantRanges,
        const uint32_t in_stageCount,
        const VkPipelineShaderStageCreateInfo* in_stagesCreateInfo,
        const VkPipelineVertexInputStateCreateInfo* in_vertexInputStateCreateInfo,
        const VkPipelineInputAssemblyStateCreateInfo* in_inputAssemblyStateCreateInfo,
        const VkPipelineTessellationStateCreateInfo* in_tessellationStateCreateInfo,
        const VkPipelineViewportStateCreateInfo* in_viewportStateCreateInfo,
        const VkPipelineRasterizationStateCreateInfo* in_rasterizationStateCreateInfo,
        const VkPipelineMultisampleStateCreateInfo* in_multisampleStateCreateInfo,
        const VkPipelineDepthStencilStateCreateInfo* in_depthStencilStateCreateInfo,
        const VkPipelineColorBlendStateCreateInfo* in_colorBlendStateCreateInfo,
        const VkPipelineDynamicStateCreateInfo* in_dynamicStateCreateInfo,
        const VkRenderPass in_renderPass,
        const VkPipeline in_basePipelineHandle );

    /// @brief release the command buffer and destroy the pipeline 
    /// @param  
    void    Destroy( void );

    /// @brief begin record pipeline render commands 
    /// @param in_frame 
    /// @param in_renderPass 
    /// @param in_frameBuffer 
    /// @return 
    VkResult    Begin( const uint32_t in_frame, const VkRenderPass in_renderPass, const VkFramebuffer in_frameBuffer );
    
    /// @brief finish record draw commands
    /// @param  
    /// @return 
    VkResult    End( void );

    /// @brief 
    /// @param in_indexBuffer 
    /// @param in_offset 
    void        BindIndexBuffer( const VkBuffer in_indexBuffer, const VkDeviceSize in_offset, const VkIndexType in_indexType );
    
    /// @brief 
    /// @param in_vertexBuffers 
    /// @param in_offsets 
    /// @param in_sizes 
    /// @param in_strides 
    /// @param in_count 
    void        BindVertexBuffers( const VkBuffer* in_vertexBuffers, const VkDeviceSize* in_offsets, const VkDeviceSize* in_sizes, const VkDeviceSize* in_strides, const uint32_t in_base, const uint32_t in_count );
    
    /// @brief 
    /// @param in_instanceCount 
    /// @param in_firstInstance 
    /// @param in_count 
    /// @param in_first 
    /// @param in_vertexOffset 
    /// @param in_indexed 
    void        Draw( const uint32_t in_instanceCount, const uint32_t in_firstInstance, const uint32_t in_count, const uint32_t in_first, const int32_t in_vertexOffset, const bool in_indexed );
    
    /// @brief 
    /// @param in_indirectBuffer 
    /// @param in_offset 
    /// @param in_drawCount 
    /// @param in_stride 
    /// @param in_indexed 
    void        DrawIndirect( VkBuffer in_indirectBuffer, VkDeviceSize in_offset, uint32_t in_drawCount, uint32_t in_stride, const bool in_indexed );

private:
    uint32_t                            m_frame;
    VkClearValue                        m_clearColor;
    VkRect2D                            m_renderArea;
    VkCommandPool                       m_commandPool;
    crvkDynamicVector<VkCommandBuffer>  m_commandBuffers;
};

#endif //!__CRVK_PIPELINE_HPP__
