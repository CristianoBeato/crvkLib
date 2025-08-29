#
#===========================================================================================
#    This file is part of crvkLib Vulkan + SDL minimal framework
#
#    Copyright (c) 2025 Cristiano B. Santos <cristianobeato_dm@hotmail.com>
#    Contributor(s): none yet.
#
#-------------------------------------------------------------------------------------------
#
# This file is part of the crvkLib library and is licensed under the
# MIT License with Attribution Requirement.
#
# You are free to use, modify, and distribute this file (even commercially),
# as long as you give credit to the original author:
#
#     “Based on crvkLib by Cristiano Beato – https://github.com/CristianoBeato/crvkLib”
#
# For full license terms, see the LICENSE file in the root of this repository.
#
################################################################################

set( CRVK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkCommandBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkCore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkFence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkFrameBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkPipeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkPrecompiled.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkPointer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkSampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkSemaphore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkShaderStage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkSwapchain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/crvkDynamicVector.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkImage.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkCommandBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkContext.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkCore.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkDevice.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkException.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkFence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkFrameBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkPipeline.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkSampler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkSemaphore.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkShaderStage.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crvkSwapchain.hpp
    )

add_library( crvkLib STATIC ${CRVK_SOURCES} )
target_include_directories( crvkLib  PRIVATE ${CMAKE_SOURCE_DIR} )
target_precompile_headers( crvkLib PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/source/crvkPrecompiled.hpp>" )