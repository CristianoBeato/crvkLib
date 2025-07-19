// ===============================================================================================
// crvkContext - Vulkan + SDL minimal framework
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

#include "crvkImplement.hpp"
#include "crvkBuffer.hpp"

#include <cstring>
#include <stdexcept>

/*
==============================================
crvkBufferStatic::crvkBufferStatic
==============================================
*/
crvkBufferStatic::crvkBufferStatic( void ) : m_device( nullptr), m_buffer( nullptr ), m_memory( nullptr ) 
{
}

/*
==============================================
crvkBufferStatic::~crvkBufferStatic
==============================================
*/
crvkBufferStatic::~crvkBufferStatic( void )
{
    Destroy();
}

/*
==============================================
crvkBufferStatic::Create
==============================================
*/
bool crvkBufferStatic::Create( const crvkContext *in_contex, const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult result = VK_SUCCESS;
    m_device = const_cast<crvkDevice*>( in_device );

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = in_size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer( in_device->Device(), &bufferInfo, &k_allocationCallbacks, &m_buffer );
    if ( result != VK_SUCCESS) 
    {
        in_contex->AppendError( "failed to create vertex !" );
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( in_device->Device(), m_buffer, &memRequirements );

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = in_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    result = vkAllocateMemory( in_device->Device(), &allocInfo, &k_allocationCallbacks, &m_memory );
    if ( result != VK_SUCCESS ) 
    {
        in_contex->AppendError("failed to allocate vertex buffer memory!");
        return false;
    }

    result = vkBindBufferMemory( in_device->Device(), m_buffer, m_memory, 0 );
    if ( result != VK_SUCCESS )
        return false;
    
    return true;
}

/*
==============================================
crvkBufferStatic::Destroy
==============================================
*/
void crvkBufferStatic::Destroy( void )
{
    if ( m_memory != nullptr )
    {
        vkFreeMemory( m_device->Device(), m_memory, &k_allocationCallbacks );
        m_memory = nullptr;
    }

    if ( m_buffer != nullptr )
    {
        vkDestroyBuffer( m_device->Device(), m_buffer, &k_allocationCallbacks );
        m_buffer = nullptr;
    }
    
    m_device = nullptr;
}

/*
==============================================
crvkBufferStatic::SubData
==============================================
*/
void crvkBufferStatic::SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size )
{
    void* poiter = nullptr;
    vkMapMemory( m_device->Device(), m_memory, in_offset, in_size, 0, &poiter );
    std::memcpy( poiter, in_data, in_size );
    vkUnmapMemory( m_device->Device(), m_memory );
}

/*
==============================================
crvkBufferStatic::GetSubData
==============================================
*/
void crvkBufferStatic::GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size )
{
    void* poiter = nullptr;
    vkMapMemory( m_device->Device(), m_memory, in_offset, in_size, 0, &poiter );
    std::memcpy( in_data, poiter, in_size );
    vkUnmapMemory( m_device->Device(), m_memory );
}

/*
==============================================
crvkBufferStatic::Map
==============================================
*/
void* crvkBufferStatic::Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags )
{
    void* poiter = nullptr;
    vkMapMemory( m_device->Device(), m_memory, in_offset, in_size, 0, &poiter );
    return poiter;    
}

/*
==============================================
crvkBufferStatic::Unmap
==============================================
*/
void crvkBufferStatic::Unmap( void )
{
    vkUnmapMemory( m_device->Device(), m_memory );
}

/*
==============================================
crvkBufferStatic::Flush
==============================================
*/
void crvkBufferStatic::Flush( const uintptr_t in_offset, const size_t in_size )
{
}

/*
==============================================
crvkBufferStatic::Handle
==============================================
*/
VkBuffer crvkBufferStatic::Handle( void ) const
{
    return m_buffer;
}

/*
==============================================
crvkBufferStaging::crvkBufferStaging
==============================================
*/
crvkBufferStaging::crvkBufferStaging( void ) :
    m_flags( 0 ),
    m_device( nullptr ),
    m_gpuBuffer( nullptr ),
    m_cpuBuffer( nullptr ),
    m_gpuBufferMemory( nullptr ),
    m_cpuBufferMemory( nullptr ),
    m_commandBuffer( nullptr )
{
}

/*
==============================================
crvkBufferStaging::~crvkBufferStaging
==============================================
*/
crvkBufferStaging::~crvkBufferStaging( void )
{
    Destroy();
}

