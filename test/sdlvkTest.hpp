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

#ifndef __SDLVH_TEST_HPP__
#define __SDLVH_TEST_HPP__

class crvkTest
{
public:
    crvkTest( void );
    ~crvkTest( void );

    void    Run( void );

private:
    SDL_Window*     m_window;
    crvkContext*    m_context;

    void    InitSDL( void );
    void    InitVulkan( void );
    void    FinishSDL( void );
    void    FinishVulkan( void );
    void    RunLoop( void );
};

#endif //!__SDLVH_TEST_HPP__