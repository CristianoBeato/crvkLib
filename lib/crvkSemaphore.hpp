// ===============================================================================================
// crvkPointer - Vulkan + SDL minimal framework
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

#ifndef __CRVK_SEMAPHORE_HPP__
#define __CRVK_SEMAPHORE_HPP__

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
    crvkDevice*             m_device;
};

#endif //!__CRVK_SEMAPHORE_HPP__