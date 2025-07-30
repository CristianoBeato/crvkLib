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

//#include <vector>
//#include <glslang/Public/ShaderLang.h>
//#include <glslang/SPIRV/GlslangToSpv.h>

#include <glslang/Include/glslang_c_shader_types.h>
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include "crvkPrecompiled.hpp"
#include "crvkShaderStage.hpp"

static const char* k_GLSL_SHADER_ENTRY_POINT = "main";

static const glslang_stage_t VulkanStageToGlslangStage( const VkShaderStageFlagBits in_stage )
{
    glslang_stage_t stage = GLSLANG_STAGE_VERTEX;
    switch ( in_stage )
    {
    case VK_SHADER_STAGE_VERTEX_BIT:
        stage = GLSLANG_STAGE_VERTEX;
        break;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        stage = GLSLANG_STAGE_TESSCONTROL;
        break;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        stage = GLSLANG_STAGE_TESSEVALUATION;
        break;
    case VK_SHADER_STAGE_GEOMETRY_BIT:
        stage = GLSLANG_STAGE_GEOMETRY;
        break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
        stage = GLSLANG_STAGE_FRAGMENT;
        break;
    case VK_SHADER_STAGE_COMPUTE_BIT:
        stage = GLSLANG_STAGE_COMPUTE;
        break;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
        stage = GLSLANG_STAGE_RAYGEN;
        break;
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
        stage = GLSLANG_STAGE_ANYHIT;
        break;
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
        stage = GLSLANG_STAGE_CLOSESTHIT;
        break;
    case VK_SHADER_STAGE_MISS_BIT_KHR:
        stage = GLSLANG_STAGE_MISS;
        break;
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
        stage = GLSLANG_STAGE_INTERSECT;
        break;
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
        stage = GLSLANG_STAGE_CALLABLE;
        break;
    case VK_SHADER_STAGE_TASK_BIT_EXT:
        stage = GLSLANG_STAGE_TASK;
        break;
    case VK_SHADER_STAGE_MESH_BIT_EXT:
        stage = GLSLANG_STAGE_MESH;
        break;
    default:    // TODO: call a error 
        throw std::runtime_error( "Invalid, or unsuported shader stage!" );
    }

    return stage;
}

crvkGLSLShader::crvkGLSLShader( void ) : m_shdhnd( nullptr )
{
}

crvkGLSLShader::~crvkGLSLShader(void)
{
    Destroy();
}

bool crvkGLSLShader::Create( const crvkDevice *in_device, const VkShaderStageFlagBits in_stage, const char * in_sources )
{
    m_stage = in_stage;
    m_device = const_cast<crvkDevice*>( in_device );
    m_shaderCI.Alloc( 1 );
    m_shaderCI->language = GLSLANG_SOURCE_GLSL;
    m_shaderCI->client = GLSLANG_CLIENT_VULKAN; 
    m_shaderCI->client_version = GLSLANG_TARGET_VULKAN_1_3; // GLSLANG_TARGET_VULKAN_1_3
    m_shaderCI->target_language = GLSLANG_TARGET_SPV;
    m_shaderCI->target_language_version = GLSLANG_TARGET_SPV_1_3; //GLSLANG_TARGET_SPV_1_6;
    m_shaderCI->messages = GLSLANG_MSG_DEFAULT_BIT;
    m_shaderCI->default_profile = GLSLANG_NO_PROFILE;
    m_shaderCI->resource = m_device->BuiltInShaderResource();
    m_shaderCI->default_version = 450; // GLSL 4.5
    m_shaderCI->force_default_version_and_profile = false;
    m_shaderCI->forward_compatible = false;
    m_shaderCI->code = in_sources;
    m_shaderCI->stage = VulkanStageToGlslangStage( in_stage );
    
    m_shdhnd = glslang_shader_create( &m_shaderCI );
    if ( !m_shdhnd )
    {
        return false;
    }
    
    if ( glslang_shader_preprocess( m_shdhnd, &m_shaderCI ) == 0 )	
    {
        printf("%s\n", glslang_shader_get_info_log( m_shdhnd ) );
        printf("%s\n", glslang_shader_get_info_debug_log( m_shdhnd ) );
        //printf("%s\n", input.code);
        return false;
    }

    if ( glslang_shader_parse( m_shdhnd, &m_shaderCI) == 0 ) 
    {
        printf( "%s\n", glslang_shader_get_info_log( m_shdhnd ) );
        printf( "%s\n", glslang_shader_get_info_debug_log( m_shdhnd ) );
        printf( "%s\n", glslang_shader_get_preprocessed_code( m_shdhnd ) );
        return false;
    }

    return true;
}

void crvkGLSLShader::Destroy( void )
{
    if ( m_shdhnd != nullptr )
    {
        glslang_shader_delete( m_shdhnd );
        m_shdhnd = nullptr;
    }
    
    m_device = nullptr;
}

crvkGLSLProgram::crvkGLSLProgram(void) : 
    m_program( nullptr ),
    m_device( nullptr )
{
}

crvkGLSLProgram::~crvkGLSLProgram(void)
{
}

void crvkGLSLProgram::Create( const crvkDevice *in_device )
{
    // get the device 
    m_device = in_device->Device();

    // create the program handle
    m_program = glslang_program_create();
}

