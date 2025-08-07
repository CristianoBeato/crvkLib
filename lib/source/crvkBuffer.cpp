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
    m_flags( 0 ),
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
bool crvkBuffer::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult result = VK_SUCCESS;
    VkDevice device  = nullptr; 
    m_device = const_cast<crvkDevice*>( in_device );
    device = m_device->Device();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = in_size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer( device, &bufferInfo, k_allocationCallbacks, &m_buffer );
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
void *crvkBuffer::Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags ) const
{
    // TODO: implement fence control 
    void* poiter = nullptr;
    vkMapMemory( m_device->Device(), m_memory, in_offset, in_size, 0, &poiter );
    return poiter;    
}

/*
==============================================
crvkBuffer::Unmap
==============================================
*/
void crvkBuffer::Unmap( void ) const
{
    // TODO: implement fence control 
    vkUnmapMemory( m_device->Device(), m_memory );
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
bool crvkBufferStatic::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult result = VK_SUCCESS;
    VkDevice device = nullptr; 
    m_device = const_cast<crvkDevice*>( in_device );
    device = in_device->Device();

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
            vkFreeCommandBuffers( device, m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER)->CommandPool(), 1, &m_commandBuffer );
        else
            vkFreeCommandBuffers( device, m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool(), 1, &m_commandBuffer );
            
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
    VkDeviceSize barrierRangeBegin = UINT64_MAX;
    VkDeviceSize barrierRangeEnd = 0;
    crvkDeviceQueue * queue = nullptr;

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

    // perform the copy of the buffer conent 
    VkCopyBufferInfo2   copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.srcBuffer = in_srcBuffer;
    copyBufferInfo.dstBuffer = m_buffer;
    copyBufferInfo.regionCount = in_count;
    copyBufferInfo.pRegions = in_regions;
    copyBufferInfo.pNext = nullptr;
    vkCmdCopyBuffer2( m_commandBuffer, &copyBufferInfo );

    // get the alteration range 
    for ( uint32_t i = 0; i < in_count; i++)
    {
        auto r = in_regions[i];
        barrierRangeBegin = std::min( barrierRangeBegin, r.dstOffset );
        barrierRangeEnd = std::max( barrierRangeEnd, r.dstOffset + r.size );
    };

    // Memory barrier to ensure later visibility
    VkBufferMemoryBarrier2 barrier;
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT; // ou fragment, compute, etc
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = m_buffer;
    barrier.offset = barrierRangeBegin;
    barrier.size = barrierRangeEnd - barrierRangeBegin; 

    //
    VkDependencyInfo    dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.pNext = nullptr;
    dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    dependencyInfo.memoryBarrierCount = 0;
    dependencyInfo.pMemoryBarriers = nullptr;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    dependencyInfo.imageMemoryBarrierCount = 0;
    dependencyInfo.pImageMemoryBarriers = nullptr;
    vkCmdPipelineBarrier2( m_commandBuffer, &dependencyInfo ); 

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

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );       

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
void crvkBufferStatic::CopyToBuffer(const VkBuffer in_dstBuffer, const VkBufferCopy2 *in_regions, const uint32_t in_count)
{
    VkResult result = VK_SUCCESS;
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

    // copy content from GPU buffer to the CPU buffer 
    VkCopyBufferInfo2 copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.srcBuffer = m_buffer;
    copyBufferInfo.dstBuffer = in_dstBuffer;
    copyBufferInfo.regionCount = in_count;
    copyBufferInfo.pRegions = in_regions;
    copyBufferInfo.pNext = nullptr;
    vkCmdCopyBuffer2( m_commandBuffer, &copyBufferInfo );
    
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
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

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );

    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, nullptr );
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
bool crvkBufferStaging::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    uint32_t queues[2]{ 0, 0 };
    VkResult result = VK_SUCCESS;
    VkDevice device = nullptr;
    crvkDeviceQueue* graphics = nullptr;
    crvkDeviceQueue* tranfer = nullptr;
    VkMemoryRequirements memRequirements;
    
    if( !crvkBufferStatic::Create( in_device, in_size, in_usage, in_flags ) )
        return false;

    // get queues 
    graphics = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );
    tranfer = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
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
void crvkBufferStaging::SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size )
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
    crvkBufferStatic::CopyFromBuffer( m_stagingBuffer, &copy, 1 );
}

/*
==============================================
crvkBufferStaging::GetSubData
==============================================
*/
void crvkBufferStaging::GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size )
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
    crvkBufferStatic::CopyToBuffer( m_stagingBuffer, &copy, 1 );

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
void* crvkBufferStaging::Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags )
{
    m_flags = in_flags;
    
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
void crvkBufferStaging::Unmap( void )
{
    // release buffer memory map 
    vkUnmapMemory( m_device->Device(), m_stagingMemory );
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
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2] = 
    {
        WaitLastCopy(), // wait device release the buffer 
        WaitLastUse(), // wait for last copy end 
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
