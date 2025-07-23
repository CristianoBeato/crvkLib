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

#ifndef __CRVK_BUFFER_HPP__
#define __CRVK_BUFFER_HPP__

// buffer creation flags
static const uint32_t CRVK_BUFFER_DYNAMIC_STORAGE_BIT   = 1 << 1; // Enable all internal buffer assistence  
static const uint32_t CRVK_BUFFER_UNSYNCHRONIZED_BIT    = 2 << 1; // Disabel buffer internal sinchronization 
static const uint32_t CRVK_BUFFER_CLIENT_STORAGE_BIT    = 3 << 1; // 

// map buffer flags
static const uint32_t CRVK_BUFFER_MAP_ACCESS_WRITE = 1 << 2; 
static const uint32_t CRVK_BUFFER_MAP_ACCESS_READ = 2 << 2;
static const uint32_t CRVK_BUFFER_MAP_COHERENT_BIT = 3 << 2;

/// @brief our buffer base structure 
class crvkBuffer 
{
public:
    crvkBuffer( void );
    ~crvkBuffer( void );

    /// @brief Create the buffer object and allocate memory 
    /// @param in_device the orgin buffer 
    /// @param in_size buffer size ( this is double on crvkBufferStaging )
    /// @param in_usage buffer usage 
    /// @param in_flags 
    /// @return 
    virtual bool        Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags );

    /// @brief 
    /// @param  
    virtual void        Destroy( void );

    /// @brief 
    /// @param in_data 
    /// @param in_offset 
    /// @param in_size 
    virtual void        SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size ) = 0;

    /// @brief 
    /// @param in_data 
    /// @param in_offset 
    /// @param in_size 
    virtual void        GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size ) = 0;

    /// @brief 
    /// @param in_offset 
    /// @param in_size 
    /// @param in_flags 
    /// @return 
    virtual void*       Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags ) = 0;

    /// @brief 
    /// @param  
    virtual void        Unmap( void ) = 0;
    
    /// @brief 
    /// @param in_offset 
    /// @param in_size 
    virtual void        Flush( const uintptr_t in_offset, const size_t in_size ) = 0;
    
    /// @brief 
    /// @param  
    /// @return 
    virtual VkBuffer    Handle( void ) const = 0;

protected:
    uint32_t            m_flags;
    uint64_t            m_useValue;
    uint64_t            m_copyValue;
    VkSemaphore         m_copySemaphore;
    VkSemaphore         m_useSemaphore;    
    VkFence             m_fence;
    crvkDevice*         m_device;

        /// @brief  
    /// @return /
    virtual VkFence     Fence( void ) const { return m_fence; };

    /// @brief 
    /// @param  
    /// @return 
    virtual VkSemaphore UseSemaphore( void ) const { return m_useSemaphore; };

    /// @brief 
    /// @param  
    /// @return 
    virtual VkSemaphore CopySemaphore( void ) const { return m_copySemaphore; }

    VkSemaphoreSubmitInfo   SignalLastUse( void );
    VkSemaphoreSubmitInfo   SignalLastCopy( void );
    VkSemaphoreSubmitInfo   WaitLastUse( void );
    VkSemaphoreSubmitInfo   WaitLastCopy( void );

private:
    crvkBuffer( const crvkBuffer & ) = delete;
    crvkBuffer operator=( const crvkBuffer & ) = delete;
};

/// @brief a simple buffer 
class crvkBufferStatic : public crvkBuffer
{
public:
    crvkBufferStatic( void );
    ~crvkBufferStatic( void );
    virtual bool Create( const crvkDevice* in_deviceProps, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags );
    virtual void Destroy( void );
    virtual void SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void* Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags );
    virtual void Unmap( void );
    virtual void Flush( const uintptr_t in_offset, const size_t in_size );
    virtual VkBuffer Handle( void ) const;

private:
    VkSemaphore     m_semaphore;
    VkBuffer        m_buffer;
    VkDeviceMemory  m_memory;
};

///
/// @brief crvkBufferStaging works like OpenGL buffers, create a two stage buffer,    
/// a GPU memory bufer and a CPU side buffer and perform the buffer and sincronization
///
class crvkBufferStaging : public crvkBuffer
{
public:
    crvkBufferStaging( void );
    ~crvkBufferStaging( void );
    virtual bool Create( const crvkDevice* in_deviceProps, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags );
    virtual void Destroy( void );
    virtual void SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void* Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags );
    virtual void Unmap( void );
    virtual void Flush( const uintptr_t in_offset, const size_t in_size );
    virtual VkBuffer Handle( void ) const;

private:
    bool                m_semaphoreInUse;
    VkBuffer            m_gpuBuffer;
    VkBuffer            m_cpuBuffer;
    VkDeviceMemory      m_gpuBufferMemory;
    VkDeviceMemory      m_cpuBufferMemory;
    VkCommandBuffer     m_commandBuffer;
};

#endif //!__CRVK_BUFFER_HPP__