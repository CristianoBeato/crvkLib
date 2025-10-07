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

#include <glslang/Include/glslang_c_interface.h>

#include "crvkPrecompiled.hpp"
#include "crvkDevice.hpp"

typedef struct crvkDeviceHandle_t
{
    VkPhysicalDeviceProperties2                     propertiesv10;
    VkPhysicalDeviceVulkan11Properties              propertiesv11;
    VkPhysicalDeviceVulkan12Properties              propertiesv12;
    VkPhysicalDeviceVulkan13Properties              propertiesv13;
    VkPhysicalDeviceTransformFeedbackPropertiesEXT  propertiesTransformFeedback;
    VkPhysicalDeviceFeatures2                       featuresv10;
    VkPhysicalDeviceVulkan12Features                featuresv11;
    VkPhysicalDeviceVulkan12Features                featuresv12;
    VkPhysicalDeviceVulkan13Features                featuresv13;
    VkPhysicalDeviceTransformFeedbackFeaturesEXT    featuresTransformFeedback;
    VkSurfaceCapabilities2KHR                       surfaceCapabilities;
    VkPhysicalDeviceMemoryProperties2               memoryProperties;
    crvkDynamicVector<VkSurfaceFormat2KHR>          surfaceFormats;
    crvkDynamicVector<VkExtensionProperties>        extensions;
    crvkDynamicVector<VkPresentModeKHR>             presentModes;
    crvkDynamicVector<VkQueueFamilyProperties2>     queueFamilies;
    crvkDynamicVector<crvkQueueInfo_t>              queuesList;
    glslang_resource_t*                             shaderBuiltInResources = nullptr;
    VkPhysicalDevice                                physicalDevice = nullptr;
    VkDevice                                        logicalDevice = nullptr;
} crvkDeviceHandle_t;


/*
==============================================
crvkDevice::crvkDevice
==============================================
*/
crvkDevice::crvkDevice( void ) : m_handle( nullptr )
{
}

/*
==============================================
crvkDevice::~crvkDevice
==============================================
*/
crvkDevice::~crvkDevice( void )
{   
    Destroy();
}

/*
==============================================
crvkDevice::Create
==============================================
*/
bool crvkDevice::Create(const char **in_layers, const uint32_t in_layersCount, const char **in_deviceExtensions, const uint32_t in_deviceExtensionsCount)
{
    VkResult result = VK_SUCCESS;
    crvkDynamicVector<VkDeviceQueueCreateInfo> queueCreateInfos;

    VkDeviceCreateInfo deviceCI{};
    deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    
#if 0
    FindQueues( queueCreateInfos );
#endif

    InitializeBuiltInShaderResources();

    
    deviceCI.queueCreateInfoCount = queueCreateInfos.Count();
    deviceCI.pQueueCreateInfos = &queueCreateInfos;
    
    // configure device features
    // concatenate device features initialization 
    m_handle->featuresv13.pNext = &m_handle->featuresTransformFeedback; // initialize transform feedback features
    m_handle->featuresv12.pNext = &m_handle->featuresv13;   // initialize vulkan 1.3 device features
    m_handle->featuresv11.pNext = &m_handle->featuresv12;   // initialize vulkan 1.2 device features 
    m_handle->featuresv10.pNext = &m_handle->featuresv11;   // initialize vulkan 1.1 device features 
    deviceCI.pNext = &m_handle->featuresv10;        // initialize vulkan 1.0 device features 

    deviceCI.enabledExtensionCount = in_deviceExtensionsCount;
    deviceCI.ppEnabledExtensionNames = const_cast<const char* const*>( in_deviceExtensions );

    if ( in_layersCount > 0 ) 
    {
        deviceCI.enabledLayerCount = in_layersCount;
        deviceCI.ppEnabledLayerNames = in_layers;
    } 
    else 
    {
        deviceCI.enabledLayerCount = 0;
        deviceCI.ppEnabledLayerNames = nullptr;
    }
    
    result = vkCreateDevice( m_handle->physicalDevice, &deviceCI, k_allocationCallbacks, &m_handle->logicalDevice );
    if ( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkDevice::Create::vkCreateDevice", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkDevice::Destroy
==============================================
*/
void crvkDevice::Destroy(void)
{
    if ( m_handle == nullptr )
        return;    
    
    if ( m_handle->logicalDevice != nullptr )
    {
        vkDestroyDevice( m_handle->logicalDevice, k_allocationCallbacks );
        m_handle->logicalDevice = nullptr;
    }

    m_handle->surfaceFormats.Clear();
    m_handle->extensions.Clear();
    m_handle->presentModes.Clear();
    m_handle->queueFamilies.Clear();

    SDL_free( m_handle );
    m_handle = nullptr;
}

/*
==============================================
crvkDevice::FindExtent
==============================================
*/
VkExtent2D crvkDevice::FindExtent( const uint32_t in_width, const uint32_t in_height ) const
{
    auto capabilities = m_handle->surfaceCapabilities.surfaceCapabilities;

    VkExtent2D actualExtent = { in_width, in_height };

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return capabilities.currentExtent;
    } 
    else 
    {
        actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
    }
    
    return actualExtent;
}

/*
==============================================
crvkDevice::FindMemoryType
==============================================
*/
uint32_t crvkDevice::FindMemoryType( const uint32_t typeFilter, const VkMemoryPropertyFlags properties ) const
{
    if( m_handle == nullptr )
        return UINT32_MAX;

    auto memoryProperties = m_handle->memoryProperties.memoryProperties;

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) 
    {
        if ((typeFilter & (1 << i)) && ( memoryProperties.memoryTypes[i].propertyFlags & properties ) == properties) 
                return i;
    }

    // todo: append error
    return UINT32_MAX;
}

