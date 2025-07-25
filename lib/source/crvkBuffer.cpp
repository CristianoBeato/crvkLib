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
    m_useValue( 1 ),
    m_copyValue( 1 ),
    m_copySemaphore( nullptr ),
    m_useSemaphore( nullptr ),
    m_fence( nullptr ),
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
crvkBuffer::~crvkBuffer
==============================================
*/
bool crvkBuffer::Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags )
{
    VkResult result = VK_SUCCESS;
    VkDevice device  = nullptr; 
    m_device = const_cast<crvkDevice*>( in_device );
    device = m_device->Device();

    ///
    /// Create Fence
    /// ==========================================================================
    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT

    result = vkCreateFence( device, &fenceCI, k_allocationCallbacks, &m_fence );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateFence", result );
        return false;
    }

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
crvkBuffer::~crvkBuffer
==============================================
*/
void crvkBuffer::Destroy( void )
{
    auto device = m_device->Device();

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

    if( m_fence != nullptr )
    {
        vkDestroyFence( device, m_fence, k_allocationCallbacks );
        m_fence = nullptr;
    }

}

/*
==============================================
crvkBufferStatic::crvkBufferStatic
==============================================
*/
crvkBufferStatic::crvkBufferStatic( void ) :
    crvkBuffer(),
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

    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT

    result = vkCreateFence( device, &fenceCI, k_allocationCallbacks, &m_fence );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStatic::Create::vkCreateFence", result );
        return false;
    }

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = 0;

    result = vkCreateSemaphore( device, &semaphoreCI, k_allocationCallbacks, &m_semaphore );
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
        vkDestroySemaphore( device, m_semaphore, k_allocationCallbacks );
        m_semaphore = nullptr;
    }

    if( m_fence != nullptr )
    {
        vkDestroyFence( device, m_fence, k_allocationCallbacks );
        m_fence = nullptr;
    }

    if ( m_memory != nullptr )
    {
        vkFreeMemory( device, m_memory, k_allocationCallbacks );
        m_memory = nullptr;
    }

    if ( m_buffer != nullptr )
    {
        vkDestroyBuffer( device, m_buffer, k_allocationCallbacks );
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
crvkBufferStatic::SignalLastUse
==============================================
*/
VkSemaphoreSubmitInfo crvkBuffer::SignalLastUse(void)
{
    VkSemaphoreSubmitInfo useSignal{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_useSemaphore, ++m_useValue, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 };
    return useSignal;
}

/*
==============================================
crvkBufferStatic::SignalLastCopy
==============================================
*/
VkSemaphoreSubmitInfo crvkBuffer::SignalLastCopy(void)
{
    VkSemaphoreSubmitInfo copySignal{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_copySemaphore, ++m_copyValue, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 };
    return copySignal;
}

/*
==============================================
crvkBufferStatic::WaitLastUse
==============================================
*/
VkSemaphoreSubmitInfo crvkBuffer::WaitLastUse(void)
{
    VkSemaphoreSubmitInfo waitUse{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_useSemaphore, m_useValue -1, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 }; // wait finish last render 
    return waitUse;
}

/*
==============================================
crvkBufferStatic::WaitLastCopy
==============================================
*/
VkSemaphoreSubmitInfo crvkBuffer::WaitLastCopy(void)
{
    VkSemaphoreSubmitInfo waitCopy{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_copySemaphore, m_copyValue -1, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0 }; // wait finish last copy
    return waitCopy;
}

/*
==============================================
crvkBufferStaging::crvkBufferStaging
==============================================
*/
crvkBufferStaging::crvkBufferStaging( void ) :
    crvkBuffer(),
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
    
    if( !crvkBuffer::Create( in_device, in_size, in_usage, in_flags ) )
        return false;

    // get queues 
    graphics = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );
    tranfer = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    device = m_device->Device();

    VkMemoryAllocateInfo memoryAllocI{};
    memoryAllocI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    //
    // Create the GPU side buffer 
    // ==========================================================================
    VkBufferCreateInfo gpuBufferCI{};
    gpuBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    gpuBufferCI.usage = in_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    gpuBufferCI.size = in_size;

    // TODO: add suport to compute queue
    if ( tranfer != nullptr )
    {
        // if the graphics and transfer are in diferent families, we are using concurrent buffers 
        if ( tranfer->Family() != graphics->Family() )         
            gpuBufferCI.sharingMode = VK_SHARING_MODE_CONCURRENT;
        else
            gpuBufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        queues[0] = graphics->Family();
        queues[1] = tranfer->Family();
        gpuBufferCI.pQueueFamilyIndices = queues; 
        gpuBufferCI.queueFamilyIndexCount = 2;
    }
    else
    {
        // we don't have a tranfer queue, use only the graphic queue 
        gpuBufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        queues[0] = graphics->Family();
        gpuBufferCI.pQueueFamilyIndices = queues; 
        gpuBufferCI.queueFamilyIndexCount = 1;
    }

    result = vkCreateBuffer( device, &gpuBufferCI, k_allocationCallbacks, &m_gpuBuffer );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::GPU::vkCreateBuffer", result );
        return false; 
    }

    vkGetBufferMemoryRequirements( device, m_gpuBuffer, &memRequirements );

    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    result = vkAllocateMemory( device, &memoryAllocI, k_allocationCallbacks, &m_gpuBufferMemory ); 
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
    result = vkCreateBuffer( device, &cpuBufferCI, k_allocationCallbacks, &m_cpuBuffer );
    if ( result != VK_SUCCESS ) 
    {
        crvkAppendError( "crvkBufferStaging::Create::CPU::vkCreateBuffer", result );
        return false; 
    }

    vkGetBufferMemoryRequirements( device, m_cpuBuffer, &memRequirements );

    memoryAllocI.allocationSize = memRequirements.size;
    memoryAllocI.memoryTypeIndex = m_device->FindMemoryType( memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

    result = vkAllocateMemory( device, &memoryAllocI, k_allocationCallbacks, &m_cpuBufferMemory ); 
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
    allocInfo.commandBufferCount = 1;
    if( m_device->HasTransferQueue() )
        allocInfo.commandPool = m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER)->CommandPool();
    else
        allocInfo.commandPool = m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool();

    result = vkAllocateCommandBuffers( device, &allocInfo, &m_commandBuffer );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::Create::vkAllocateCommandBuffers", result );
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

    // release fences and semaphores
    crvkBuffer::Destroy();

    // release the buffer operation command buffer 
    if ( m_commandBuffer != nullptr )
    {
        if ( m_device->HasComputeQueue() )
            vkFreeCommandBuffers( device, m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER)->CommandPool(), 1, &m_commandBuffer );
        else
            vkFreeCommandBuffers( device, m_device->GetQueue(crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS)->CommandPool(), 1, &m_commandBuffer );
            
        m_commandBuffer = nullptr;
    }

    // release CPU side buffer memory 
    if ( m_cpuBufferMemory != nullptr )
    {
        vkFreeMemory( device, m_cpuBufferMemory, k_allocationCallbacks );
        m_cpuBufferMemory = nullptr; 
    }
    
    // release the CPU side buffer handler 
    if ( m_cpuBuffer != nullptr )
    {
        vkDestroyBuffer( device, m_cpuBuffer, k_allocationCallbacks );
        m_cpuBuffer = nullptr;
    }
 
    // release GPU side buffer memory 
    if ( m_gpuBufferMemory != nullptr )
    {
        vkFreeMemory( device, m_gpuBufferMemory, k_allocationCallbacks );
        m_gpuBufferMemory = nullptr; 
    }
    
    // release GPU side buffer handler 
    if ( m_gpuBuffer != nullptr )
    {
        vkDestroyBuffer( device, m_gpuBuffer, k_allocationCallbacks );
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
    VkResult result = VK_SUCCESS;
    void* data = nullptr;
    VkDevice device = m_device->Device();
    crvkDeviceQueue* queue = nullptr;

    // copy data to our CPU buffer 
    result = vkMapMemory( device, m_cpuBufferMemory, in_offset, in_size, 0, &data );
    std::memcpy( data, in_data, in_size );
    vkUnmapMemory( device, m_cpuBufferMemory );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkMapMemory", result );
        return;
    }

    // begin registering command buffer 
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffer, &beginInfo );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkBufferStaging::SubData::vkBeginCommandBuffer", result );
        return;
    }

    // perform a copy command 
    VkBufferCopy copyRegion{};
    copyRegion.dstOffset = in_offset;
    copyRegion.srcOffset = in_offset;
    copyRegion.size = in_size;
    vkCmdCopyBuffer( m_commandBuffer, m_cpuBuffer, m_gpuBuffer, 1, &copyRegion );

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
    VkSemaphoreSubmitInfo signalInfo = SignalLastCopy();

    // Wait for the last copy to finish, or buffer to be released  
    VkSemaphoreSubmitInfo waitInfo[2];
    waitInfo[0] = WaitLastCopy(); // wait device release the buffer 
    waitInfo[1] = WaitLastUse(); // wait for last copy end 

    if ( m_device->HasTransferQueue() )
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_TRANSFER );
    else
        queue = m_device->GetQueue( crvkDeviceQueue::CRVK_DEVICE_QUEUE_GRAPHICS );

    queue->Submit( waitInfo, 2, &commandBufferSubmitInfo, 1, &signalInfo, 1, m_fence );
    
#if 1
    // wait for device end copy the buffer 
    vkWaitForFences( device, 1, &m_fence, VK_TRUE, UINT64_MAX);
    vkResetFences( device, 1, &m_fence );
#else
    VkSemaphoreWaitInfo smWaitInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO, nullptr, 0, 1, &m_copySemaphore, 
    vkWaitSemaphores( m_device->Device(), &smWaitInfo, UINT64_MAX );
#endif

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

/*
==============================================
crvkBufferStaging::Handle
==============================================
*/
VkBuffer crvkBufferStaging::Handle( void ) const
{
    return m_cpuBuffer;
}
