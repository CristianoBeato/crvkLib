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

class crvkShaderStage
{
public:
    crvkShaderStage( void );
    ~crvkShaderStage( void );

    /// @brief Create shader based on GLSL
    /// @param in_devie 
    /// @param in_codeLegenth 
    /// @param in_code 
    /// @param in_shaderStageFlagBits 
    /// @param in_next 
    /// @return 
    bool    Create( const crvkDevice* in_devie, 
                    const uint32_t in_sourceCount, 
                    const char* const* in_sources, 
                    const VkShaderStageFlagBits in_shaderStageFlagBits, 
                    const void* in_next = nullptr );

    /// @brief Create shader based on SPIR-V
    /// @param in_codeSize 
    /// @param in_code 
    /// @param in_entry 
    /// @param in_shaderStageFlagBits 
    /// @param in_next 
    /// @return 
    bool    Create( const crvkDevice* in_devie, 
                    const size_t in_codeSize, 
                    const uint32_t* in_code, 
                    const char* in_entry, 
                    const VkShaderStageFlagBits in_shaderStageFlagBits, 
                    const void* in_next );
    void    Destroy( void );

    const VkPipelineShaderStageCreateInfo ShaderStage( void ) const;

private:
    VkPipelineShaderStageCreateInfo m_pipelineShaderStageCreateInfo;
    VkShaderModule                  m_shaderModule;
    VkDevice                        m_device;
};

#endif //__CRVK_SHADER_STAGE_HPP__