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

#ifndef __CRVK_PRECOMPILED_HPP__
#define __CRVK_PRECOMPILED_HPP__

#include <iostream> // std::cerr 
#include <cstdint> // int32_t, uint32_, int16_t, uint16_t, UINT32_MAX 
#include <cstring> // std::memcpy, std::strcmp
#include <stdexcept> // std::runtime_error
#include <exception> // std::exeption
#include <algorithm> // std::clamp
#include <cstdio> // std::snprintf
#include <limits> // std::numeric_limits
#include <atomic> // std::atomic

#include <SDL3/SDL_assert.h> // SDL_assert
#include <SDL3/SDL_stdinc.h> // SDL_malloc, SDL_realloc, SDL_free

#include "crvkPointer.hpp"
#include "crvkDynamicVector.hpp"

#include "crvkCore.hpp"
#include "crvkException.hpp"
#include "crvkContext.hpp"
#include "crvkDevice.hpp"
#include "crvkFence.hpp"
#include "crvkSemaphore.hpp"
#include "crvkCommandBuffer.hpp"
#include "crvkSwapchain.hpp"
#include "crvkBuffer.hpp"
#include "crvkShaderStage.hpp"
#include "crvkPipeline.hpp"

#endif //__CRVK_PRECOMPILED_HPP__