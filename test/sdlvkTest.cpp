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

#include "sdlvkTest.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

const const char* validationLayers[1] = { "VK_LAYER_KHRONOS_validation" };
const const char* deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

crvkTest::crvkTest( void ) : m_window( nullptr )
{
}

crvkTest::~crvkTest( void )
{
}

void crvkTest::Run(void)
{
    InitSDL();
    InitVulkan();
    RunLoop();
    FinishVulkan();
    FinishSDL();
}

void crvkTest::InitSDL(void)
{
    // Initialize SDL3 lib
    if( !SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS ) )
        throw std::runtime_error( SDL_GetError() );

    if ( !SDL_Vulkan_LoadLibrary( nullptr ) )
        throw std::runtime_error( SDL_GetError() );

    // Create the window 
    m_window = SDL_CreateWindow( "crVkLib-Test", 800, 600, SDL_WINDOW_VULKAN );
    if ( !m_window )
        throw std::runtime_error( SDL_GetError() );
}

void crvkTest::InitVulkan(void)
{

    // create the context object 
    m_context = new crvkContext();

    // create our context instance 
    m_context->Create( m_window, "crvkTest", "crvkLib", validationLayers, 1 );

    // get the device list 


    //
    //m_context->InitializeDevice( , );

}

void crvkTest::FinishSDL(void)
{
    // destroy window
    if ( m_window != nullptr )
    {
        SDL_DestroyWindow( m_window );
        m_window = nullptr;
    }

    SDL_Vulkan_UnloadLibrary();
    
    // Free sdl lib 
    SDL_Quit();
}

void crvkTest::FinishVulkan(void)
{
}

void crvkTest::RunLoop(void)
{
    bool running = true;
    do
    {
        SDL_Event evt{};
        while ( SDL_PollEvent( &evt ) )
        {
            switch ( evt.type )
            {
            case SDL_EVENT_WINDOW_DESTROYED:
            case SDL_EVENT_QUIT:
                running = false;
                break;
            
            default:
                break;
            }
        }
    } while ( running );
}

// main entry point
int main(int argc, char *argv[] )
{
    crvkTest app = crvkTest();   
    try
    {
        app.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}