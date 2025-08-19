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
#include "crvkPrecompiled.hpp"
#include "crvkBuffer.hpp"

/*
==============================================
crvkBuffer::crvkBuffer
==============================================
*/
crvkBuffer::crvkBuffer( void ) : 
    m_buffer( nullptr ), 
    m_memory( nullptr ),
    m_device( nullptr )
{
}

/*
==============================================
crvkBuffer::~crvkBuffer
==============================================
*/
crvkBuffer::~crvkBuffer(void)
{
    Destroy();
}

/*
==============================================
crvkBuffer::Create
==============================================
*/
bool crvkBuffer::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const VkMemoryPropertyFlags in_flags )
{
    VkResult result = VK_SUCCESS;
    VkDevice device  = nullptr; 
    m_device = const_cast<crvkDevice*>( in_device );
    m_currentState.usage = in_usage;
    m_currentState.property = in_flags;
    m_currentState.queue = VK_QUEUE_FAMILY_IGNORED;
    device = m_device->Device();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = in_size;
    bufferInfo.usage = in_usage;

    // we use tranfer queue for buffer content
    if( m_device->HasTransferQueue() )
        bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    else
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer( device, &bufferInfo, k_allocationCallbacks, &m_buffer );
    if ( result != VK_SUCCESS) 
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateBuffer", result );
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements( device, m_buffer, &memRequirements );

    // try find the required memory 
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, in_flags );
    result = vkAllocateMemory( device, &allocInfo, k_allocationCallbacks, &m_memory );
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

    return true;
}

/*
==============================================
crvkBuffer::Destroy
==============================================
*/
void crvkBuffer::Destroy( void )
{
    if ( m_memory != nullptr )
    {
        vkFreeMemory( m_device->Device(), m_memory, k_allocationCallbacks );
        m_memory = nullptr;
    }

    if ( m_buffer != nullptr )
    {
        vkDestroyBuffer( m_device->Device(), m_buffer, k_allocationCallbacks );
        m_buffer = nullptr;
    }
    
    m_device = nullptr;
}

/*
==============================================
crvkBuffer::Map
==============================================
*/
void *crvkBuffer::Map( const uintptr_t in_offset, const size_t in_size, const crvkBufferMapAccess_t in_acces ) 
{
    void* poiter = nullptr;
    vkMapMemory( m_device->Device(), m_memory, in_offset, in_size, 0, &poiter );
    return poiter;    
}

/*
==============================================
crvkBuffer::Unmap
==============================================
*/
void crvkBuffer::Unmap( void )
{
    vkUnmapMemory( m_device->Device(), m_memory );
}

/*
==============================================
crvkBuffer::Flush
==============================================
*/
void crvkBuffer::Flush( const uintptr_t in_offset, const size_t in_size ) const
{
    VkMappedMemoryRange range{}; 
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = m_memory;
    range.offset = in_offset;
    range.size = in_size;
    vkFlushMappedMemoryRanges( m_device->Device(), 1, &range);
}


