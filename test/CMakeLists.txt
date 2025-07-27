################################################################################
#
# crvkCore - Vulkan + SDL minimal framework
# Copyright (c) 2025 Beato
#
# This file is part of the crvkCore library and is licensed under the
# MIT License with Attribution Requirement.
#
# You are free to use, modify, and distribute this file (even commercially),
# as long as you give credit to the original author:
#
#     “Based on crvkCore by Beato – https://github.com/seuusuario/crvkCore”
#
# For full license terms, see the LICENSE file in the root of this repository.
#
################################################################################

find_package( SDL3 REQUIRED )
include_directories( ${SDL3_INCLUDE_DIR} ) 

# OSDependent need Threads lib 
find_package( Threads REQUIRED )
find_package( glslang REQUIRED )
include_directories( ${glslang_INCLUDE_DIR} ) 

find_package( Vulkan REQUIRED )
include_directories( ${Vulkan_INCLUDE_DIR} ) 

set( CRVKTEST_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/sdlvkTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sdlvkTest.hpp 
    )

set( CRVKTEST_LIBRARIES 
    crvkLib 
    ${SDL3_LIBRARIES} 
    ${Vulkan_LIBRARIES} 
    glslang::glslang
    glslang::SPIRV
    glslang::OSDependent
    Threads::Threads  # Adiciona o que faltava
    )

add_executable( crvkTest ${CRVKTEST_SOURCES} )
add_dependencies( crvkTest crvkLib )
target_link_libraries( crvkTest PRIVATE ${CRVKTEST_LIBRARIES} )