/*
==============================================
crvkDevice::PhysicalDevice
==============================================
*/
VkPhysicalDevice crvkDevice::PhysicalDevice( void ) const
{
    if ( m_handle == nullptr )
        return nullptr;
    
    return m_handle->physicalDevice;
}

/*
==============================================
crvkDevice::Device
==============================================
*/
VkDevice crvkDevice::Device(void) const
{
    if ( m_handle == nullptr )
        return nullptr;
    
    return m_handle->logicalDevice;
}

crvkQueueInfo_t *crvkDevice::GetQueueInfo( uint32_t* in_count ) const
{
    if ( in_count != nullptr )
        *in_count = m_handle->queuesList.Count();
    
    return m_handle->queuesList.Pointer();
}

/*
==============================================
crvkDevice::SurfaceCapabilities
==============================================
*/
VkSurfaceCapabilitiesKHR crvkDevice::SurfaceCapabilities(void) const
{
    return m_handle->surfaceCapabilities.surfaceCapabilities;
}

uint32_t crvkDevice::FindBestImageCount(const uint32_t in_frames)
{
    uint32_t maxImageCount = 0;
    uint32_t minImageCount = 0;

    if ( m_handle != nullptr )
    {
        minImageCount = m_handle->surfaceCapabilities.surfaceCapabilities.maxImageCount;
        maxImageCount = m_handle->surfaceCapabilities.surfaceCapabilities.maxImageCount;
    }
    
    // some drivers don't define max image count, so we do a bypass
    if ( maxImageCount == 0 )
        return std::max( in_frames, minImageCount );

    return std::clamp( in_frames, minImageCount, maxImageCount );
}

/*
==============================================
crvkDevice::GetSuportedPresentModes
==============================================
*/
const VkSurfaceFormat2KHR *crvkDevice::GetSuportedSurfaceFormat( uint32_t *in_count ) const
{
    if( m_handle == nullptr )
    {
        // set the count to zero
        if( in_count != nullptr )
            *in_count = 0;

        return nullptr;
    }

    // retrieve the formats count
    if ( in_count != nullptr )
        *in_count = m_handle->surfaceFormats.Count();

    // 
    return &m_handle->surfaceFormats;
}

/*
==============================================
crvkDevice::GetSuportedPresentModes
==============================================
*/
const VkPresentModeKHR *crvkDevice::GetSuportedPresentModes( uint32_t *in_count ) const
{
    if( m_handle == nullptr )
    {
        // set the count to zero
        if( in_count != nullptr )
            *in_count = 0;

        return nullptr;
    }

    // retrieve the formats count
    if ( in_count != nullptr )
        *in_count = m_handle->presentModes.Count();

    // 
    return &m_handle->presentModes;
}

