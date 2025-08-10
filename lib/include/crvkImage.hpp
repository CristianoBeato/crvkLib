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

#ifndef __CRVK_IMAGE_HPP__
#define __CRVK_IMAGE_HPP__

// basic image object, just create the structure
class crvkImage
{
public:
    crvkImage( void );
    ~crvkImage( void );

    virtual bool    Create(
        const crvkDevice* in_device, 
        const VkImageViewType in_type, 
        const VkFormat in_format,
        const uint16_t in_levels,
        const uint16_t in_layers,
        const uint32_t in_width,
        const uint32_t in_height,
        const uint32_t in_depth );
        
    virtual void    Destroy( void );
    virtual void    CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) {};
    virtual void    CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) {};
    virtual void    SubData( const void* in_data, const uintptr_t in_offset, const size_t in_size ) {};
    virtual void    GetSubData( void* in_data, const uintptr_t in_offset, const size_t in_size ) {};
    VkImage         Handle( void ) const { return m_imageHandle; };
    VkImageView     View( void ) const { return m_imageView; }
    VkDeviceMemory  Memory( void ) const { return m_imageMemory; }

public:
    VkImage             m_imageHandle;
    VkImageView         m_imageView;
    VkDeviceMemory      m_imageMemory;
    crvkDevice*         m_device;
};

class crvkImageStatic : public crvkImage 
{
public:
    crvkImageStatic( void );
    ~crvkImageStatic( void );

    virtual bool    Create(
        const crvkDevice* in_device, 
        const VkImageViewType in_type, 
        const VkFormat in_format,
        const uint16_t in_levels,
        const uint16_t in_layers,
        const uint32_t in_width,
        const uint32_t in_height,
        const uint32_t in_depth ) override;
        
    virtual void    Destroy( void ) override;
    virtual void    CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) override;
    virtual void    CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) override;

protected:
    VkSemaphoreSubmitInfo   SignalLastUse( void );
    VkSemaphoreSubmitInfo   SignalLastCopy( void );
    VkSemaphoreSubmitInfo   WaitLastUse( void );
    VkSemaphoreSubmitInfo   WaitLastCopy( void );

protected:
    uint64_t            m_useValue;
    uint64_t            m_copyValue;
    VkSemaphore         m_copySemaphore;
    VkSemaphore         m_useSemaphore;    
    VkCommandBuffer     m_commandBuffer;  
};

class crvkImageStaging : public crvkImageStatic 
{
public:
    crvkImageStaging( void );
    ~crvkImageStaging( void );

    virtual bool    Create(
        const crvkDevice* in_device, 
        const VkImageViewType in_type, 
        const VkFormat in_format,
        const uint16_t in_levels,
        const uint16_t in_layers,
        const uint32_t in_width,
        const uint32_t in_height,
        const uint32_t in_depth );
        
    virtual void    Destroy( void ) override;
    
private:
    VkBuffer        m_staging;
    VkDeviceMemory  m_memoryStaging;
};

#endif // __CRVK_IMAGE_HPP__