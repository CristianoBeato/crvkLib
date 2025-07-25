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
crvkGraphicPipeline::crvkGraphicPipeline
==============================================
*/
crvkGraphicPipeline::~crvkGraphicPipeline( void )
{
}


/*
==============================================
crvkPipelineCommand::crvkPipelineCommand
==============================================
*/
crvkPipelineCommand::crvkPipelineCommand( void )
{
}

/*
==============================================
crvkPipelineCommand::crvkPipelineCommand
==============================================
*/
crvkPipelineCommand::~crvkPipelineCommand( void )
{
}

/*
==============================================
crvkPipelineCommand::crvkPipelineCommand
==============================================
*/
bool crvkPipelineCommand::Create( 
        const crvkDevice* in_device, 
        const uint32_t in_frames, 
        const uint32_t in_subpass,
        const VkPipelineCreateFlags in_flags,
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

    m_device = in_device->Device();
    m_commandPool = graphics->CommandPool();

    ///
    /// create the pipeline 
    /// ==========================================================================
    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 0;
    pipelineLayoutCI.pushConstantRangeCount = 0;

    auto result = vkCreatePipelineLayout( m_device, &pipelineLayoutCI, k_allocationCallbacks, &m_pipelineLayout ); 
    if( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkPipelineCommand::Create::vkCreatePipelineLayout", result );
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
    result = vkCreateGraphicsPipelines( m_device, VK_NULL_HANDLE, 1, &pipelineInfo, k_allocationCallbacks, &m_graphicsPipeline ); 
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkPipelineCommand::Create::vkCreateGraphicsPipelines", result );
        return false;
    }

    ///
    /// Allocate command buffers
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = in_frames;
    
    m_commandBuffers.Resize( in_frames );
    VkResult result = vkAllocateCommandBuffers( m_device, &allocInfo, &m_commandBuffers );
    if ( result != VK_SUCCESS ) 
    {   
        
        crvkAppendError( "crvkPipelineCommand::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    return true;    
}

/*
==============================================
crvkPipelineCommand::Destroy
==============================================
*/
void crvkPipelineCommand::Destroy( void )
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

VkResult crvkPipelineCommand::Begin( const uint32_t in_frame, const VkRenderPass in_renderPass, const VkFramebuffer in_frameBuffer )
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
    vkCmdBindPipeline( m_commandBuffers[m_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline );

    return VK_SUCCESS;
}

VkResult crvkPipelineCommand::End( void )
{
    // finish registe commands 
    vkCmdEndRenderPass( m_commandBuffers[m_frame] );
    return vkEndCommandBuffer( m_commandBuffers[m_frame] );
}