/*
==============================================
crvkBuffer::StateTransition
==============================================
*/
void crvkBuffer::StateTransition(   const VkCommandBuffer in_commandBuffer, 
                                    const crvkBufferState_t in_state, 
                                    const uint32_t in_dstQueue, 
                                    const VkDeviceSize in_offset, 
                                    const VkDeviceSize in_size )
{
    state_t oldState = m_currentState;
    state_t newState = m_currentState; // copy usage and properties

    newState.queue = in_dstQueue;
    
    switch ( in_state )
    {
    case CRVK_BUFFER_STATE_GRAPHIC_READ:
    {
        // index input 
        if( m_currentState.usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT ) // we just read 
        {
            newState.stage = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
            newState.access = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
        }
        // vertex input 
        else if ( m_currentState.usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ) // we just read 
        {
            newState.stage = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
            newState.access = VK_ACCESS_2_INDEX_READ_BIT;
        }
        // indirect input
        else if( m_currentState.usage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT )
        {
            newState.stage = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
            newState.access = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
        }
        // shader storage
        else if( m_currentState.usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
        {
            newState.stage = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
            newState.access = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
        }
        else
        {
            newState.stage = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
            newState.access = VK_ACCESS_2_SHADER_READ_BIT;
        }
    } break;
    case CRVK_BUFFER_STATE_GRAPHIC_WRITE:
    {
        // shader storage
        if( m_currentState.usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
        {
            newState.stage = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
            newState.access = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
        }
        else
        {
            newState.stage = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
            newState.access = VK_ACCESS_2_SHADER_WRITE_BIT;
        }
    } break;
    case CRVK_BUFFER_STATE_COMPUTE_READ:
    {
        newState.stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        newState.access = VK_ACCESS_2_SHADER_READ_BIT;
    } break;
    case CRVK_BUFFER_STATE_COMPUTE_WRITE:
    {
        newState.stage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        newState.access = VK_ACCESS_2_SHADER_WRITE_BIT;
    } break;
    case CRVK_BUFFER_STATE_GPU_COPY_SRC:
    {
        newState.stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        newState.access = VK_ACCESS_2_TRANSFER_READ_BIT;
    } break;
    case CRVK_BUFFER_STATE_GPU_COPY_DST:
    {
        newState.stage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        newState.access = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    } break;
    case CRVK_BUFFER_STATE_CPU_COPY_SRC:
    {
        newState.stage = VK_PIPELINE_STAGE_2_HOST_BIT;
        newState.access = VK_ACCESS_2_HOST_READ_BIT;
    } break;
    case CRVK_BUFFER_STATE_CPU_COPY_DST:
    {
        newState.stage = VK_PIPELINE_STAGE_2_HOST_BIT;
        newState.access = VK_ACCESS_2_HOST_WRITE_BIT;
    } break;
    }

    // nothing to change
    if( newState == oldState )
        return;

    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;

    // source state
    barrier.srcStageMask = oldState.stage;
    barrier.srcAccessMask = oldState.access;
    barrier.srcQueueFamilyIndex = oldState.queue;
    
    // destine state 
    barrier.dstStageMask = newState.stage;
    barrier.dstAccessMask = newState.access;
    barrier.dstQueueFamilyIndex = newState.queue;
    
    // region
    barrier.buffer = m_buffer;
    barrier.offset = in_offset;
    barrier.size = in_size;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;
    depInfo.dependencyFlags = 0;
    depInfo.memoryBarrierCount = 0;
    depInfo.pMemoryBarriers = nullptr;
    depInfo.bufferMemoryBarrierCount = 1;
    depInfo.pBufferMemoryBarriers = &barrier;
    depInfo.imageMemoryBarrierCount = 0;
    depInfo.pImageMemoryBarriers = nullptr;
    vkCmdPipelineBarrier2( in_commandBuffer, &depInfo );

    // update buffer state
    m_currentState = newState;
}

/*
==============================================
crvkBufferStatic::crvkBufferStatic
==============================================
*/
crvkBufferStatic::crvkBufferStatic( void ) :
    crvkBuffer(),
    m_useValue( 1 ),
    m_copyValue( 1 ),
    m_copySemaphore( nullptr ),
    m_useSemaphore( nullptr )
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
bool crvkBufferStatic::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const VkMemoryPropertyFlags in_flags )
{
    VkResult result = VK_SUCCESS;
    crvkDeviceQueue* queue = nullptr;
    VkDevice device = nullptr; 
    m_device = const_cast<crvkDevice*>( in_device );
    device = in_device->Device();

    if( !crvkBuffer::Create( in_device, in_size, in_usage, in_flags ) )
        return false;

    ///
    /// Create semaphores 
    /// ==========================================================================
    VkSemaphoreTypeCreateInfo timelineCreateInfo{};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo copySemaphoreCI{};
    copySemaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    copySemaphoreCI.flags = 0;
    copySemaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( device, &copySemaphoreCI, k_allocationCallbacks, &m_copySemaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkCreateSemaphore::COPY", result );
        return false;
    }

    VkSemaphoreCreateInfo drawSemaphoreCI{};
    drawSemaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawSemaphoreCI.flags = 0;
    drawSemaphoreCI.pNext = &timelineCreateInfo;

    result = vkCreateSemaphore( device, &drawSemaphoreCI, k_allocationCallbacks, &m_useSemaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkCreateSemaphore::DRAW", result );
        return false;
    }

    ///
    if( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );

    ///
    /// Create command buffer  
    /// ==========================================================================
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.commandPool = queue->CommandPool(); // get the command pool form available queue 
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    result = vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer ); 
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkAllocateCommandBuffers", result );
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

    // release the buffer operation command buffer 
    if ( m_commandBuffer != nullptr )
    {
        if ( m_device->HasComputeQueue() )
            vkFreeCommandBuffers( device, m_device->GetQueue(CRVK_DEVICE_QUEUE_TRANSFER)->CommandPool(), 1, &m_commandBuffer );
        else
            vkFreeCommandBuffers( device, m_device->GetQueue(CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool(), 1, &m_commandBuffer );
            
        m_commandBuffer = nullptr;
    }

    if( m_useSemaphore != nullptr )
    {
        vkDestroySemaphore( device, m_useSemaphore, k_allocationCallbacks );
        m_useSemaphore = nullptr;
    }

    if( m_copySemaphore != nullptr )
    {
        vkDestroySemaphore( device, m_copySemaphore, k_allocationCallbacks );
        m_copySemaphore = nullptr;
    }
    
    crvkBuffer::Destroy();
}

/*
==============================================
crvkBufferStatic::CopyFromBuffer
==============================================
*/
void crvkBufferStatic::CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferCopy2* in_regions, const uint32_t in_count ) 
{
    VkResult result = VK_SUCCESS;
    VkDeviceSize offsetBegin = 0;
    VkDeviceSize offsetEnd = 0;
    crvkDeviceQueue * queue = nullptr;

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );       

    // reset the command buffer before start using 
    result = vkResetCommandBuffer( m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::CopyFromBuffer::vkBeginCommandBuffer", result );
        return; // todo: trow a error
    }

    // begin registering command buffer 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::CopyFromBuffer::CopyFromBuffer", result );
        return;
    }

    // get the operation range
    for ( uint32_t i = 0; i < in_count; i++)
    {
        auto region = in_regions[i];
        offsetBegin = std::min( region.dstOffset, offsetBegin );
        offsetEnd = std::max( region.dstOffset + region.size, offsetEnd );
    }
    
    // change the buffer state to recive content
    crvkBuffer::StateTransition( m_commandBuffer, CRVK_BUFFER_STATE_GPU_COPY_DST, queue->Family(), offsetBegin, offsetEnd - offsetBegin );

    // perform the copy of the buffer conent 
    VkCopyBufferInfo2   copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.srcBuffer = in_srcBuffer;
    copyBufferInfo.dstBuffer = m_buffer;
    copyBufferInfo.regionCount = in_count;
    copyBufferInfo.pRegions = in_regions;
    copyBufferInfo.pNext = nullptr;
    vkCmdCopyBuffer2( m_commandBuffer, &copyBufferInfo );

    // End buffer recording 
    result = vkEndCommandBuffer( m_commandBuffer );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkEndCommandBuffer", result );
        return;
    }

    // submint the copy command to the device queue 
    VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmitInfo.commandBuffer = m_commandBuffer;
    
    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();
   
    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        WaitLastCopy(),
        WaitLastUse(),
    };

    result = queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, nullptr );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkQueueSubmit2", result );
        return;
    }
}

