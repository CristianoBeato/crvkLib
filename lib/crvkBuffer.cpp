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

static const uint32_t k_CRVK_BUFFER_SEM_TYPE_FREE = 1;  // bufer is not in use
static const uint32_t k_CRVK_BUFFER_SEM_TYPE_COPY = 2;  // buffer is perfoming a copy 
static const uint32_t k_CRVK_BUFFER_SEM_TYPE_CLEAR = 3; // buffer is cleanig 

/*
==============================================
crvkBufferStatic::crvkBufferStatic
==============================================
*/
crvkBufferStatic::crvkBufferStatic( void ) : 
    m_device( nullptr), 
    m_fence( nullptr ), 
    m_semaphore( nullptr ),
    m_buffer( nullptr ), 
    m_memory( nullptr ) 
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
bool crvkBufferStatic::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult result = VK_SUCCESS;
    VkDevice device = nullptr; 
    m_device = const_cast<crvkDevice*>( in_device );
    device = in_device->Device();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = in_size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer( device, &bufferInfo, &k_allocationCallbacks, &m_buffer );
    if ( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateBuffer", result );
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( device, m_buffer, &memRequirements );

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    result = vkAllocateMemory( device, &allocInfo, &k_allocationCallbacks, &m_memory );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkBufferStatic::Create::vkAllocateMemory", result );
        return false;
    }

    result = vkBindBufferMemory( device, m_buffer, m_memory, 0 );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkBindBufferMemory", result );
        return false;
    }

    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT

    result = vkCreateFence( device, &fenceCI, &k_allocationCallbacks, &m_fence );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateFence", result );
        return false;
    }

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = 0;

    result = vkCreateSemaphore( device, &semaphoreCI, &k_allocationCallbacks, &m_semaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateFence", result );
        return false;
    }

    return true;
}

/*
==============================================
crvkBufferStatic::Destroy
==============================================
*/
void crvkBufferStatic::Destroy( void )
{
    VkDevice device = m_device->Device();
    if( m_semaphore != nullptr )
    {
        vkDestroySemaphore( device, m_semaphore, &k_allocationCallbacks );
        m_semaphore = nullptr;
    }

    if( m_fence != nullptr )
    {
        vkDestroyFence( device, m_fence, &k_allocationCallbacks );
        m_fence = nullptr;
    }

    if ( m_memory != nullptr )
    {
        vkFreeMemory( device, m_memory, &k_allocationCallbacks );
        m_memory = nullptr;
    }

    if ( m_buffer != nullptr )
    {
        vkDestroyBuffer( device, m_buffer, &k_allocationCallbacks );
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
    //TODO: sync whit GPU operations    
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
    auto device = m_device->Device();

    // wait for device release the buffer 
    vkWaitForFences( device, 1, &m_fence, VK_TRUE, UINT64_MAX);
    vkResetFences( device, 1, &m_fence );

    vkMapMemory( device, m_memory, in_offset, in_size, 0, &poiter );
    std::memcpy( in_data, poiter, in_size );
    vkUnmapMemory( device, m_memory );

}

/*
==============================================
crvkBufferStatic::Map
==============================================
*/
void* crvkBufferStatic::Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags )
{
    // TODO: implement fence control 
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
    // TODO: implement fence control 
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
crvkBufferStatic::Fence
==============================================
*/
VkFence crvkBufferStatic::Fence( void ) const
{
    return m_fence;
}

/*
==============================================
crvkBufferStatic::Semaphore
==============================================
*/
VkSemaphore crvkBufferStatic::Semaphore(void) const
{
    return m_semaphore;
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
bool crvkBufferStaging::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult result = VK_SUCCESS;
    VkDevice device = nullptr;
    VkMemoryRequirements memRequirements;
    
    m_device = const_cast<crvkDevice*>( in_device );
    device = m_device->Device(); 

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = in_size;

    VkMemoryAllocateInfo memoryAllocI{};
    memoryAllocI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    //
    // Create the GPU side buffer 
    // ==========================================================================
    bufferInfo.usage = in_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = m_device->HasTransferQueue() ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    
    result = vkCreateBuffer( device, &bufferInfo, &k_allocationCallbacks, &m_gpuBuffer );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::GPU::vkCreateBuffer", result );
        return false; 
    }

    vkGetBufferMemoryRequirements( device, m_gpuBuffer, &memRequirements );

    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    result = vkAllocateMemory( device, &memoryAllocI, &k_allocationCallbacks, &m_gpuBufferMemory ); 
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::GPU::vkAllocateMemory", result );
        return false;
    }

    // bind buffer handler to the memory
    result = vkBindBufferMemory( device, m_gpuBuffer, m_gpuBufferMemory, 0);
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::GPU::vkBindBufferMemory", result );
        return false;
    }

    //
    // Create the CPU side buffer 
    // ==========================================================================
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = m_device->HasTransferQueue() ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer( device, &bufferInfo, &k_allocationCallbacks, &m_cpuBuffer );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkCreateBuffer", result );
        return false; 
    }

    vkGetBufferMemoryRequirements( device, m_cpuBuffer, &memRequirements );

    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

    result = vkAllocateMemory( device, &memoryAllocI, &k_allocationCallbacks, &m_cpuBufferMemory ); 
    if ( result != VK_SUCCESS ) 
    {        
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkAllocateMemory", result );
        return false;
    }

    // bind buffer handler to the memory
    result = vkBindBufferMemory( device, m_cpuBuffer, m_cpuBufferMemory, 0);
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkBindBufferMemory", result );
        return false;
    }

    ///
    /// Create the Command buffer to record the buffer transfer operations
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_device->CommandPool();
    allocInfo.commandBufferCount = 1;

    result = vkAllocateCommandBuffers( device, &allocInfo, &m_commandBuffer );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkAllocateCommandBuffers", result );
        return false;
    }

    ///
    /// Create Fences and semaphores 
    /// ==========================================================================

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT

    result = vkCreateFence( device, &fenceCreateInfo, &k_allocationCallbacks, &m_fence );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateFence", result );
        return false;
    }

    VkSemaphoreTypeCreateInfo timelineCreateInfo{};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = 0;
    semaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( device, &semaphoreCI, &k_allocationCallbacks, &m_semaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkCreateFence", result );
        return false;
    }

    return true;
}
    
