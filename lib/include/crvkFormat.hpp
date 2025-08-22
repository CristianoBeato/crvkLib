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
#ifndef __CRVK_FORMAT_HPP__
#define __CRVK_FORMAT_HPP__

struct crvkFormat_t
{
    VkFormat format;
    
    crvkFormat_t( void ) : format( VK_FORMAT_UNDEFINED )
    {
    }

    crvkFormat_t( const VkFormat &in_format ) : format( in_format )
    {
    }

    const bool      IsCompressed( void ) const;
    const uint32_t  ComponetType( void ) const;
    const uint32_t  Components( void ) const;
    const uint32_t  BytesPerPixel( void ) const;

    operator VkFormat( void ) const { return format; }
};
    
#endif //__CRVK_FORMAT_HPP__