/*
==============================================
crvkDevice::CheckExtensionSupport
==============================================
*/
const bool crvkDevice::CheckExtensionSupport( const char *in_extension )
{
    bool found = false;

    for ( uint32_t i = 0; i < m_handle->extensions.Count(); i++)
    {   
        if ( std::strcmp( in_extension, m_handle->extensions[i].extensionName ) == 0 )
        {
            found = true;
            break;
        }
    }
    
    // TODO: print in verbose mode the needed 
    return found;
}

/*
==============================================
crvkDevice::BuiltInShaderResource
==============================================
*/
const glslang_resource_t* crvkDevice::BuiltInShaderResource( void ) const
{
    return m_handle->shaderBuiltInResources;
}

/*
==============================================
crvkDevice::InitDevice
==============================================
*/
bool crvkDevice::InitDevice( const crvkContext* in_context, const VkPhysicalDevice in_device )
{
    VkResult result = VK_SUCCESS; 
    uint32_t extensionCount = 0;

    std::memset( &m_handle->surfaceCapabilities, 0x00, sizeof( VkSurfaceCapabilities2KHR ) );
 
    m_handle->physicalDevice = in_device;
    SDL_assert( m_handle->physicalDevice != nullptr );

    VkPhysicalDeviceSurfaceInfo2KHR deviceSurfaceInfo{}; 
    deviceSurfaceInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    deviceSurfaceInfo.surface = in_context->Surface();
    deviceSurfaceInfo.pNext = nullptr;

    //
    AquireDeviceProperties();
  
    if( !AquireDeviceSurfaceProperties( deviceSurfaceInfo ) )
        return false;   

    if( !AquireDeviceFeatures( deviceSurfaceInfo ) )
        return false;

    // query device extensions
    result = vkEnumerateDeviceExtensionProperties( m_handle->physicalDevice, nullptr, &extensionCount, nullptr );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::InitDevice::vkEnumerateDeviceExtensionProperties", result );
        return false;
    }

    m_handle->extensions.Resize( extensionCount );
    m_handle->extensions.Memset( 0x00 );
    result = vkEnumerateDeviceExtensionProperties( m_handle->physicalDevice, nullptr, &extensionCount, &m_handle->extensions );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::InitDevice::vkEnumerateDeviceExtensionProperties", result );
        return false;
    }
    
    if( !AquireDeviceQueues( deviceSurfaceInfo ) )
        return false;

#if VK_KHR_copy_commands2
    m_deviceSuportedFeatures.copyCommands2Enabled = CheckExtensionSupport( VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME );
#endif 

    return true;
}

#define HAS(feature) ( feature == VK_TRUE)
#define COMPONENTS_TO_VECTORS(x) ((x) / 4)

