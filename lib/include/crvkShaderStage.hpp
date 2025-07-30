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

#ifndef __CRVK_SHADER_STAGE_HPP__
#define __CRVK_SHADER_STAGE_HPP__

typedef struct glslang_shader_s glslang_shader_t;
typedef struct glslang_program_s glslang_program_t;
typedef struct glslang_input_s glslang_input_t;

class crvkProgram
{
public:
    crvkProgram( void ) {};
    ~crvkProgram( void ) {};
    virtual const uint32_t                          PipelineShaderStagesCount( void ) const = 0;
    virtual const VkPipelineShaderStageCreateInfo*  PipelineShaderStages( void ) const = 0;
};

class crvkGLSLShader : public crvkProgram
{
public:
    crvkGLSLShader( void );
    ~crvkGLSLShader( void );
    bool                            Create( const crvkDevice* in_device, const VkShaderStageFlagBits in_stage, const char * in_sources );
    void                            Destroy( void );
    VkShaderStageFlagBits           ShaderStageFlag( void ) const { return m_stage; }
    glslang_shader_t*               Shader( void ) const { return m_shdhnd; }
private:
    VkShaderStageFlagBits           m_stage;
    crvkDevice*                     m_device;
    crvkPointer<glslang_input_t>    m_shaderCI;
    glslang_shader_t*               m_shdhnd;  
};

class crvkGLSLProgram : public crvkProgram
{
public:
    crvkGLSLProgram( void );
    ~crvkGLSLProgram( void );
    void                                            Create( const crvkDevice* in_device );
    void                                            Destroy( void );
    void                                            AttachShader( const crvkGLSLShader* in_shader );
    bool                                            LinkProgram( void );
    virtual const uint32_t                          PipelineShaderStagesCount( void ) const { return m_stages.Count(); }
    virtual const VkPipelineShaderStageCreateInfo*  PipelineShaderStages( void ) const; 

private:
    glslang_program_t*                                  m_program;
    VkDevice                                            m_device;
    crvkDynamicVector<VkPipelineShaderStageCreateInfo>  m_stages;
};

class crvkSpirVProgram : public crvkProgram
{
public:
    crvkSpirVProgram( void );
    ~crvkSpirVProgram( void );
    bool    Create( const crvkDevice* in_device, const VkShaderStageFlagBits *in_stage, uint32_t** in_sources, const size_t* in_sizes, const uint32_t in_count );
    void    Destroy( void );
    virtual const uint32_t                          PipelineShaderStagesCount( void ) const { return m_stages.Count(); }
    virtual const VkPipelineShaderStageCreateInfo*  PipelineShaderStages( void ) const;

private:
    VkDevice                                            m_device;
    crvkDynamicVector<VkPipelineShaderStageCreateInfo>  m_stages;
};

#endif //__CRVK_SHADER_STAGE_HPP__