/*
==============================================
crvkBufferStatic::CopyToBuffer
==============================================
*/
void crvkBufferStatic::CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferCopy2 *in_regions, const uint32_t in_count )
{
    VkResult result = VK_SUCCESS;
    VkDeviceSize offsetBegin = 0;
    VkDeviceSize offsetEnd = 0;
    crvkDeviceQueue* queue = nullptr;
    VkDevice device = m_device->Device();

    // reset the command buffer before start using 
    result = vkResetCommandBuffer( m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::CopyFromBuffer::vkBeginCommandBuffer", result );
        return; // todo: trow a error
    }

    // begin a copy download comand 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer( m_commandBuffer, &beginInfo );

    // get the operation range
    for ( uint32_t i = 0; i < in_count; i++)
    {
        auto region = in_regions[i];
        offsetBegin = std::min( region.dstOffset, offsetBegin );
        offsetEnd = std::max( region.dstOffset + region.size, offsetEnd );
    }
    
    // change the buffer state to recive content
    crvkBuffer::StateTransition( m_commandBuffer, CRVK_BUFFER_STATE_GPU_COPY_SRC, queue->Family(), offsetBegin, offsetEnd - offsetBegin );

// BEATO Begin: is more safier we set this on source buffer before begin to copy the content
//              or we can lead to driver overhead
#if 0   
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    // we come from a buffer writig operation 
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    barrier.dstStageMask = VK_PIPELINE_STAGE_2_HOST_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_HOST_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = in_dstBuffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.dependencyFlags = 0;
    depInfo.memoryBarrierCount = 0;
    depInfo.pMemoryBarriers = nullptr;
    depInfo.bufferMemoryBarrierCount = 1;
    depInfo.pBufferMemoryBarriers = &barrier;
    depInfo.imageMemoryBarrierCount = 0;
    depInfo.pImageMemoryBarriers = nullptr;
    depInfo.pNext = nullptr;
    vkCmdPipelineBarrier2( m_commandBuffer, &depInfo );
#endif
// BEATO End

    // copy content from GPU buffer to the CPU buffer 
    VkCopyBufferInfo2 copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.srcBuffer = m_buffer;
    copyBufferInfo.dstBuffer = in_dstBuffer;
    copyBufferInfo.regionCount = in_count;
    copyBufferInfo.pRegions = in_regions;
    copyBufferInfo.pNext = nullptr;
    vkCmdCopyBuffer2( m_commandBuffer, &copyBufferInfo );
    
    // submint the copy command to the device queue 
    VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmitInfo.commandBuffer = m_commandBuffer;
    
    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2];
    waitInfo[0] = WaitLastCopy(); // wait device release the buffer 
    waitInfo[1] = WaitLastUse(); // wait for last copy end 

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );

    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, nullptr );
}

