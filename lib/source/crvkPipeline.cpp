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
#include "crvkPipeline.hpp"


/*
==============================================
crvkGraphicPipeline::crvkGraphicPipeline
==============================================
*/
crvkGraphicPipeline::crvkGraphicPipeline( void )
{
}

/*
==============================================
crvkGraphicPipeline::Create
==============================================
*/
bool crvkGraphicPipeline::Create( 
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
        const VkPipeline in_basePipelineHandle
    )
{
    VkResult result = VK_SUCCESS;

    ///
    /// create the pipeline Layout 
    /// ==========================================================================
     VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.flags = 0;
    pipelineLayoutCI.setLayoutCount = in_setLayoutCount;
    pipelineLayoutCI.pSetLayouts = in_setLayouts;
    pipelineLayoutCI.pushConstantRangeCount = in_pushConstantRangeCount;
    pipelineLayoutCI.pPushConstantRanges = in_pushConstantRanges;
    auto result = vkCreatePipelineLayout( m_device->Device(), &pipelineLayoutCI, k_allocationCallbacks, &m_pipelineLayout ); 
    if( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkGraphicPipeline::Create::vkCreatePipelineLayout", result );
        return false;
    }


    ///
    /// create the pipeline 
    /// ==========================================================================
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = in_stageCount;
    pipelineInfo.pStages = in_stagesCreateInfo;
    pipelineInfo.pVertexInputState = in_vertexInputStateCreateInfo;
    pipelineInfo.pInputAssemblyState = in_inputAssemblyStateCreateInfo;
    pipelineInfo.pViewportState = in_viewportStateCreateInfo;
    pipelineInfo.pRasterizationState = in_rasterizationStateCreateInfo;
    pipelineInfo.pMultisampleState = in_multisampleStateCreateInfo;
    pipelineInfo.pColorBlendState = in_colorBlendStateCreateInfo;
    pipelineInfo.pDynamicState = in_dynamicStateCreateInfo;   
    pipelineInfo.renderPass = in_renderPass;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.subpass = in_subpass;
    pipelineInfo.basePipelineHandle = in_basePipelineHandle;
    result = vkCreateGraphicsPipelines( m_device->Device(), VK_NULL_HANDLE, 1, &pipelineInfo, k_allocationCallbacks, &m_pipeline ); 
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkGraphicPipeline::Create::vkCreateGraphicsPipelines", result );
        return false;
    }
}

/*
==============================================
crvkGraphicPipeline::Destroy
==============================================
*/
void crvkGraphicPipeline::Destroy( void )
{

}

/*
==============================================
crvkGraphicPipeline::crvkGraphicPipeline
==============================================
*/
crvkGraphicPipeline::~crvkGraphicPipeline( void )
{
}


/*
==============================================
crvkGraphicPipelineExecutor::crvkGraphicPipelineExecutor
==============================================
*/
crvkGraphicPipelineExecutor::crvkGraphicPipelineExecutor( void )
{
}

/*
==============================================
crvkGraphicPipelineExecutor::crvkGraphicPipelineExecutor
==============================================
*/
crvkGraphicPipelineExecutor::~crvkGraphicPipelineExecutor( void )
{
}