/*
==============================================
crvkBufferStaging::Create
==============================================
*/
bool crvkBufferStaging::Create( const crvkContext* in_contex, const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult res = VK_SUCCESS;
    VkMemoryRequirements memRequirements;
    
    m_device = const_cast<crvkDevice*>( in_device );

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = in_size;

    VkMemoryAllocateInfo memoryAllocI{};
    memoryAllocI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    //
    // Create the GPU side buffer 
    // ==========================================================================
    bufferInfo.usage = in_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    res = vkCreateBuffer( in_device->Device(), &bufferInfo, &k_allocationCallbacks, &m_gpuBuffer );
    if ( res != VK_SUCCESS )
    {
        in_contex->AppendError( "failed to create GPU buffer!" );
        return false; 
    }

    vkGetBufferMemoryRequirements( m_device->Device(), m_gpuBuffer, &memRequirements );

    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    res = vkAllocateMemory( m_device->Device(), &memoryAllocI, &k_allocationCallbacks, &m_gpuBufferMemory ); 
    if ( res != VK_SUCCESS )
    {
        in_contex->AppendError( "failed to allocate GPU buffer memory!" );
        return false;
    }

    // bind buffer handler to the memory
    vkBindBufferMemory( m_device->Device(), m_gpuBuffer, m_gpuBufferMemory, 0);

    //
    // Create the CPU side buffer 
    // ==========================================================================
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    res = vkCreateBuffer( m_device->Device(), &bufferInfo, &k_allocationCallbacks, &m_cpuBuffer );
    if ( res != VK_SUCCESS ) 
    {
        in_contex->AppendError( "failed to create CPU buffer!" );
        return false; 
    }
    vkGetBufferMemoryRequirements( m_device->Device(), m_cpuBuffer, &memRequirements );

    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

    res = vkAllocateMemory( m_device->Device(), &memoryAllocI, &k_allocationCallbacks, &m_cpuBufferMemory ); 
    if ( res != VK_SUCCESS ) 
    {
        in_contex->AppendError( "failed to allocate CPU buffer memory!" );
        return false;
    }

    // bind buffer handler to the memory
    vkBindBufferMemory( m_device->Device(), m_cpuBuffer, m_cpuBufferMemory, 0);

    ///
    /// Create the Command buffer to record the buffer transfer operations
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_device->CommandPool();
    allocInfo.commandBufferCount = 1;

    res = vkAllocateCommandBuffers( m_device->Device(), &allocInfo, &m_commandBuffer );
    if ( res != VK_SUCCESS )
        return false;

    return true;
}
    
/*
==============================================
crvkBufferStaging::Destroy
==============================================
*/
void crvkBufferStaging::Destroy( void )
{
    // release the buffer operation command buffer 
    if ( m_commandBuffer != nullptr )
    {
        vkFreeCommandBuffers( m_device->Device(), m_device->CommandPool(), 1, &m_commandBuffer );
        m_commandBuffer = nullptr;
    }

    // release CPU side buffer memory 
    if ( m_cpuBufferMemory != nullptr )
    {
        vkFreeMemory( m_device->Device(), m_cpuBufferMemory, &k_allocationCallbacks );
        m_cpuBufferMemory = nullptr; 
    }
    
    // release the CPU side buffer handler 
    if ( m_cpuBuffer != nullptr )
    {
        vkDestroyBuffer( m_device->Device(), m_cpuBuffer, &k_allocationCallbacks );
        m_cpuBuffer = nullptr;
    }
 
    // release GPU side buffer memory 
    if ( m_gpuBufferMemory != nullptr )
    {
        vkFreeMemory( m_device->Device(), m_gpuBufferMemory, &k_allocationCallbacks );
        m_gpuBufferMemory = nullptr; 
    }
    
    // release GPU side buffer handler 
    if ( m_gpuBuffer != nullptr )
    {
        vkDestroyBuffer( m_device->Device(), m_gpuBuffer, &k_allocationCallbacks );
        m_gpuBuffer = nullptr;
    }    
}

