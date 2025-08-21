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
#include <cstring>
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
    if( !m_elementBuffer->Create( m_device, sizeof( uint16_t ) * 6, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 0  ) )
        throw std::runtime_error( "can't create element buffer" );

    // Copy the index to buffer 
    m_elementBuffer->SubData( indices, 0, sizeof( uint16_t ) * 6 );

    // Create the vertex buffer
    m_vertexBuffer = new crvkBufferStaging();
    if( !m_vertexBuffer->Create( m_device, sizeof( crVertex ) * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 0 ) )
        throw std::runtime_error( "can't create element buffer" );

    // copy vertex to buffer
    m_vertexBuffer->SubData( m_device, 0, sizeof( crVertex ) * 4 );

    m_cmd = new crvkCommandBufferRoundRobin();
    m_cmd->Create( m_device, m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS ), 3 );

    InitShaders();

    InitPipeline( 0 );
}

void crvkTest::InitShaders( void )
{
    
    FILE* sourceFile = nullptr;
    
    // file size lambda
    auto fSize = []( FILE* in_file )
    {
        size_t size = 0;
        fseek( in_file, 0, SEEK_END ); // move to file end 
        size = ftell( in_file ); // get the data legth 
        fseek( in_file, 0, SEEK_SET ); // back to data start 
        return size;
    };
    
#if 0
    size_t fileSize = 0;
    char* shaderSource = nullptr;
    crvkShader* vertexShader = nullptr;
    crvkShader* fragmentShader = nullptr;
    
    // Open and compile vertx shader
    sourceFile = fopen( "shaders/test_shader.vert", "r" );
    if ( sourceFile == nullptr ) 
        throw std::runtime_error("failed to open file!");
    
    fileSize = fSize( sourceFile );
   
    shaderSource = static_cast<char*>( SDL_malloc( fileSize ) );
    std::memset( shaderSource, 0x00, fileSize );

    // read source content
    fread( shaderSource, 1, fileSize, sourceFile ); 

    // release the file 
    fclose( sourceFile );
    
    // create vertex shader 
    vertexShader = new crvkShader();
    if( !vertexShader->Create( m_device, VK_SHADER_STAGE_VERTEX_BIT, shaderSource ) )
    {
        SDL_free(  shaderSource );
        throw std::runtime_error("failed to createh the shader!");
    }

    // release shader source
    SDL_free( shaderSource );
    
    // Open and compile vertx shader  
    sourceFile = fopen( "shaders/test_shader.frag", "r" );

    if ( sourceFile == nullptr ) 
        throw std::runtime_error("failed to open file!");
    
    fileSize = fSize( sourceFile );
    
    shaderSource = static_cast<char*>( SDL_malloc( fileSize ) );
    std::memset( shaderSource, 0x00, fileSize );
    
    // read source content
    fread( shaderSource, 1, fileSize, sourceFile ); 

    // release the file 
    fclose( sourceFile );

    // create the shader 
    fragmentShader = new crvkShader();
    if( !fragmentShader->Create( m_device,VK_SHADER_STAGE_FRAGMENT_BIT, shaderSource ) )
    {
        SDL_free( const_cast<char*>( shaderSource ) );
        throw std::runtime_error("failed to create the shader!");
    }

    SDL_free( const_cast<char*>( shaderSource ) );

    m_shaderProgram = new crvkProgram();
    m_shaderProgram->Create( m_device );
    m_shaderProgram->AttachShader( vertexShader );
    m_shaderProgram->AttachShader( fragmentShader );
    if ( m_shaderProgram->LinkProgram() )
    {
        delete vertexShader;
        delete fragmentShader;
        throw std::runtime_error("failed to compile the shader!");
    }
    
    // release shaders 
    delete vertexShader;
    delete fragmentShader;
#else
    size_t legenth[2] = { 0, 0 };
    uint32_t* spirVSources[2] = { nullptr, nullptr };
    VkShaderStageFlagBits shaderStages[2] = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };

    /*============ vertex shader ============*/
    // Open and compile vertx shader
    sourceFile = fopen( "shaders/test_shader_vert.spv", "r" );
    if ( sourceFile == nullptr ) 
        throw std::runtime_error("failed to open file!");
    
    legenth[0] = fSize( sourceFile );
   
    spirVSources[0] = static_cast<uint32_t*>( SDL_malloc( legenth[0] ) );
    std::memset( spirVSources[0], 0x00, legenth[0] );

    // read source content
    fread( spirVSources[0], 1, legenth[0], sourceFile ); 

    // release the file 
    fclose( sourceFile );
    
    /*============ frament shader ============*/
    // Open and compile frament
    sourceFile = fopen( "shaders/test_shader_frag.spv", "r" );
    if ( sourceFile == nullptr ) 
        throw std::runtime_error("failed to open file!");
    
    legenth[1] = fSize( sourceFile );
   
    spirVSources[1] = static_cast<uint32_t*>( SDL_malloc( legenth[1] ) );
    std::memset( spirVSources[1], 0x00, legenth[1] );

    // read source content
    fread( spirVSources[1], 1, legenth[1], sourceFile ); 

    // release the file 
    fclose( sourceFile );
    
    m_shaderProgram = new crvkSpirVProgram();
    if( !dynamic_cast<crvkSpirVProgram*>( m_shaderProgram )->Create( m_device, shaderStages, spirVSources, legenth, 2 ) ) 
        throw std::runtime_error( "can't compile shaders\n" );