void crvkDevice::InitializeBuiltInShaderResources(void)
{
    m_handle->shaderBuiltInResources = new glslang_resource_t();
    
    auto properties10 = m_handle->propertiesv10.properties;
    auto featuresv10 = m_handle->featuresv10.features;
    
    m_handle->shaderBuiltInResources->max_lights = 32; // irrelevante no Vulkan, mas ainda exigido
    m_handle->shaderBuiltInResources->max_clip_planes = properties10.limits.maxClipDistances;
    m_handle->shaderBuiltInResources->max_clip_distances = properties10.limits.maxClipDistances;
    m_handle->shaderBuiltInResources->max_texture_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_texture_coords = properties10.limits.maxPerStageDescriptorSamplers;
    m_handle->shaderBuiltInResources->max_vertex_attribs = properties10.limits.maxVertexInputAttributes;
    m_handle->shaderBuiltInResources->max_vertex_uniform_components = COMPONENTS_TO_VECTORS( properties10.limits.maxUniformBufferRange );
    m_handle->shaderBuiltInResources->max_varying_floats = COMPONENTS_TO_VECTORS( properties10.limits.maxVertexOutputComponents );
    m_handle->shaderBuiltInResources->max_vertex_texture_image_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_combined_texture_image_units = properties10.limits.maxDescriptorSetSampledImages;
    m_handle->shaderBuiltInResources->max_texture_image_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_fragment_uniform_components = COMPONENTS_TO_VECTORS( properties10.limits.maxUniformBufferRange );
    m_handle->shaderBuiltInResources->max_draw_buffers = properties10.limits.maxColorAttachments;
    m_handle->shaderBuiltInResources->max_vertex_uniform_vectors = properties10.limits.maxUniformBufferRange / 16;
    m_handle->shaderBuiltInResources->max_fragment_uniform_vectors = properties10.limits.maxUniformBufferRange / 16;
    m_handle->shaderBuiltInResources->max_varying_vectors = properties10.limits.maxUniformBufferRange / 16;
    m_handle->shaderBuiltInResources->max_vertex_output_vectors = properties10.limits.maxUniformBufferRange / 16;
    m_handle->shaderBuiltInResources->max_fragment_input_vectors = properties10.limits.maxUniformBufferRange / 16;
    m_handle->shaderBuiltInResources->min_program_texel_offset = properties10.limits.minTexelOffset;
    m_handle->shaderBuiltInResources->max_program_texel_offset = properties10.limits.maxTexelOffset;
    m_handle->shaderBuiltInResources->max_compute_work_group_count_x = properties10.limits.maxComputeWorkGroupCount[0];
    m_handle->shaderBuiltInResources->max_compute_work_group_count_y = properties10.limits.maxComputeWorkGroupCount[1];
    m_handle->shaderBuiltInResources->max_compute_work_group_count_z = properties10.limits.maxComputeWorkGroupCount[2];
    m_handle->shaderBuiltInResources->max_compute_work_group_size_x = properties10.limits.maxComputeWorkGroupSize[0];
    m_handle->shaderBuiltInResources->max_compute_work_group_size_y = properties10.limits.maxComputeWorkGroupSize[1];
    m_handle->shaderBuiltInResources->max_compute_work_group_size_z = properties10.limits.maxComputeWorkGroupSize[2];
    m_handle->shaderBuiltInResources->max_compute_uniform_components = properties10.limits.maxUniformBufferRange / 4;
    m_handle->shaderBuiltInResources->max_compute_texture_image_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_compute_image_uniforms = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_compute_atomic_counters = 8;
    m_handle->shaderBuiltInResources->max_compute_atomic_counter_buffers = 1;
    m_handle->shaderBuiltInResources->max_varying_components = properties10.limits.maxVertexOutputComponents;
    m_handle->shaderBuiltInResources->max_vertex_output_components = properties10.limits.maxVertexOutputComponents;
    m_handle->shaderBuiltInResources->max_geometry_input_components = properties10.limits.maxGeometryInputComponents;
    m_handle->shaderBuiltInResources->max_geometry_output_components = properties10.limits.maxGeometryOutputComponents;
    m_handle->shaderBuiltInResources->max_fragment_input_components = properties10.limits.maxFragmentInputComponents;
    m_handle->shaderBuiltInResources->max_image_units = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_combined_image_units_and_fragment_outputs = properties10.limits.maxColorAttachments + properties10.limits.maxDescriptorSetSampledImages;
    m_handle->shaderBuiltInResources->max_vertex_image_uniforms = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_tess_control_image_uniforms = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_tess_evaluation_image_uniforms = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_geometry_image_uniforms = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_fragment_image_uniforms = properties10.limits.maxPerStageDescriptorStorageImages;
    m_handle->shaderBuiltInResources->max_combined_image_uniforms = properties10.limits.maxDescriptorSetStorageImages;
    m_handle->shaderBuiltInResources->max_geometry_texture_image_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_geometry_output_vertices = properties10.limits.maxGeometryOutputVertices;
    m_handle->shaderBuiltInResources->max_geometry_total_output_components = properties10.limits.maxGeometryTotalOutputComponents;
    m_handle->shaderBuiltInResources->max_geometry_uniform_components = properties10.limits.maxUniformBufferRange / 4;
    m_handle->shaderBuiltInResources->max_geometry_varying_components = properties10.limits.maxVertexOutputComponents;
    m_handle->shaderBuiltInResources->max_tess_control_input_components = properties10.limits.maxTessellationControlPerVertexInputComponents;
    m_handle->shaderBuiltInResources->max_tess_control_output_components = properties10.limits.maxTessellationControlPerVertexOutputComponents;
    m_handle->shaderBuiltInResources->max_tess_control_texture_image_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_tess_control_uniform_components = properties10.limits.maxUniformBufferRange / 4;
    m_handle->shaderBuiltInResources->max_tess_control_total_output_components = properties10.limits.maxTessellationControlTotalOutputComponents;
    m_handle->shaderBuiltInResources->max_tess_evaluation_input_components = properties10.limits.maxTessellationEvaluationInputComponents;
    m_handle->shaderBuiltInResources->max_tess_evaluation_output_components = properties10.limits.maxTessellationEvaluationOutputComponents;
    m_handle->shaderBuiltInResources->max_tess_evaluation_texture_image_units = properties10.limits.maxPerStageDescriptorSampledImages;
    m_handle->shaderBuiltInResources->max_tess_evaluation_uniform_components = COMPONENTS_TO_VECTORS( properties10.limits.maxUniformBufferRange );
    m_handle->shaderBuiltInResources->max_tess_patch_components = properties10.limits.maxTessellationPatchSize;
    m_handle->shaderBuiltInResources->max_patch_vertices = properties10.limits.maxTessellationPatchSize;
    m_handle->shaderBuiltInResources->max_tess_gen_level = properties10.limits.maxTessellationGenerationLevel;
    m_handle->shaderBuiltInResources->max_viewports = properties10.limits.maxViewports;
    m_handle->shaderBuiltInResources->max_vertex_atomic_counters = 0;
    m_handle->shaderBuiltInResources->max_tess_control_atomic_counters = 0;
    m_handle->shaderBuiltInResources->max_tess_evaluation_atomic_counters = 0;
    m_handle->shaderBuiltInResources->max_geometry_atomic_counters = 0;
    m_handle->shaderBuiltInResources->max_fragment_atomic_counters = 8;
    m_handle->shaderBuiltInResources->max_combined_atomic_counters = 8;
    m_handle->shaderBuiltInResources->max_atomic_counter_bindings = 1;
    m_handle->shaderBuiltInResources->max_vertex_atomic_counter_buffers = 0;
    m_handle->shaderBuiltInResources->max_tess_control_atomic_counter_buffers = 0;
    m_handle->shaderBuiltInResources->max_tess_evaluation_atomic_counter_buffers = 0;
    m_handle->shaderBuiltInResources->max_geometry_atomic_counter_buffers = 0;
    m_handle->shaderBuiltInResources->max_fragment_atomic_counter_buffers = 1;
    m_handle->shaderBuiltInResources->max_combined_atomic_counter_buffers = 1;
    m_handle->shaderBuiltInResources->max_atomic_counter_buffer_size = properties10.limits.maxStorageBufferRange;
    m_handle->shaderBuiltInResources->max_transform_feedback_buffers = m_handle->propertiesTransformFeedback.maxTransformFeedbackBuffers;
    m_handle->shaderBuiltInResources->max_transform_feedback_interleaved_components = COMPONENTS_TO_VECTORS( m_handle->propertiesTransformFeedback.maxTransformFeedbackBufferDataSize );
    m_handle->shaderBuiltInResources->max_cull_distances = properties10.limits.maxCullDistances;
    m_handle->shaderBuiltInResources->max_combined_clip_and_cull_distances = properties10.limits.maxCombinedClipAndCullDistances;
    
    m_handle->shaderBuiltInResources->max_combined_shader_output_resources =
    properties10.limits.maxPerStageDescriptorSamplers +
    properties10.limits.maxPerStageDescriptorUniformBuffers +
    properties10.limits.maxPerStageDescriptorStorageBuffers +
    properties10.limits.maxPerStageDescriptorSampledImages +
    properties10.limits.maxPerStageDescriptorStorageImages +
    properties10.limits.maxPerStageDescriptorInputAttachments;
    
    if ( properties10.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_64_BIT ) 
        m_handle->shaderBuiltInResources->max_image_samples = 64;
    else if ( properties10.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_32_BIT ) 
        m_handle->shaderBuiltInResources->max_image_samples = 32;
    else if ( properties10.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_16_BIT ) 
        m_handle->shaderBuiltInResources->max_image_samples = 16;
    else if ( properties10.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_8_BIT ) 
        m_handle->shaderBuiltInResources->max_image_samples = 8;
    else if ( properties10.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT ) 
        m_handle->shaderBuiltInResources->max_image_samples = 4;
    else if ( properties10.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_2_BIT ) 
        m_handle->shaderBuiltInResources->max_image_samples = 2;

    m_handle->shaderBuiltInResources->max_samples = m_handle->shaderBuiltInResources->max_image_samples;

    glslang_limits_t limits;
    limits.non_inductive_for_loops = true; // There's no way to know via Vulkan. Suppose it's true.
    limits.while_loops = true; // No specific flag. Assumes true.
    limits.do_while_loops = true; // No specific flag. Assumes true..
    limits.general_attribute_matrix_vector_indexing = true; // No specific flag. Assumes true.
    limits.general_varying_indexing = true;               // No specific flag. Assumes true.
    limits.general_constant_matrix_vector_indexing = true;  // Assumes true, generally supported.

    // Dynamic indexing — some of these are directly mappable:
    limits.general_uniform_indexing = HAS( featuresv10.shaderUniformBufferArrayDynamicIndexing );
    limits.general_sampler_indexing = HAS( featuresv10.shaderSampledImageArrayDynamicIndexing );
    limits.general_variable_indexing = HAS( featuresv10.shaderStorageBufferArrayDynamicIndexing );
    m_handle->shaderBuiltInResources->limits = limits;
}

