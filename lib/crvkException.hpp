// ===============================================================================================
// crvkCore - Vulkan + SDL minimal framework
// Copyright (c) 2025 Beato
//
// This file is part of the crvkCore library and is licensed under the
// MIT License with Attribution Requirement.
//
// You are free to use, modify, and distribute this file (even commercially),
// as long as you give credit to the original author:
//
//     “Based on crvkCore by Beato – https://github.com/seuusuario/crvkCore”
//
// For full license terms, see the LICENSE file in the root of this repository.
// ===============================================================================================

#ifndef __CRVK_EXCEPTION_HPP__
#define __CRVK_EXCEPTION_HPP__

#include <exception>

// get a error string from vulkan error code
extern const char* crvkGetVulkanError( const VkResult errorCode );

class crvkException : public std::exception
{
public:
    explicit crvkException( const char* _error, const VkResult errorCode );
    virtual const char* what( void ) const noexcept { return err; }
    
private:
        char  err[1024];
};

#define VK_CHECK( X ) VkResult res = X; if( res != VK_SUCCESS ) throw crvkException( #X, res );

#endif // __CRVK_EXCEPTION_HPP__