#endif
}

void crvkTest::InitPipeline( const uint32_t in_samples )
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof( crVertex );
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    VkVertexInputAttributeDescription attributeDescriptions[3]{{}, {}, {}};
    // vertex position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof( crVertex, pos);

    // texture coordinate 
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof( crVertex, pos);

    // vertex color 
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof( crVertex, color);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 3;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineTessellationStateCreateInfo pipelineTessellationStateCI{};

    VkPipelineViewportStateCreateInfo viewportStateCI{};
    viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCI.viewportCount = 1;
    viewportStateCI.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
    rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCI.depthClampEnable = VK_FALSE;
    rasterizationStateCI.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCI.lineWidth = 1.0f;
    rasterizationStateCI.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCI.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
    multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCI.sampleShadingEnable = in_samples > 0 ? VK_TRUE : VK_FALSE;
    
    //
    if( in_samples < 2 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    else if( in_samples < 4 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT;
    else if( in_samples < 8 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
    else if( in_samples < 16 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT;
    else if( in_samples < 32 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_16_BIT;
    else if( in_samples < 64 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_32_BIT;
    else if( in_samples > 64 )
        multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_64_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
    colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCI.logicOpEnable = VK_FALSE;
    colorBlendStateCI.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCI.attachmentCount = 1;
    colorBlendStateCI.pAttachments = &colorBlendAttachment;
    colorBlendStateCI.blendConstants[0] = 0.0f;
    colorBlendStateCI.blendConstants[1] = 0.0f;
    colorBlendStateCI.blendConstants[2] = 0.0f;
    colorBlendStateCI.blendConstants[3] = 0.0f;
    
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    VkDynamicState dynamicStates[11] = 
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_DEPTH_BIAS,
        VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
        VK_DYNAMIC_STATE_CULL_MODE,
        VK_DYNAMIC_STATE_FRONT_FACE,
        VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
        VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
        VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE
    };
    
    dynamicState.dynamicStateCount = 11;
    dynamicState.pDynamicStates = dynamicStates;

    m_pipeline = new crvkGraphicPipeline();
    if( m_pipeline->Create( 
            m_device,   
            3,
            1 
    ) )
    {
        throw std::runtime_error("failed to create pipeline!");
    }
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
    if ( m_shaderProgram != nullptr )
    {
        delete m_shaderProgram;
        m_shaderProgram = nullptr; 
    }
    
    if ( m_cmd != nullptr )
    {
        m_cmd->Destroy();
        delete m_cmd;
        m_cmd = nullptr;
    }
    

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