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

#ifndef __CRVK_SAMPLER_HPP__
#define __CRVK_SAMPLER_HPP__

typedef struct crvkSamplerHandler_t crvkSamplerHandler_t;

class crvkSampler
{
public:
    crvkSampler( void );
    ~crvkSampler( void );

    bool Create( const crvkDevice* in_device,
                const VkSamplerCreateFlags in_flags,
                const VkFilter in_magFilter,
                const VkFilter in_minFilter,
                const VkSamplerMipmapMode in_mipmapMode,
                const VkSamplerAddressMode in_addressModeU,
                const VkSamplerAddressMode in_addressModeV,
                const VkSamplerAddressMode in_addressModeW,
                const float in_mipLodBias,
                const VkBool32 in_anisotropyEnable,
                const float in_maxAnisotropy,
                const VkBool32 in_compareEnable,
                const VkCompareOp in_compareOp,
                const float in_minLod,
                const float in_maxLod,
                const VkBorderColor in_borderColor,
                const VkBool32 in_unnormalizedCoordinates );
    void Destroy( void );

private:
    crvkSamplerHandler_t*   m_samplerHandler;
};

#endif //__CRVK_SAMPLER_HPP__