void crvkDevice::AquireDeviceProperties(void)
{
    // clear from memory gargbage 
    std::memset( &m_handle->propertiesv10, 0x00, sizeof( VkPhysicalDeviceProperties2 ) );
    std::memset( &m_handle->propertiesv11, 0x00, sizeof( VkPhysicalDeviceVulkan11Properties ) );
    std::memset( &m_handle->propertiesv12, 0x00, sizeof( VkPhysicalDeviceVulkan12Properties ) );
    std::memset( &m_handle->propertiesv13, 0x00, sizeof( VkPhysicalDeviceVulkan13Properties ) );
    std::memset( &m_handle->propertiesTransformFeedback, 0x00, sizeof( VkPhysicalDeviceTransformFeedbackPropertiesEXT ) );
    std::memset( &m_handle->memoryProperties, 0x00, sizeof( VkPhysicalDeviceMemoryProperties2 ) );

    // query device properties
    m_handle->propertiesTransformFeedback.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;
    m_handle->propertiesTransformFeedback.pNext = nullptr;
    m_handle->propertiesv13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
    m_handle->propertiesv13.pNext = &m_handle->propertiesTransformFeedback;
    m_handle->propertiesv12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    m_handle->propertiesv12.pNext = &m_handle->propertiesv13;
    m_handle->propertiesv11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    m_handle->propertiesv11.pNext = &m_handle->propertiesv12;
    m_handle->propertiesv10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    m_handle->propertiesv10.pNext = &m_handle->propertiesv11;
    vkGetPhysicalDeviceProperties2( m_handle->physicalDevice, &m_handle->propertiesv10 );

    // query device memory properties
    m_handle->memoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    m_handle->memoryProperties.pNext = nullptr;
    vkGetPhysicalDeviceMemoryProperties2( m_handle->physicalDevice, &m_handle->memoryProperties );
}