/*
==============================================
crvkBufferStaging::Destroy
==============================================
*/
void crvkBufferStaging::Destroy( void )
{
    VkDevice device = m_device->Device();

    if( m_semaphore != nullptr )
    {
        vkDestroySemaphore( device, m_semaphore, &k_allocationCallbacks );
        m_semaphore = nullptr;
    }

    if( m_fence != nullptr )
    {
        vkDestroyFence( device, m_fence, &k_allocationCallbacks );
        m_fence = nullptr;
    }

    // release the buffer operation command buffer 
    if ( m_commandBuffer != nullptr )
    {
        vkFreeCommandBuffers( device, m_device->CommandPool(), 1, &m_commandBuffer );
        m_commandBuffer = nullptr;
    }

    // release CPU side buffer memory 
    if ( m_cpuBufferMemory != nullptr )
    {
        vkFreeMemory( device, m_cpuBufferMemory, &k_allocationCallbacks );
        m_cpuBufferMemory = nullptr; 
    }
    
    // release the CPU side buffer handler 
    if ( m_cpuBuffer != nullptr )
    {
        vkDestroyBuffer( device, m_cpuBuffer, &k_allocationCallbacks );
        m_cpuBuffer = nullptr;
    }
 
    // release GPU side buffer memory 
    if ( m_gpuBufferMemory != nullptr )
    {
        vkFreeMemory( device, m_gpuBufferMemory, &k_allocationCallbacks );
        m_gpuBufferMemory = nullptr; 
    }
    
    // release GPU side buffer handler 
    if ( m_gpuBuffer != nullptr )
    {
        vkDestroyBuffer( device, m_gpuBuffer, &k_allocationCallbacks );
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
    VkDevice device = m_device->Device();
    crvkDeviceQueue* queue = nullptr;

    // copy data to our CPU buffer 
    vkMapMemory( device, m_cpuBufferMemory, in_offset, in_size, 0, &data );
    std::memcpy( data, in_data, in_size );
    vkUnmapMemory( device, m_cpuBufferMemory );

    // begin copy upload content to GPU     
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_device->CommandPool();
    allocInfo.commandBufferCount = 1;

    // begin registering command buffer 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer( m_commandBuffer, &beginInfo );

    // perform a copy command 
    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;
    vkCmdCopyBuffer( m_commandBuffer, m_cpuBuffer, m_gpuBuffer, 1, &copyRegion );

    // End buffer recording 
    vkEndCommandBuffer( m_commandBuffer );
    
    // submint the copy command to the device queue 
    VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmitInfo.commandBuffer = m_commandBuffer;
    
    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.stageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    signalInfo.semaphore = m_semaphore;
    signalInfo.value = k_CRVK_BUFFER_SEM_TYPE_COPY;
    signalInfo.deviceIndex = 0;
    signalInfo.pNext = nullptr;

   // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, k_CRVK_BUFFER_SEM_TYPE_FREE, VK_PIPELINE_STAGE_2_COPY_BIT, 0 }, // wait device release the buffer 
        { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, k_CRVK_BUFFER_SEM_TYPE_COPY, VK_PIPELINE_STAGE_2_COPY_BIT, 0 } // wait for last copy end 
    };

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );       

    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, nullptr ); 
}

