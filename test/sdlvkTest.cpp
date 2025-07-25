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
#include <fstream>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include "crvkCore.hpp"

static const char* validationLayers[1] = { "VK_LAYER_KHRONOS_validation" };
static const char* deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#if 0
static const uint32_t k_FRAME_COUNT = 2; // double buffered
#else
static const uint32_t k_FRAME_COUNT = 3; // triple buffered
#endif

typedef struct vec2
{
    float x = 0.0f;
    float y = 0.0f;
} vec2;

typedef struct vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
} vec3;

typedef struct crVertex 
{
    vec3 pos;
    vec2 uv;
    vec3 color;
} crVertex;

const uint16_t indices[6] = 
{
    0, 1, 2,
    2, 1, 3
};

const crVertex vertices[4] = 
{
    // vertex positions         uv coordinate   // vertex color 
    { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }, // RT
    { {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } }, // RB
    { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // LT
    { { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } }, // LB
};

crvkTest::crvkTest( void ) : 
    m_window( nullptr ),
    m_context( nullptr ),
    m_device( nullptr ),
    m_swapchain( nullptr ),
    m_vertexBuffer( nullptr ),
    m_elementBuffer( nullptr )
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
    uint32_t deviceCount = 0;
    int width = 0, height = 0;
    crvkDevice* const* devices = nullptr;

    // create the context object 
    m_context = new crvkContext();

    // create our context instance 
    if( !m_context->Create( m_window, "crvkTest", "crvkLib", validationLayers, 1 ) )
        throw std::runtime_error( "can't create context!" );

    // get the device list 
    devices = m_context->GetDeviceList( &deviceCount );
    
    
    // TODO: find the best device 
    // just pic the first 
    m_device = devices[0];
    m_device->Create( validationLayers, 1, deviceExtensions, 1 );

    // aquire window surface size 
    SDL_GetWindowSizeInPixels( m_window, &width, &height );

    auto surfaceCapabilities = m_device->SurfaceCapabilities();

    // create the swapchain 
    m_swapchain = new crvkSwapchain();
    if( !m_swapchain->Create( 
        m_context,      // the context for error handling 
        m_device,       // aquire the device 
        m_device->FindBestImageCount( k_FRAME_COUNT ),
        // try find if device suport the requested swapchain properties 
        m_device->FindExtent( width, height ),
        m_device->FindSurfaceFormat( VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ),
        m_device->FindPresentMode( VK_PRESENT_MODE_MAILBOX_KHR ),
        surfaceCapabilities.currentTransform
    ) )
        throw std::runtime_error( "can't create device!" );

    // Create the element buffer 
    m_elementBuffer = new crvkBufferStaging();
    if( !m_elementBuffer->Create( m_device, sizeof( uint16_t ) * 6, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, CRVK_BUFFER_DYNAMIC_STORAGE_BIT ) )
        throw std::runtime_error( "can't create element buffer" );

    // Copy the index to buffer 
    m_elementBuffer->SubData( indices, 0, sizeof( uint16_t ) * 6 );

    // Create the vertex buffer
    m_vertexBuffer = new crvkBufferStaging();
    if( !m_vertexBuffer->Create( m_device, sizeof( crVertex ) * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, CRVK_BUFFER_DYNAMIC_STORAGE_BIT ) )
        throw std::runtime_error( "can't create element buffer" );

    // copy vertex to buffer
    m_vertexBuffer->SubData( m_device, 0, sizeof( crVertex ) * 4 );
}

void crvkTest::InitShaders( void )
{
}

/*
static crvkShaderStage* CreateShader( , const char* in_srcPath, const crvkDevice* in_device )
{
    std::ifstream file( in_srcPath, std::ios::ate );

    crvkShaderStage* vertexShader = nullptr;
    
    if (!file.is_open()) 
        throw std::runtime_error("failed to open file!");
    
    size_t fileSize = (size_t) file.tellg();
    vertexShader = new crvkShaderStage();

    const char* src = static_cast<const char*>( SDL_malloc( fileSize ) );
    
    // read source content 
    file.seekg(0);
    file.read( const_cast<char*>( src ), fileSize );
    file.close();

    // 
    vertexShader->Create( in_device, 1, &src,  );
}
*/

void crvkTest::InitPipeline(void)
{
    size_t fileSize = 0;
    std::ifstream sourceFile;
    const char* shaderSource = nullptr;
    crvkShaderStage* vertexShader = nullptr;
    crvkShaderStage* fragmentShader = nullptr;
    
#if 1 // USE GLSL sources

    // Open and compile vertx shader  
    sourceFile.open( "shaders/test_shader.vert" );

    if (!sourceFile.is_open() ) 
        throw std::runtime_error("failed to open file!");
    
    fileSize = (size_t) sourceFile.tellg();
    
    shaderSource = static_cast<const char*>( SDL_malloc( fileSize ) );
    
    // read source content 
    sourceFile.seekg(0);
    sourceFile.read( const_cast<char*>( shaderSource ), fileSize );
    sourceFile.close();
    
    vertexShader = new crvkShaderStage();
    if( !vertexShader->Create( m_device, 1, &shaderSource, VK_SHADER_STAGE_VERTEX_BIT ) )
    {
        SDL_free( const_cast<char*>( shaderSource ) );
        return; // todo: do a throw
    }

    // release shader source
    SDL_free( const_cast<char*>( shaderSource ) );
    
    // Open and compile vertx shader  
    sourceFile.open( "shaders/test_shader.frag" );

    if (!sourceFile.is_open() ) 
        throw std::runtime_error("failed to open file!");
    
    fileSize = (size_t) sourceFile.tellg();
    
    shaderSource = static_cast<const char*>( SDL_malloc( fileSize ) );
    
    // read source content 
    sourceFile.seekg(0);
    sourceFile.read( const_cast<char*>( shaderSource ), fileSize );
    sourceFile.close();
    
    fragmentShader = new crvkShaderStage();
    if( !fragmentShader->Create( m_device, 1, &shaderSource, VK_SHADER_STAGE_FRAGMENT_BIT ) )
    {
        SDL_free( const_cast<char*>( shaderSource ) );
        return; // todo: do a throw
    }

    SDL_free( const_cast<char*>( shaderSource ) );

    
#else // USE DIRECT SPIR-V intermediate representation

#endif

//    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//    vertShaderStageInfo.module = vertShaderModule;
//    vertShaderStageInfo.pName = "main";

//    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//    fragShaderStageInfo.module = fragShaderModule;
//    fragShaderStageInfo.pName = "main";

    m_pipeline = new crvkPipelineCommand();
    //m_pipeline->Create( m_device, m_swapchain->FrameCount(), 0,  );

    delete fragmentShader;
    delete vertexShader;
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
    if( m_vertexBuffer != nullptr )
    {   
        delete m_vertexBuffer;
        m_vertexBuffer = nullptr;
    }

    if ( m_elementBuffer != nullptr )
    {
        delete m_elementBuffer;
        m_elementBuffer = nullptr;
    }

    if ( m_pipeline != nullptr )
    {
        delete m_pipeline;
        m_pipeline = nullptr;
    }
    
    
    if( m_swapchain != nullptr )
    {
        delete m_swapchain;
        m_swapchain = nullptr;
    }

    m_device->Destroy();
    m_device = nullptr;

    if ( m_context != nullptr )
    {
        delete m_context;
        m_context = nullptr;
    }
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
    catch( const std::exception& e )
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}