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

#ifndef __CRVK_QUEUE_HPP__
#define __CRVK_QUEUE_HPP__

typedef struct crvkQueueHandle_t;

typedef enum crvkQueueType_e : int8_t
{
    CRVK_DEVICE_QUEUE_NONE = -1,
    CRVK_DEVICE_QUEUE_GRAPHICS,
    CRVK_DEVICE_QUEUE_PRESENT,
    CRVK_DEVICE_QUEUE_COMPUTE,
    CRVK_DEVICE_QUEUE_TRANSFER
} crvkQueueType_t;

typedef struct crvkQueueInfo_t
{
    bool        present = false;    // true if suport presenting
    bool        graphic = false;    // true if are a valid graphic queue
    bool        compute = false;    // true for suport 
    bool        transfer = false;
    uint32_t    family = UINT32_MAX;
    uint32_t    index = UINT32_MAX;
} crvkQueueInfo_t;

class crvkDeviceQueue
{
public:
    crvkDeviceQueue( void );
    ~crvkDeviceQueue( void );

    bool    Create( const crvkDevice *in_device, const crvkQueueInfo_t *&in_deviceQueue );
    void    Destroy( void );

    VkResult Submit( 
                    const VkSemaphoreSubmitInfo* in_waitSemaphoreInfos,
                    const uint32_t in_waitSemaphoreInfoCount,
                    const VkCommandBufferSubmitInfo* in_commandBufferInfos,
                    const uint32_t in_commandBufferInfoCount,
                    const VkSemaphoreSubmitInfo* in_signalSemaphoreInfos,
                    const uint32_t in_signalSemaphoreInfoCount, 
                    const VkFence in_fence );

    VkResult    Present( 
        const VkSwapchainKHR* in_swapchains,
        const uint32_t* in_imageIndices,
        const uint32_t in_swapchainCount,
        const VkSemaphore* in_waitSemaphores,
        const uint32_t in_waitSemaphoresCount );

    VkResult    WaitIdle( void ) const;

    /// @brief Queue family index
    /// @return UINT32_MAX if not valid
    uint32_t    Family( void ) const;

    /// @brief Queue index 
    /// @return UINT32_MAX if not valid 
    uint32_t    Index( void ) const;

    /// @brief
    /// @return 
    VkQueue         Queue( void ) const;

    /// @brief  
    /// @return 
    VkCommandPool   CommandPool( void ) const;

protected:
    friend class crvkDevice;
    bool    InitQueue( const VkDevice in_device );

private:
    crvkQueueHandle_t*  m_handle;

    crvkDeviceQueue( const crvkDeviceQueue & ) = delete;
    crvkDeviceQueue operator=( const crvkDeviceQueue & ) = delete;
};

#endif //__CRVK_QUEUE_HPP__