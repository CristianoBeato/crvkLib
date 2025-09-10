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

#ifndef __CRVK_FENCE_HPP__
#define __CRVK_FENCE_HPP__

typedef struct crvkFenceHandler_t;

class crvkFence
{
public:
    crvkFence( void );
    ~crvkFence( void );

    /// @brief Create the fence object 
    /// @param in_device Host device.
    /// @param in_flags Creation flags 
    void        Create( const VkDevice in_logicDevice, const VkFenceCreateFlags in_flags );

    /// @brief destroy the fence object  
    void        Destroy( void );

    /// @brief Reset the fence signal.
    /// @return On success, this command returns, VK_SUCCESS 
    /// On failure, this command returns VK_ERROR_OUT_OF_DEVICE_MEMORY, VK_ERROR_UNKNOWN or VK_ERROR_VALIDATION_FAILED.
    VkResult    Reset( void ) const;

    /// @brief Wait for fence to become signaled.
    /// @param in_timeout is the timeout period in units of nanoseconds.
    /// timeout is adjusted to the closest value allowed by the implementation-dependent
    /// timeout accuracy, which may be substantially longer than one nanosecond, and may 
    /// be longer than the requested period.
    /// @return On success, this command returns : VK_SUCCESS or VK_TIMEOUT
    /// On failure, this command returns VK_ERROR_DEVICE_LOST, VK_ERROR_OUT_OF_DEVICE_MEMORY,
    /// VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_UNKNOWN, VK_ERROR_VALIDATION_FAILED.
    VkResult    Wait( const uint64_t in_timeout ) const;

    /// @brief Return the current status of a fence.
    /// @return On success, this command returns VK_NOT_READY, VK_SUCCESS.
    /// On failure, this command returns VK_ERROR_DEVICE_LOST, VK_ERROR_OUT_OF_DEVICE_MEMORY, 
    // VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_UNKNOWN, VK_ERROR_VALIDATION_FAILED.
    VkResult    GetStatus( void ) const;

private:
    crvkFenceHandler_t* m_handle;
};

#endif //!__CRVK_FENCE_HPP__