/*
==============================================
crvkBufferStatic::StateTransition
==============================================
*/
void crvkBufferStatic::StateTransition(const crvkBufferState_t in_state, const uint32_t in_dstQueue, const VkDeviceSize in_offset, const VkDeviceSize in_size)
{
    crvkBuffer::StateTransition( m_commandBuffer, in_state, in_dstQueue, in_offset, in_size );
}

/*
==============================================
crvkBufferStatic::StateTransition
==============================================
*/
void *crvkBufferStatic::Map( const uintptr_t in_offset, const size_t in_size, const crvkBufferMapAccess_t in_acces )
{
    VkResult result = VK_SUCCESS;
    crvkBufferState_t state = CRVK_BUFFER_STATE_CPU_COPY_SRC;
    crvkDeviceQueue* queue = nullptr;
    void* pointer = nullptr;

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );

    // reset the command buffer before start using 
    result = vkResetCommandBuffer( m_commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Map::vkResetCommandBuffer", result );
        return; // todo: trow a error
    }

    // begin a copy download comand 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Map::vkBeginCommandBuffer", result );
        return; // todo: trow a error
    }

    if ( in_acces == CRVK_BUFFER_MAP_ACCESS_READ )
        state = CRVK_BUFFER_STATE_CPU_COPY_SRC;
    else
        state = CRVK_BUFFER_STATE_CPU_COPY_DST;

    // change the buffer state to recive content
    crvkBuffer::StateTransition( m_commandBuffer, state, queue->Family(), in_offset, in_size );

    vkMapMemory( m_device->Device(), m_memory, in_offset, in_size, 0, &pointer );
    
    return pointer;    
}

/*
==============================================
crvkBufferStatic::Unmap
==============================================
*/
void crvkBufferStatic::Unmap( const crvkBufferState_t in_state )
{
    crvkDeviceQueue* queue = nullptr;

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );

    // release memory acess
    vkUnmapMemory( m_device->Device(), m_memory );
    crvkBuffer::StateTransition( m_commandBuffer, in_state, queue->Family(), 0, VK_WHOLE_SIZE );

    //
    vkEndCommandBuffer( m_commandBuffer );
    
    // submint the copy command to the device queue 
    VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferSubmitInfo.commandBuffer = m_commandBuffer;
    
    // signal to GPU to wait for the copy end before use
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2];
    waitInfo[0] = WaitLastCopy(); // wait device release the buffer 
    waitInfo[1] = WaitLastUse(); // wait for last copy end 

    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, nullptr );
}