void crvkGLSLProgram::Destroy( void )
{   
    for ( uint32_t i = 0; i < m_stages.Count(); i++)
    {
        auto stage = m_stages[i];
        if ( stage.module == nullptr )
            continue;
        
        vkDestroyShaderModule( m_device, stage.module, k_allocationCallbacks );
        stage.module = nullptr;
    }
    
    m_stages.Clear();
}

void crvkGLSLProgram::AttachShader(const crvkGLSLShader *in_shader)
{
    VkPipelineShaderStageCreateInfo pipelineShaderStageCI{};

    // append shader stage
    glslang_program_add_shader( m_program, in_shader->Shader() );

    pipelineShaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCI.stage = in_shader->ShaderStageFlag();
    pipelineShaderStageCI.module = nullptr;
    pipelineShaderStageCI.pName = k_GLSL_SHADER_ENTRY_POINT;

    m_stages.Append( pipelineShaderStageCI );
}

bool crvkGLSLProgram::LinkProgram(void)
{
    // link shaders stages 
    if ( !glslang_program_link( m_program, GLSLANG_MSG_DEFAULT_BIT /*GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT*/ ) ) 
    {
        printf("%s\n", glslang_program_get_info_log( m_program ));
        printf("%s\n", glslang_program_get_info_debug_log( m_program ));
        return false;
    }

    // create the vulkan stages 
    for ( uint32_t i = 0; i < m_stages.Count(); i++)
    {
#if 0
        // aquire spirV shader source
        glslang_spv_options_t spvOptions{};        
        spvOptions.disable_optimizer = false;
        spvOptions.optimize_size = true;
        spvOptions.disassemble = false;

#if defined( NDEBUG )
        spvOptions.strip_debug_info = true;
        spvOptions.generate_debug_info = false;
        spvOptions.validate = false;
        spvOptions.emit_nonsemantic_shader_debug_info = false;
        spvOptions.emit_nonsemantic_shader_debug_source = false;
#else
        spvOptions.strip_debug_info = false;
        spvOptions.generate_debug_info = true;
        spvOptions.validate = true;
        spvOptions.emit_nonsemantic_shader_debug_info = true;
        spvOptions.emit_nonsemantic_shader_debug_source = true;
#endif
        glslang_program_SPIRV_generate_with_options( m_program, VulkanStageToGlslangStage( m_stages[i].stage ), &spvOptions );
#else
        glslang_program_SPIRV_generate( m_program, VulkanStageToGlslangStage( m_stages[i].stage ) );
#endif

        crvkPointer<uint32_t> code;
        code.Alloc( glslang_program_SPIRV_get_size( m_program ), 0x00 );

        // create the shader module
        VkShaderModuleCreateInfo shaderModuleCI{};
        shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCI.codeSize = code.Size();
        shaderModuleCI.pCode = &code;
        shaderModuleCI.pNext = nullptr;

        auto result = vkCreateShaderModule( m_device, &shaderModuleCI, k_allocationCallbacks, &m_stages[i].module ); 
        if ( result != VK_SUCCESS ) 
        {    
            crvkAppendError( "crvkGLSLShader::Create::vkCreateShaderModule", VK_ERROR_UNKNOWN );
            return false;
        }

        code.Free(); // release source
    }
    
    // clear program
    glslang_program_delete( m_program );

    return true;
}

const VkPipelineShaderStageCreateInfo *crvkGLSLProgram::PipelineShaderStages(void) const
{
    return m_stages.Pointer();
}

crvkSpirVProgram::crvkSpirVProgram( void ) : m_device( nullptr )
{
}

crvkSpirVProgram::~crvkSpirVProgram( void )
{
}

bool crvkSpirVProgram::Create( const crvkDevice* in_device, const VkShaderStageFlagBits *in_stage, uint32_t** in_sources, const size_t* in_sizes, const uint32_t in_count )
{
    m_stages.Resize( in_count );

    for ( uint32_t i = 0; i < m_stages.Count(); i++)
    {
        // configure shader stage
        auto &stage = m_stages[i];
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = in_stage[i];
        stage.pName = k_GLSL_SHADER_ENTRY_POINT;   
        
        // create the shader module
        VkShaderModuleCreateInfo shaderModuleCI{};
        shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCI.codeSize = in_sizes[i];
        shaderModuleCI.pCode = in_sources[i];
        shaderModuleCI.pNext = nullptr;

        auto result = vkCreateShaderModule( m_device, &shaderModuleCI, k_allocationCallbacks, &m_stages[i].module ); 
        if ( result != VK_SUCCESS ) 
        {    
            crvkAppendError( "crvkGLSLShader::Create::vkCreateShaderModule", VK_ERROR_UNKNOWN );
            return false;
        }
    }
}

void crvkSpirVProgram::Destroy( void )
{
    for ( uint32_t i = 0; i < m_stages.Count(); i++)
    {
        auto &stage = m_stages[i];
        if ( stage.module == nullptr )
            continue;
        
        vkDestroyShaderModule( m_device, stage.module, k_allocationCallbacks );
        stage.module = nullptr;    
    }

    m_stages.Clear();
}

const VkPipelineShaderStageCreateInfo* crvkSpirVProgram::PipelineShaderStages( void ) const
{
    return &m_stages;
}
