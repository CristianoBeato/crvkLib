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

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h> // define DefaultTBuiltInResource

#include "crvkPrecompiled.hpp"
#include "crvkShaderStage.hpp"

crvkShaderStage::crvkShaderStage( void ) : m_shaderModule( nullptr )
{
    std::memset( &m_pipelineShaderStageCreateInfo, 0x00, sizeof( VkPipelineShaderStageCreateInfo ) );
}

crvkShaderStage::~crvkShaderStage( void )
{
    Destroy();
}

bool crvkShaderStage::Create(const crvkDevice *in_devie, const uint32_t in_sourceCount, const char* const* in_sources, const VkShaderStageFlagBits in_shaderStageFlagBits, const void *in_next)
{
    EShLanguage shaderStage;
    glslang::TProgram program;
    std::vector<uint32_t> spirvSource;

    switch ( in_shaderStageFlagBits )
    {
    case VK_SHADER_STAGE_VERTEX_BIT:
        shaderStage = EShLangVertex; 
        break;

    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        shaderStage = EShLangTessControl;
        break;

    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        shaderStage = EShLangTessEvaluation;
        break;

    case VK_SHADER_STAGE_GEOMETRY_BIT:
        shaderStage = EShLangGeometry;
        break;

    case VK_SHADER_STAGE_FRAGMENT_BIT:
        shaderStage = EShLangFragment;
        break;

    case VK_SHADER_STAGE_COMPUTE_BIT:
        shaderStage = EShLangCompute;
        break;

    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
        shaderStage = EShLangRayGen;
        break;
    case VK_SHADER_STAGE_MESH_BIT_EXT:
        shaderStage = EShLangMesh;    
        break;

    default:    // TODO: call a error 
        return false;
    }

    glslang::TShader shader( shaderStage ); // shaderStage is a glslang::EShLanguage enum
    shader.setStrings( in_sources, in_sourceCount );
    shader.setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, 450 );
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
    
    // todo configure using vulkan properties 
    // TBuiltInResource defaultTBuiltInResource{};

    // DefaultTBuiltInResource is a struct containing limits and capabilities
    // 100 is the default GLSL version (e.g., 450 for Vulkan)
    // false for no forward-compatible flag
    // EShMsgDefault for default messages
    if( !shader.parse( GetDefaultResources(), 450, false, EShMsgVulkanRules ) )     
    {
        return false;
    }

    program.addShader( &shader );
    if( !program.link( EShMessages::EShMsgDefault ) )
    {
        crvkAppendError( program.getInfoLog(), VK_ERROR_INITIALIZATION_FAILED );
        return false;
    }

    glslang::GlslangToSpv( *program.getIntermediate(shaderStage), spirvSource );

    // create the shader module
    VkShaderModuleCreateInfo shaderModuleCI{};
    shaderModuleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderModuleCI.codeSize = spirvSource.size();
    shaderModuleCI.pCode = spirvSource.data();
    shaderModuleCI.pNext = in_next;
    auto result = vkCreateShaderModule( in_devie->Device(), &shaderModuleCI, nullptr, &m_shaderModule ); 
    if ( result != VK_SUCCESS ) 
    {    
        crvkAppendError( "crvkShaderStage::Create::vkCreateShaderModule", VK_ERROR_UNKNOWN );
        return false;
    }

    m_pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_pipelineShaderStageCreateInfo.stage = in_shaderStageFlagBits;
    m_pipelineShaderStageCreateInfo.module = m_shaderModule;
    m_pipelineShaderStageCreateInfo.pName = "main"; // glsl use defaut main entry

    return true;
}

bool crvkShaderStage::Create( const crvkDevice* in_devie, const size_t in_codeSize, const uint32_t* in_code, const char* in_entry, const VkShaderStageFlagBits in_shaderStageFlagBits, const void* in_next )
{
    // create the shader module
    VkShaderModuleCreateInfo shaderModuleCI{};
    shaderModuleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderModuleCI.codeSize = in_codeSize;
    shaderModuleCI.pCode = in_code;
    shaderModuleCI.pNext = in_next;
    auto result = vkCreateShaderModule( in_devie->Device(), &shaderModuleCI, nullptr, &m_shaderModule ); 
    if ( result != VK_SUCCESS ) 
    {    
        crvkAppendError( "crvkShaderStage::Create::vkCreateShaderModule", VK_ERROR_UNKNOWN );
        return false;
    }

    m_pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_pipelineShaderStageCreateInfo.stage = in_shaderStageFlagBits;
    m_pipelineShaderStageCreateInfo.module = m_shaderModule;
    m_pipelineShaderStageCreateInfo.pName = in_entry;

    return true;
}

void crvkShaderStage::Destroy( void )
{
    if ( m_shaderModule != nullptr )
    {
        vkDestroyShaderModule( m_device, m_shaderModule, k_allocationCallbacks );
        m_shaderModule = nullptr;
    }
    
    m_device = nullptr;

    std::memset( &m_pipelineShaderStageCreateInfo, 0x00, sizeof( VkPipelineShaderStageCreateInfo ) );
}