/*
==============================================
crvkBufferStaging::GetSubData
==============================================
*/
void crvkBufferStaging::GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size )
{
    void* data = nullptr;
    crvkDeviceQueue* queue = nullptr;
    VkDevice device = m_device->Device();

    // begin a copy download comand 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer( m_commandBuffer, &beginInfo );

    // copy content from GPU buffer to the CPU buffer 
    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;
    vkCmdCopyBuffer( m_commandBuffer, m_gpuBuffer, m_cpuBuffer, 1, &copyRegion );
    
    //
    vkEndCommandBuffer( m_commandBuffer );
    
    // submint the copy command to the device queue 
    VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmitInfo.commandBuffer = m_commandBuffer;
    
    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.stageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    signalInfo.semaphore = m_semaphore;
    signalInfo.value = k_CRVK_BUFFER_SEM_TYPE_COPY; // signal that copy is done
    signalInfo.deviceIndex = 0;
    signalInfo.pNext = nullptr;

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, k_CRVK_BUFFER_SEM_TYPE_FREE, VK_PIPELINE_STAGE_2_COPY_BIT, 0 }, // wait device release the buffer 
        { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, k_CRVK_BUFFER_SEM_TYPE_COPY, VK_PIPELINE_STAGE_2_COPY_BIT, 0 } // wait for last copy end 
    };

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );

    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, m_fence );

    // wait for device end copy the buffer 
    vkWaitForFences( device, 1, &m_fence, VK_TRUE, UINT64_MAX);
    vkResetFences( device, 1, &m_fence );

    // copy the content of the CPU buffer to the data pointer
    vkMapMemory( device, m_cpuBufferMemory, in_offset, in_size, 0, &data );
    std::memcpy( in_data, data, in_size );
    vkUnmapMemory( device, m_cpuBufferMemory );
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
    crvkDeviceQueue * queue = nullptr;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer( m_commandBuffer, &beginInfo );

    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;

    // perform a copy from buffer 
    if ( m_flags & CRVK_BUFFER_MAP_ACCESS_WRITE ) // copy content from 
        vkCmdCopyBuffer( m_commandBuffer, m_cpuBuffer, m_gpuBuffer, 1, &copyRegion );
    else if ( m_flags & CRVK_BUFFER_MAP_ACCESS_READ )
        vkCmdCopyBuffer( m_commandBuffer, m_gpuBuffer, m_cpuBuffer, 1, &copyRegion );

    vkEndCommandBuffer( m_commandBuffer );

    VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmitInfo.commandBuffer = m_commandBuffer;
        
    // Signal that copy has ended  
    VkSemaphoreSubmitInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.stageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    signalInfo.semaphore = m_semaphore;
    signalInfo.value = k_CRVK_BUFFER_SEM_TYPE_COPY;
    signalInfo.deviceIndex = 0;
    signalInfo.pNext = nullptr;

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, k_CRVK_BUFFER_SEM_TYPE_FREE, VK_PIPELINE_STAGE_2_COPY_BIT, 0 }, // wait device release the buffer 
        { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, k_CRVK_BUFFER_SEM_TYPE_COPY, VK_PIPELINE_STAGE_2_COPY_BIT, 0 } // wait for last copy end 
    };
    
    // submint the copy command to the device queue 
    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );
    
    // submit commands
    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, m_fence );
        
    if ( m_flags & CRVK_BUFFER_MAP_ACCESS_READ )
    {
        // wait for copy end
        vkWaitForFences( m_device->Device(), 1, &m_fence, VK_TRUE, UINT64_MAX );
        vkResetFences( m_device->Device(), 1, &m_fence );
    }
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

/*
==============================================
crvkBufferStaging::Fence
==============================================
*/
VkFence crvkBufferStaging::Fence( void ) const
{
    return m_fence;
}

/*
==============================================
crvkBufferStaging::Handle
==============================================
*/
VkSemaphore crvkBufferStaging::Semaphore( void ) const
{
    return m_semaphore;
}

