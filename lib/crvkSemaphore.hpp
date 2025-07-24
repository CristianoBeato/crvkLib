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

#ifndef __CRVK_SEMAPHORE_HPP__
#define __CRVK_SEMAPHORE_HPP__

class crvkSemaphore
{
public:
    crvkSemaphore( void );
    ~crvkSemaphore( void );

    /// @brief Create semaphore object 
    /// @param in_device 
    /// @param in_flags 
    /// @return true on success, false on a error 
    bool                    Create( const crvkDevice* in_device, const VkSemaphoreCreateFlags in_flags );

    /// @brief Destroy the semaphore 
    /// @param  
    void                    Destroy( void );

    /// @brief 
    /// @param  
    /// @return 
    VkResult                Signal( void ) const;
    VkResult                Wait( const VkSemaphoreWaitFlags in_flags, const uint64_t in_timeou = UINT64_MAX ) const;
    VkSemaphore             Semaphore( void ) const;
    
private:
    VkDevice    m_device;
    VkSemaphore m_semaphore;
};

class crvkSemaphoreTimeline
{
public:
    crvkSemaphoreTimeline( void );
    ~crvkSemaphoreTimeline( void );

    bool                    Create( const crvkDevice* in_device, const uint32_t in_initialValue );
    void                    Destroy( void );
    VkResult                Signal( const uint64_t in_value ) const;
    VkResult                Wait( const uint64_t in_timeout ) const;
    uint64_t                CounterValue( void ) const;
    VkSemaphoreSubmitInfo   SignalSubmitInfo( const VkPipelineStageFlags2 in_stageFlags2 );
    VkSemaphoreSubmitInfo   WaitLastSubmitInfo( const VkPipelineStageFlags2 in_stageFlags2 );
    VkSemaphore             Semaphore( void ) const { return m_semaphore; }
    
private:
    std::atomic<int64_t>    m_value;
    VkSemaphore             m_semaphore;
    VkDevice                m_device;
};

#endif //!__CRVK_SEMAPHORE_HPP__