/*
==============================================
crvkGraphicPipelineExecutor::crvkGraphicPipelineExecutor
==============================================
*/
bool crvkGraphicPipelineExecutor::Create( 
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
        const VkPipeline in_basePipelineHandle )
{
    VkResult result = VK_SUCCESS;

    // get the graphics queue 
    auto graphics = in_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );

    if( !crvkGraphicPipeline::Create( 
        in_device, 
        in_subpass, 
        in_setLayoutCount, 
        in_setLayouts, 
        in_pushConstantRangeCount, 
        in_pushConstantRanges, 
        in_stageCount, 
        in_stagesCreateInfo, 
        in_vertexInputStateCreateInfo, 
        in_inputAssemblyStateCreateInfo, 
        in_tessellationStateCreateInfo,
        in_viewportStateCreateInfo,
        in_rasterizationStateCreateInfo,
        in_multisampleStateCreateInfo,
        in_depthStencilStateCreateInfo,
        in_colorBlendStateCreateInfo,
        in_dynamicStateCreateInfo,
        in_renderPass,
        in_basePipelineHandle ) )
        return false;

    ///
    /// Allocate command buffers
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = in_frames;
    
    m_commandBuffers.Resize( in_frames );
    result = vkAllocateCommandBuffers( m_device->Device(), &allocInfo, &m_commandBuffers );
    if ( result != VK_SUCCESS ) 
    {   
        
        crvkAppendError( "crvkGraphicPipelineExecutor::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    return true;    
}

/*
==============================================
crvkGraphicPipelineExecutor::Destroy
==============================================
*/
void crvkGraphicPipelineExecutor::Destroy( void )
{
    // release the command buffers 
    vkFreeCommandBuffers( m_device, m_commandPool, m_commandBuffers.Count(), &m_commandBuffers );

    // destroy the graphic pipeline 
    if( m_graphicsPipeline != nullptr )
    {
        vkDestroyPipeline( m_device, m_graphicsPipeline, k_allocationCallbacks );
        m_graphicsPipeline = nullptr;
    }

    // destroy the pipeline layout
    if( m_pipelineLayout != nullptr )
    {
        vkDestroyPipelineLayout( m_device, m_pipelineLayout, k_allocationCallbacks );
        m_pipelineLayout = nullptr;
    }
    
    m_commandPool = nullptr;
}

VkResult crvkGraphicPipelineExecutor::Begin( const uint32_t in_frame, const VkRenderPass in_renderPass, const VkFramebuffer in_frameBuffer )
{
    m_frame = in_frame;
    VkResult result = VK_SUCCESS;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    // reset last command buffer state
    result = vkResetCommandBuffer( m_commandBuffers[m_frame], /*VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT*/ 0 );
    if ( result != VK_SUCCESS ) 
        return result; 

    // begin register the commands 
    result = vkBeginCommandBuffer( m_commandBuffers[m_frame], &beginInfo );
    if ( result != VK_SUCCESS ) 
        return result; 

    // bind the pipeline
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = in_renderPass;
    renderPassInfo.framebuffer = in_frameBuffer;
    renderPassInfo.renderArea = m_renderArea;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &m_clearColor;
    vkCmdBeginRenderPass( m_commandBuffers[m_frame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
    
    // bind pipeline to command buffer 
    vkCmdBindPipeline( m_commandBuffers[m_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline );

    return VK_SUCCESS;
}

VkResult crvkGraphicPipelineExecutor::End( void )
{
    // finish registe commands 
    vkCmdEndRenderPass( m_commandBuffers[m_frame] );
    return vkEndCommandBuffer( m_commandBuffers[m_frame] );
}

void crvkGraphicPipelineExecutor::BindIndexBuffer( const VkBuffer in_indexBuffer, const VkDeviceSize in_offset, const VkIndexType in_indexType )
{
    // Memory barrier to ensure later visibility
    VkBufferMemoryBarrier2 barrier;
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;

    // we are coming from a copy operation
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        
    // and going to a read operation by the raster
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_INDEX_READ_BIT;

    // the source queue, and the destine queue ( currently ignored, may we assign in future )
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    // buffers and ranges 
    barrier.buffer = in_indexBuffer;
    barrier.offset = in_offset;
    barrier.size = VK_WHOLE_SIZE; 

    //
    VkDependencyInfo    dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.pNext = nullptr;
    dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencyInfo.memoryBarrierCount = 0;
    dependencyInfo.pMemoryBarriers = nullptr;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    dependencyInfo.imageMemoryBarrierCount = 0;
    dependencyInfo.pImageMemoryBarriers = nullptr;
    vkCmdPipelineBarrier2( m_commandBuffers[m_frame], &dependencyInfo ); 

    vkCmdBindIndexBuffer( m_commandBuffers[m_frame], in_indexBuffer, in_offset, in_indexType );
}

void crvkGraphicPipelineExecutor::BindVertexBuffers(const VkBuffer *in_vertexBuffers, const VkDeviceSize *in_offsets, const VkDeviceSize *in_sizes, const VkDeviceSize *in_strides, const uint32_t in_base, const uint32_t in_count)
{
    crvkDynamicVector<VkBufferMemoryBarrier2>   barriers;
    barriers.Resize( in_count );
    for ( uint32_t i = 0; i < in_count; i++)
    {
        // Memory barrier to ensure later visibility
        VkBufferMemoryBarrier2 barrier;
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;

        // we are coming from a copy operation
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        
        // and going to a read operation by the shader
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

        // the source queue, and the destine queue ( currently ignored, may we assign in future )
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        
        // buffers and ranges 
        barrier.buffer = in_vertexBuffers[i];
        barrier.offset = in_offsets[i];
        barrier.size = in_sizes[i]; 
        
        barriers[i] = barrier;
    }

    //
    VkDependencyInfo    dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.pNext = nullptr;
    dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencyInfo.memoryBarrierCount = 0;
    dependencyInfo.pMemoryBarriers = nullptr;
    dependencyInfo.bufferMemoryBarrierCount = barriers.Count();
    dependencyInfo.pBufferMemoryBarriers = &barriers;
    dependencyInfo.imageMemoryBarrierCount = 0;
    dependencyInfo.pImageMemoryBarriers = nullptr;
    vkCmdPipelineBarrier2( m_commandBuffers[m_frame], &dependencyInfo ); 

    vkCmdBindVertexBuffers2( m_commandBuffers[m_frame], in_base, in_count, in_vertexBuffers, in_offsets, in_sizes, in_strides );
}