/*
==============================================
crvkBufferStatic::SignalLastUse
==============================================
*/
VkSemaphoreSubmitInfo crvkBufferStatic::SignalLastUse(void)
{
    VkSemaphoreSubmitInfo useSignal{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_useSemaphore, ++m_useValue, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 };
    return useSignal;
}

/*
==============================================
crvkBufferStatic::SignalLastCopy
==============================================
*/
VkSemaphoreSubmitInfo crvkBufferStatic::SignalLastCopy(void)
{
    VkSemaphoreSubmitInfo copySignal{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_copySemaphore, ++m_copyValue, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 };
    return copySignal;
}

/*
==============================================
crvkBufferStatic::WaitLastUse
==============================================
*/
VkSemaphoreSubmitInfo crvkBufferStatic::WaitLastUse(void)
{
    VkSemaphoreSubmitInfo waitUse{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_useSemaphore, m_useValue -1, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 }; // wait finish last render 
    return waitUse;
}

/*
==============================================
crvkBufferStatic::WaitLastCopy
==============================================
*/
VkSemaphoreSubmitInfo crvkBufferStatic::WaitLastCopy(void)
{
    VkSemaphoreSubmitInfo waitCopy{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_copySemaphore, m_copyValue -1, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 }; // wait finish last copy
    return waitCopy;
}

/*
==============================================
crvkBufferStaging::crvkBufferStaging
==============================================
*/
crvkBufferStaging::crvkBufferStaging( void ) : crvkBufferStatic(), 
    m_stagingBuffer( nullptr ),
    m_stagingMemory( nullptr )
{
}

/*
==============================================
crvkBufferStaging::~crvkBufferStaging
==============================================
*/
crvkBufferStaging::~crvkBufferStaging( void )
{
}

/*
==============================================
crvkBufferStaging::Create
==============================================
*/
bool crvkBufferStaging::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const VkMemoryPropertyFlags in_flags )
{
    uint32_t queues[2]{ 0, 0 };
    VkResult result = VK_SUCCESS;
    VkDevice device = nullptr;
    crvkDeviceQueue* graphics = nullptr;
    crvkDeviceQueue* tranfer = nullptr;
    VkMemoryRequirements memRequirements;

    if( !crvkBufferStatic::Create( in_device, in_size, in_usage, in_flags | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) )
        return false;

    // get queues 
    graphics = m_device->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );
    tranfer = m_device->GetQueue( CRVK_DEVICE_QUEUE_TRANSFER );
    device = m_device->Device();

    //
    // Create the CPU side buffer 
    // ==========================================================================
    VkBufferCreateInfo cpuBufferCI{};
    cpuBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    cpuBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // this is a staging buffer on CPU side
    cpuBufferCI.size = in_size;

    if ( m_device->HasTransferQueue() )
        queues[0] = tranfer->Family();
    else
        queues[0] = graphics->Family();

    cpuBufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // only graphic or only transfer use this buffer 
    cpuBufferCI.queueFamilyIndexCount = 1;
    cpuBufferCI.pQueueFamilyIndices = queues;
    result = vkCreateBuffer( device, &cpuBufferCI, k_allocationCallbacks, &m_stagingBuffer );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkCreateBuffer", result );
        return false; 
    }

    vkGetBufferMemoryRequirements( device, m_stagingBuffer, &memRequirements );

    VkMemoryAllocateInfo memoryAllocI{};
    memoryAllocI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

    result = vkAllocateMemory( device, &memoryAllocI, k_allocationCallbacks, &m_stagingMemory ); 
    if ( result != VK_SUCCESS ) 
    {        
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkAllocateMemory", result );
        return false;
    }

    // bind buffer handler to the memory
    result = vkBindBufferMemory( device, m_stagingBuffer, m_stagingMemory, 0 );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkBindBufferMemory", result );
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

    // release CPU side buffer memory 
    if ( m_stagingMemory != nullptr )
    {
        vkFreeMemory( device, m_stagingMemory, k_allocationCallbacks );
        m_stagingMemory = nullptr; 
    }
    
    // release the CPU side buffer handler 
    if ( m_stagingBuffer != nullptr )
    {
        vkDestroyBuffer( device, m_stagingBuffer, k_allocationCallbacks );
        m_stagingBuffer = nullptr;
    }
 
    // release fences and semaphores
    crvkBufferStatic::Destroy();
}