bool crvkDevice::AquireDeviceSurfaceProperties( const VkPhysicalDeviceSurfaceInfo2KHR in_deviceSurfaceInfo )
{
    VkResult result = VK_SUCCESS;
    uint32_t formatCount = 0;
    uint32_t presentModeCount = 0;

    // query device suported surface formats
    result = vkGetPhysicalDeviceSurfaceFormats2KHR( m_handle->physicalDevice, &in_deviceSurfaceInfo, &formatCount, nullptr );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::AquireDeviceSurfaceProperties::vkGetPhysicalDeviceSurfaceFormats2KHR", result );
        return false;
    }

    m_handle->surfaceFormats.Resize( formatCount );
    m_handle->surfaceFormats.Memset( 0x00 );

    // you believe that vulkan require this ?
    for ( uint32_t i = 0; i < m_handle->surfaceFormats.Count(); i++)
    {
        m_handle->surfaceFormats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        m_handle->surfaceFormats[i].pNext = nullptr;
    }
    
    result = vkGetPhysicalDeviceSurfaceFormats2KHR( m_handle->physicalDevice, &in_deviceSurfaceInfo, &formatCount, &m_handle->surfaceFormats );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::AquireDeviceSurfaceProperties::vkGetPhysicalDeviceSurfaceFormats2KHR", result );
        return false;
    }

    // query device suported surface presenting mode
    result = vkGetPhysicalDeviceSurfacePresentModesKHR( m_handle->physicalDevice, in_deviceSurfaceInfo.surface, &presentModeCount, nullptr );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::AquireDeviceSurfaceProperties::vkGetPhysicalDeviceSurfacePresentModesKHR", result );
        return false;
    }

    m_handle->presentModes.Resize( presentModeCount );
    m_handle->presentModes.Memset( 0x00 );
    result = vkGetPhysicalDeviceSurfacePresentModesKHR( m_handle->physicalDevice, in_deviceSurfaceInfo.surface, &presentModeCount, &m_handle->presentModes );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::AquireDeviceSurfaceProperties::vkGetPhysicalDeviceSurfacePresentModesKHR", result );
        return false;
    }

    return true;
}

