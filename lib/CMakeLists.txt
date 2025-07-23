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
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkPrecompiled.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkCore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkCore.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkException.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkFence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkFence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkSemaphore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkSemaphore.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkDevice.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkContext.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkSwapchain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkSwapchain.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crvkBuffer.hpp
    )

add_library( crvkLib STATIC ${CRVK_SOURCES} )
target_include_directories( crvkLib  PRIVATE ${CMAKE_SOURCE_DIR} )
target_precompile_headers( crvkLib PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/crvkPrecompiled.hpp>" )