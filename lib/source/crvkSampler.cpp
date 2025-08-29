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
#include "crvkSampler.hpp"

typedef struct crvkSamplerHandler_t 
{
    VkSampler   sampler = nullptr;
    VkDevice    device = nullptr;
} crvkSamplerHandler_t;

/*
==============================================
crvkSampler::crvkSampler
==============================================
*/
crvkSampler::crvkSampler( void ) : m_samplerHandler( nullptr )
{
    m_samplerHandler = new crvkSamplerHandler_t();
}

/*
==============================================
crvkSampler::~crvkSampler
==============================================
*/
crvkSampler::~crvkSampler( void )
{
    if ( m_samplerHandler != nullptr )
    {
        delete m_samplerHandler;
        m_samplerHandler = nullptr;
    }
}

/*
==============================================
crvkSampler::Create
==============================================
*/
bool crvkSampler::Create(
        const crvkDevice *in_device, 
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
        const VkBool32 in_unnormalizedCoordinates )
{
    VkResult result = VK_SUCCESS;
    VkSamplerCreateInfo samplerCI{};

    if ( m_samplerHandler == nullptr )
    {
        // TODO: call a error
        return false;
    }
    
    m_samplerHandler->device = in_device->Device();

    samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCI.pNext = nullptr;
    samplerCI.flags = in_flags;
    samplerCI.magFilter = in_magFilter;
    samplerCI.minFilter = in_minFilter;
    samplerCI.mipmapMode = in_mipmapMode;
    samplerCI.addressModeU = in_addressModeU;
    samplerCI.addressModeV = in_addressModeV;
    samplerCI.addressModeW = in_addressModeW;
    samplerCI.mipLodBias = in_mipLodBias;
    samplerCI.anisotropyEnable = in_anisotropyEnable;
    samplerCI.maxAnisotropy = in_maxAnisotropy;
    samplerCI.compareEnable = in_compareEnable;
    samplerCI.compareOp = in_compareOp;
    samplerCI.minLod = in_minLod;
    samplerCI.maxLod = in_maxLod;
    samplerCI.borderColor = in_borderColor;
    samplerCI.unnormalizedCoordinates = in_unnormalizedCoordinates;
    result = vkCreateSampler( m_samplerHandler->device, &samplerCI, k_allocationCallbacks, &m_samplerHandler->sampler );
    if( result != VK_SUCCESS )
        return false;

    return true;
}

/*
==============================================
crvkSampler::Destroy
==============================================
*/
void crvkSampler::Destroy(void)
{
    if ( m_samplerHandler == nullptr )
        return;    

    if ( m_samplerHandler->sampler != nullptr )
    {
        vkDestroySampler( m_samplerHandler->device, m_samplerHandler->sampler, k_allocationCallbacks );
        m_samplerHandler->sampler = nullptr;
    }
    
    m_samplerHandler->device = nullptr;
}