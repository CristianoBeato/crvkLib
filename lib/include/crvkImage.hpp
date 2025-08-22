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

enum crvkImageState_t : uint8_t
{
    CRVK_IMAGE_STATE_GRAPHIC_SHADER_SAMPLER = 0, // shader input texture 
    CRVK_IMAGE_STATE_GRAPHIC_SHADER_BINDING, // storage image
    CRVK_IMAGE_STATE_GRAPHIC_RENDER_TARGET, // frame buffer output pinding image
    CRVK_IMAGE_STAGE_GRAPHIC_RENDER_DEPTH,
    CRVK_IMAGE_STAGE_GRAPHIC_RENDER_DEPTH_STENCIL, 
    CRVK_IMAGE_STATE_COMPUTE_READ,
    CRVK_IMAGE_STATE_COMPUTE_WRITE,
    CRVK_IMAGE_STATE_GPU_COPY_SRC,
    CRVK_IMAGE_STATE_GPU_COPY_DST,
};

typedef struct crvkImageRegion_t
{
    uint32_t    width = 0;
    uint32_t    hight = 0;
    uint32_t    depth = 0;
    uint32_t    level = 0;
    uint32_t    array = 0;
    void*       pixels = nullptr;
};

typedef struct crvkImageHandle_t crvkImageHandle_t;

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
        const uint32_t in_depth,
        const VkSampleCountFlagBits in_samples = VK_SAMPLE_COUNT_1_BIT );
        
    virtual void    Destroy( void );
    virtual bool    CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) { return false; };
    virtual bool    CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) { return false; };
    virtual bool    SubData( const void* in_data, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) { return false; };
    virtual bool    GetSubData( void* in_data, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) { return false; }; 
    virtual void    StateTransition( const VkCommandBuffer in_commandBuffer, const crvkImageState_t in_state, const VkImageAspectFlags in_aspect, const uint32_t in_dstQueue );
    
    VkImage         Handle( void ) const;
    VkImageView     View( void ) const;
    VkDeviceMemory  Memory( void ) const;

protected:
    crvkImageHandle_t* m_imageHandle;  
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
        const uint32_t in_depth, 
        const VkSampleCountFlagBits in_samples = VK_SAMPLE_COUNT_1_BIT ) override;
        
    virtual void    Destroy( void ) override;
    virtual bool    CopyFromBuffer( const VkBuffer in_srcBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) override;
    virtual bool    CopyToBuffer( const VkBuffer in_dstBuffer, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) override;
    virtual void    StateTransition( const VkCommandBuffer in_commandBuffer, const crvkImageState_t in_state, const VkImageAspectFlags in_aspect, const uint32_t in_dstQueue );

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
    VkCommandPool       m_commandPool;
    VkCommandBuffer     m_commandBuffer;
    crvkDevice*         m_device;  
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
    virtual bool    SubData( const void* in_data, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) override;
    virtual bool    GetSubData( void* in_data, const VkBufferImageCopy2* in_copyRegions, const uint32_t in_count ) override;
    
private:
    VkBuffer        m_staging;
    VkDeviceMemory  m_memoryStaging;
};

#endif // __CRVK_IMAGE_HPP__