/*
==============================================
crvkBufferStaging::SubData
==============================================
*/
void crvkBufferStaging::SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size ) const
{
    VkResult result = VK_SUCCESS;
    void* data = nullptr;
    VkDevice device = m_device->Device();

    // copy data to our CPU buffer 
    result = vkMapMemory( device, m_stagingMemory, in_offset, in_size, 0, &data );
    std::memcpy( data, in_data, in_size );
    vkUnmapMemory( device, m_stagingMemory );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkMapMemory", result );
        return;
    }

    VkBufferCopy2 copy{};
    copy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    copy.srcOffset = in_offset;
    copy.dstOffset = in_offset;
    copy.size =  in_size;
    copy.pNext = nullptr;

    // now we copy to from CPU buffer size to GPU Buffer
    const_cast<crvkBufferStaging*>( this )->CopyFromBuffer( m_stagingBuffer, &copy, 1 );
}

/*
==============================================
crvkBufferStaging::GetSubData
==============================================
*/
void crvkBufferStaging::GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size ) const 
{
    void* data = nullptr;
    VkDevice device = m_device->Device();

    VkBufferCopy2 copy{};
    copy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    copy.srcOffset = in_offset;
    copy.dstOffset = in_offset;
    copy.size =  in_size;
    copy.pNext = nullptr;
    
    // copy from GPU buffer to CPU
    const_cast<crvkBufferStaging*>( this )->CopyToBuffer( m_stagingBuffer, &copy, 1 );

    // wait for device end copy the buffer 
    VkSemaphoreWaitInfo smWaitInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO, nullptr, 0, 1, &m_copySemaphore }; 
    vkWaitSemaphores( m_device->Device(), &smWaitInfo, UINT64_MAX );

    // copy the content of the CPU buffer to the data pointer
    vkMapMemory( device, m_stagingMemory, in_offset, in_size, 0, &data );
    std::memcpy( in_data, data, in_size );
    vkUnmapMemory( device, m_stagingMemory );
}

/*
==============================================
crvkBufferStaging::Map
==============================================
*/
void* crvkBufferStaging::Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags ) const
{
    const_cast<crvkBufferStaging*>( this )->m_mapFlags = in_flags;
    VkResult res = VK_SUCCESS;
    VkDeviceSize bufferSize = VK_WHOLE_SIZE;
    void* map = nullptr;

    if ( in_size != 0 )
        bufferSize = in_size;

    res = vkMapMemory( m_device->Device(), m_stagingMemory, in_offset, bufferSize, 0, &map );
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
void crvkBufferStaging::Unmap( void ) const
{
    // release buffer memory map 
    vkUnmapMemory( m_device->Device(), m_stagingMemory );

    const_cast<crvkBufferStaging*>( this )->m_mapFlags = 0;
}

/*
==============================================
crvkBufferStaging::Flush
==============================================
*/
void crvkBufferStaging::Flush( const uintptr_t in_offset, const size_t in_size ) const
{
    if ( m_mapFlags == 0 )
        return;    

    VkBufferCopy2 copyRegion{};
    copyRegion.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    copyRegion.pNext = nullptr;
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;

    // perform a copy from buffer 
    if ( m_mapFlags & CRVK_BUFFER_MAP_ACCESS_WRITE ) // copy content from 
        const_cast<crvkBufferStaging*>( this )->CopyToBuffer( m_stagingBuffer, &copyRegion, 1 );
    else if ( m_mapFlags & CRVK_BUFFER_MAP_ACCESS_READ )
        const_cast<crvkBufferStaging*>( this )->CopyFromBuffer( m_stagingBuffer, &copyRegion, 1 );

}
