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

#ifndef __CRVK_BUFFER_HPP__
#define __CRVK_BUFFER_HPP__

static const unsigned int CRVK_ACCESS_BUFFER_WRITE = 1 << 1; 
static const unsigned int CRVK_ACCESS_BUFFER_READ = 1 << 2; 

/// @brief our buffer base structure 
class crvkBuffer 
{
public:
    crvkBuffer( void ) {}

    /// @brief 
    /// @param in_contex 
    /// @param in_deviceProps 
    /// @param in_size 
    /// @param in_usage 
    /// @param in_flags 
    /// @return 
    virtual bool        Create( 
        const crvkContext* in_contex, 
        const crvkDevice* in_device, 
        const size_t in_size, 
        const VkBufferUsageFlags in_usage, 
        const uint32_t in_flags ) = 0;

    /// @brief 
    /// @param  
    virtual void        Destroy( void ) = 0;

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

    virtual bool Create( const crvkContext* in_contex, const crvkDevice* in_deviceProps, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags );
    virtual void        Destroy( void );
    virtual void        SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void        GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void*       Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags );
    virtual void        Unmap( void );
    virtual void        Flush( const uintptr_t in_offset, const size_t in_size );
    virtual VkBuffer    Handle( void ) const;

private:
    crvkDevice*     m_device;
    VkBuffer        m_buffer;
    VkDeviceMemory  m_memory;
};

///
/// @brief crvkBufferStaging works like OpenGL buffers, create a two stage buffer,    
/// a GPU memory bufer and a CPU side buffer and perform the buffer and sincronization
///
class crvkBufferStaging
{
public:
    crvkBufferStaging( void );
    ~crvkBufferStaging( void );

    virtual bool Create( const crvkContext* in_contex, const crvkDevice* in_deviceProps, const size_t in_size, const VkBufferUsageFlags in_usage, const uint32_t in_flags );
    virtual void Destroy( void );
    virtual void SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size );
    virtual void* Map( const uintptr_t in_offset, const size_t in_size, const uint32_t in_flags );
    virtual void Unmap( void );
    virtual void Flush( const uintptr_t in_offset, const size_t in_size );
    virtual VkBuffer Handle( void ) const;

private:
    uint32_t            m_flags;
    crvkDevice*         m_device;
    VkBuffer            m_gpuBuffer;
    VkBuffer            m_cpuBuffer;
    VkDeviceMemory      m_gpuBufferMemory;
    VkDeviceMemory      m_cpuBufferMemory;
    VkCommandBuffer     m_commandBuffer;
};

#endif //!__CRVK_BUFFER_HPP__