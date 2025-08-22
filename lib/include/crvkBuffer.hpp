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

enum crvkBufferMapAccess_t : uint8_t
{
    CRVK_BUFFER_MAP_ACCESS_NONE = 0; // not mapped 
    CRVK_BUFFER_MAP_ACCESS_WRITE,
    CRVK_BUFFER_MAP_ACCESS_READ
};

enum crvkBufferState_t : uint8_t
{
    CRVK_BUFFER_STATE_GRAPHIC_READ = 0,     /// buffer is goig to be used in shader as source 
    CRVK_BUFFER_STATE_GRAPHIC_WRITE,        /// buffer is going to be used in shader as destine
    CRVK_BUFFER_STATE_COMPUTE_READ,         ///
    CRVK_BUFFER_STATE_COMPUTE_WRITE,        ///
    CRVK_BUFFER_STATE_GPU_COPY_SRC,         /// buffer is going to be used on GPU copy operation as source 
    CRVK_BUFFER_STATE_GPU_COPY_DST,         /// buffer is going to be used on GPU copy operation as destine
    CRVK_BUFFER_STATE_CPU_COPY_SRC,         /// buffer is going to be used on CPU copy operation as source
    CRVK_BUFFER_STATE_CPU_COPY_DST          /// buffer is going to be used on CPU copy operation as destine
};

typedef struct crvkBufferHandler_t;

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
    virtual bool        Create( const crvkDevice* in_device, const size_t in_size, const VkBufferUsageFlags in_usage, const VkMemoryPropertyFlags in_flags );

    /// @brief 
    /// @param  
    virtual void        Destroy( void );

    /// @brief 
    /// @param srcBuffer 
    virtual void        CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferCopy2* in_regions, const uint32_t in_count ) {};

    /// @brief 
    /// @param srcBuffer 
    virtual void        CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferCopy2* in_regions, const uint32_t in_count ) {};

    /// @brief 
    /// @param in_data 
    /// @param in_offset 
    /// @param in_size 
    virtual void        SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size ) const {};

    /// @brief 
    /// @param in_data 
    /// @param in_offset 
    /// @param in_size 
    virtual void        GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size ) const {};

    /// @brief 
    /// @param in_offset 
    /// @param in_size 
    /// @param in_flags 
    /// @return 
    virtual void*       Map( const uintptr_t in_offset, const size_t in_size, const crvkBufferMapAccess_t in_acces );

    /// @brief 
    /// @param  
    virtual void        Unmap( void );
    
    /// @brief 
    /// @param in_offset 
    /// @param in_size 
    virtual void        Flush( const uintptr_t in_offset, const size_t in_size ) const;
    
    /// @brief 
    /// @param in_commandBuffer 
    /// @param in_state 
    /// @param in_dstQueue 
    virtual void        StateTransition( const VkCommandBuffer in_commandBuffer, const crvkBufferState_t in_state, const uint32_t in_dstQueue );

    /// @brief 
    /// @param  
    /// @return 
    virtual VkBuffer    Handle( void ) const;

protected:
    crvkBufferHandler_t*    m_bufferHandler;
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

    virtual bool Create( const crvkDevice* in_deviceProps, const size_t in_size, const VkBufferUsageFlags in_usage, const VkMemoryPropertyFlags in_flags ) override;
    virtual void Destroy( void ) override;
    
    /// @brief 
    /// @param srcBuffer 
    virtual void        CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferCopy2* in_regions, const uint32_t in_count ) override;
    virtual void        CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferCopy2* in_regions, const uint32_t in_count ) override;
    virtual void        StateTransition( const crvkBufferState_t in_state, const uint32_t in_dstQueue );
    virtual void*       Map( const uintptr_t in_offset, const size_t in_size, const crvkBufferMapAccess_t in_acces ) override;
    virtual void        Unmap( const crvkBufferState_t in_state );

protected:
    uintptr_t           m_mapOffset;
    size_t              m_mapSize;
    uint64_t            m_useValue;
    uint64_t            m_copyValue;
    VkSemaphore         m_copySemaphore;
    VkSemaphore         m_useSemaphore;    
    VkCommandBuffer     m_commandBuffer;
    crvkDevice*         m_device;

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
};

///
/// @brief crvkBufferStaging works like OpenGL buffers, create a two stage buffer,    
/// a GPU memory bufer and a CPU side buffer and perform the buffer and sincronization
///
class crvkBufferStaging : public crvkBufferStatic
{
public:
    crvkBufferStaging( void );
    ~crvkBufferStaging( void );
    virtual bool    Create( const crvkDevice* in_deviceProps, const size_t in_size, const VkBufferUsageFlags in_usage, const VkMemoryPropertyFlags in_flags ) override;
    virtual void    Destroy( void ) override;
    virtual void    SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size ) const override;
    virtual void    GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size ) const override;
    virtual void*   Map( const uintptr_t in_offset, const size_t in_size, const crvkBufferMapAccess_t in_acces ) override;
    virtual void    Unmap( void ) override;
    virtual void    Flush( const uintptr_t in_offset, const size_t in_size ) const override;
    
private:
    crvkBufferMapAccess_t   m_mapacess;
    VkBuffer                m_stagingBuffer;
    VkDeviceMemory          m_stagingMemory;
};

#endif //!__CRVK_BUFFER_HPP__