/*
==============================================
crvkBufferStaging::SubData
==============================================
*/
void crvkBufferStaging::SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size )
{
    void* data = nullptr;

    // copy data to our CPU buffer 
    vkMapMemory( m_device->Device(), m_cpuBufferMemory, in_offset, in_size, 0, &data);
    std::memcpy( data, in_data, in_size );
    vkUnmapMemory( m_device->Device(), m_cpuBufferMemory );

    // begin copy content to GPU     
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_device->CommandPool();
    allocInfo.commandBufferCount = 1;

    // create a copy command buffer
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers( m_device->Device(), &allocInfo, &commandBuffer );

    // 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer( commandBuffer, &beginInfo );

    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;

    vkCmdCopyBuffer( commandBuffer, m_cpuBuffer, m_gpuBuffer, 1, &copyRegion );

    // End buffer recording 
    vkEndCommandBuffer( commandBuffer );

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
#if 0
    // submint the copy command to the device queue 
    if ( m_contex->HasTransferQueue() )
        vkQueueSubmit( m_contex->GetTransferQueue(), 1, &submitInfo, VK_NULL_HANDLE );
        // vkQueueWaitIdle( m_contex->GetTransferQueue() );
    else
        vkQueueSubmit( m_contex->GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE );
        // vkQueueWaitIdle( m_contex->GetGraphicQueue() );
#else
    vkQueueSubmit( m_device->GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE );
    vkQueueWaitIdle( m_device->GraphicsQueue() );
#endif
}

/*
==============================================
crvkBufferStaging::GetSubData
==============================================
*/
void crvkBufferStaging::GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size )
{
    void* data = nullptr;
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer( m_commandBuffer, &beginInfo );

    // copy content from GPU buffer to the CPU buffer 
    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;

    // register the copy command 
    vkCmdCopyBuffer( m_commandBuffer, m_gpuBuffer, m_cpuBuffer, 1, &copyRegion );
    
    vkEndCommandBuffer( m_commandBuffer );

    // submit copy, and wait for copy end 
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    
#if 0
    if ( m_contex->HasTransferQueue() )
        vkQueueSubmit( m_contex->GetTransferQueue(), 1, &submitInfo, VK_NULL_HANDLE );
        //vkQueueWaitIdle( m_contex->GetTransferQueue() );
    else
#else
    vkQueueSubmit( m_device->GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE );
    vkQueueWaitIdle( m_device->GraphicsQueue() );
#endif

    // copy the content of the CPU buffer to the data pointer
    vkMapMemory( m_device->Device(), m_cpuBufferMemory, in_offset, in_size, 0, &data );
    std::memcpy( in_data, data, in_size );
    vkUnmapMemory( m_device->Device(), m_cpuBufferMemory );
}

/*
==============================================
crvkBufferStaging::Map
==============================================
*/
void* crvkBufferStaging::Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags )
{
    m_flags = in_flags;
    
    VkResult res = VK_SUCCESS;
    VkDeviceSize bufferSize = VK_WHOLE_SIZE;
    void* map = nullptr;

    if ( in_size != 0 )
        bufferSize = in_size;

    res = vkMapMemory( m_device->Device(), m_cpuBufferMemory, in_offset, bufferSize, 0, &map );
    if ( res != VK_SUCCESS )
    {
        throw std::runtime_error( "Map Error" );
    }
    return map;
}

/*
==============================================
crvkBufferStaging::Unmap
==============================================
*/
void crvkBufferStaging::Unmap( void )
{
    // release buffer memory map 
    vkUnmapMemory( m_device->Device(), m_cpuBufferMemory );
}

/*
==============================================
crvkBufferStaging::Flush
==============================================
*/
void crvkBufferStaging::Flush( const uintptr_t in_offset, const size_t in_size )
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer( m_commandBuffer, &beginInfo );

    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;

    // perform a copy from buffer 
    if ( m_flags & CRVK_ACCESS_BUFFER_WRITE ) // copy content from 
        vkCmdCopyBuffer( m_commandBuffer, m_cpuBuffer, m_gpuBuffer, 1, &copyRegion );
    else if ( m_flags & CRVK_ACCESS_BUFFER_READ )
        vkCmdCopyBuffer( m_commandBuffer, m_gpuBuffer, m_cpuBuffer, 1, &copyRegion );

    vkEndCommandBuffer( m_commandBuffer );

    /// submit buffer operations
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;
    
#if 0
    if ( m_contex->HasTransferQueue() )
        vkQueueSubmit( m_contex->GetTransferQueue(), 1, &submitInfo, VK_NULL_HANDLE );
    else
        vkQueueSubmit( m_contex->GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE );
#else
    vkQueueSubmit( m_device->GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE );
    vkQueueWaitIdle( m_device->GraphicsQueue() );
#endif
}

/*
==============================================
crvkBufferStaging::Handle
==============================================
*/
VkBuffer crvkBufferStaging::Handle( void ) const
{
    return m_cpuBuffer;
}