bool crvkDevice::AquireDeviceFeatures( const VkPhysicalDeviceSurfaceInfo2KHR in_deviceSurfaceInfo )
{
    VkResult result = VK_SUCCESS;
    
    // clear from memory gargbage
    std::memset( &m_handle->featuresv10, 0x00, sizeof( VkPhysicalDeviceFeatures2 ) );
    std::memset( &m_handle->featuresv11, 0x00, sizeof( VkPhysicalDeviceVulkan12Features ) );
    std::memset( &m_handle->featuresv12, 0x00, sizeof( VkPhysicalDeviceVulkan12Features ) );
    std::memset( &m_handle->featuresv13, 0x00, sizeof( VkPhysicalDeviceVulkan13Features ) );
    std::memset( &m_handle->featuresTransformFeedback, 0x00, sizeof( VkPhysicalDeviceTransformFeedbackFeaturesEXT ) );
    
    // query vulkan features
    m_handle->featuresTransformFeedback.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT;
    m_handle->featuresv13.pNext = nullptr;
    m_handle->featuresv13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    m_handle->featuresv13.pNext = &m_handle->featuresTransformFeedback;
    m_handle->featuresv12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    m_handle->featuresv12.pNext = &m_handle->featuresv13;
    m_handle->featuresv11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    m_handle->featuresv11.pNext = &m_handle->featuresv12;
    m_handle->featuresv10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    m_handle->featuresv10.pNext = &m_handle->featuresv11;   
    vkGetPhysicalDeviceFeatures2( m_handle->physicalDevice, &m_handle->featuresv10 );

    // query device surface  capabilities
    m_handle->surfaceCapabilities.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
    m_handle->surfaceCapabilities.pNext = nullptr;
    result = vkGetPhysicalDeviceSurfaceCapabilities2KHR( m_handle->physicalDevice, &in_deviceSurfaceInfo, &m_handle->surfaceCapabilities ); 
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkDevice::AquireDeviceFeatures::vkGetPhysicalDeviceSurfaceCapabilities2KHR", result );
        return false;
    }

    return true;
}

bool crvkDevice::AquireDeviceQueues( const VkPhysicalDeviceSurfaceInfo2KHR in_deviceSurfaceInfo )
{
    uint32_t queueFamilyCount = 0;
    
    // query device queues
    vkGetPhysicalDeviceQueueFamilyProperties2( m_handle->physicalDevice, &queueFamilyCount, nullptr );
    m_handle->queueFamilies.Resize( queueFamilyCount );
    m_handle->queueFamilies.Memset( 0x00 );
    vkGetPhysicalDeviceQueueFamilyProperties2( m_handle->physicalDevice, &queueFamilyCount, &m_handle->queueFamilies );

    for ( uint32_t i = 0; i < queueFamilyCount; i++)
    {
        VkBool32 presentSupport = VK_FALSE;
        crvkQueueInfo_t queue{};
        
        auto family = m_handle->queueFamilies[i].queueFamilyProperties;
        
        queue.family = i;

        queue.graphic = family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        queue.compute = family.queueFlags & VK_QUEUE_COMPUTE_BIT;
        queue.transfer = family.queueFlags & VK_QUEUE_TRANSFER_BIT;

        vkGetPhysicalDeviceSurfaceSupportKHR( m_handle->physicalDevice, i, in_deviceSurfaceInfo.surface, &presentSupport );
        queue.present = presentSupport == VK_TRUE;

        for ( uint32_t j = 0; j < family.queueCount; j++)
        {
            queue.index = j;
            m_handle->queuesList.Append( queue );
        }
    }

    return true;
}
