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
#include "crvkSemaphore.hpp"

/*
==============================================
crvkSemaphore::crvkSemaphore
==============================================
*/
crvkSemaphore::crvkSemaphore( void ) : m_device( nullptr ), m_semaphore( nullptr )
{
}

/*
==============================================
crvkSemaphore::~crvkSemaphore
==============================================
*/
crvkSemaphore::~crvkSemaphore( void )
{
}

/*
==============================================
crvkSemaphore::Create
==============================================
*/
bool crvkSemaphore::Create( const crvkDevice* in_device, const VkSemaphoreCreateFlags in_flags )
{
    m_device = const_cast<crvkDevice*>( in_device );

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = in_flags;
    semaphoreCI.pNext = nullptr;

    VkResult result = vkCreateSemaphore( m_device->Device(), &semaphoreCI, k_allocationCallbacks, &m_semaphore );
    if ( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkSemaphore::Create::vkCreateSemaphore", result );
        return false;
    }
    
    return false;
}

/*
==============================================
crvkSemaphore::Destroy
==============================================
*/
void crvkSemaphore::Destroy( void )
{
    if( m_semaphore != nullptr )
    {
        vkDestroySemaphore( m_device->Device(), m_semaphore, k_allocationCallbacks );
        m_semaphore = nullptr;
    }

    m_device = nullptr;
}

/*
==============================================
crvkSemaphore::Signal
==============================================
*/
VkResult crvkSemaphore::Signal( void ) const
{
    VkSemaphoreSignalInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = m_semaphore;
    signalInfo.value = 0;
    return vkSignalSemaphore( m_device->Device(), &signalInfo );   
}

/*
==============================================
crvkSemaphore::Wait
==============================================
*/
VkResult crvkSemaphore::Wait( const VkSemaphoreWaitFlags in_flags, const uint64_t in_timeou ) const
{
    VkSemaphoreWaitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.flags = in_flags;
    waitInfo.pSemaphores = &m_semaphore;
    waitInfo.semaphoreCount = 1;
    waitInfo.pValues = nullptr;
    waitInfo.pNext = nullptr;
    return vkWaitSemaphores( m_device->Device(), &waitInfo, in_timeou );
}

/*
==============================================
crvkSemaphoreTimeline::crvkSemaphoreTimeline
==============================================
*/
crvkSemaphoreTimeline::crvkSemaphoreTimeline( void ) : 
    m_value( 0 ),
    m_semaphore( nullptr ),
    m_device( nullptr )
{
}

/*
==============================================
crvkSemaphoreTimeline::~crvkSemaphoreTimeline
==============================================
*/
crvkSemaphoreTimeline::~crvkSemaphoreTimeline( void )
{
}

/*
==============================================
crvkSemaphoreTimeline::Create
==============================================
*/
bool crvkSemaphoreTimeline::Create( const crvkDevice* in_device, const uint32_t in_initialValue )
{
    m_device = const_cast<crvkDevice*>( in_device );
    
    VkSemaphoreTypeCreateInfo timelineCreateInfo{};
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = in_initialValue;

    VkSemaphoreCreateInfo semaphoreCI{};
    semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCI.flags = 0;
    semaphoreCI.pNext = &timelineCreateInfo;

    auto result = vkCreateSemaphore( m_device->Device(), &semaphoreCI, k_allocationCallbacks, &m_semaphore );
    if( result != VK_SUCCESS )
    {
        crvkAppendError( "crvkSemaphoreTimeline::Create::vkCreateSemaphore", result );
        return false;
    }   

    return true;
}

/*
==============================================
crvkSemaphoreTimeline::Destroy
==============================================
*/
void crvkSemaphoreTimeline::Destroy( void )
{
    if ( m_semaphore != nullptr )
    {
        vkDestroySemaphore( m_device->Device(), m_semaphore, k_allocationCallbacks );
        m_semaphore = nullptr;
    }
    
    m_value = 0;
    m_device = nullptr;
}

/*
==============================================
crvkSemaphoreTimeline::Signal
==============================================
*/
VkResult crvkSemaphoreTimeline::Signal( const uint64_t in_value ) const
{
    VkSemaphoreSignalInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.semaphore = m_semaphore;
    signalInfo.value = in_value;

    return vkSignalSemaphore( m_device->Device(), &signalInfo );   
}

/*
==============================================
crvkSemaphoreTimeline::Wait
==============================================
*/
VkResult crvkSemaphoreTimeline::Wait( const uint64_t in_timeout ) const
{
    VkSemaphoreWaitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.pSemaphores = &m_semaphore;
    waitInfo.semaphoreCount = 1;
 
    return vkWaitSemaphores( m_device->Device(), &waitInfo, in_timeout );
}

/*
==============================================
crvkSemaphoreTimeline::CounterValue
==============================================
*/
uint64_t crvkSemaphoreTimeline::CounterValue(void) const
{
    uint64_t value = 0;
    vkGetSemaphoreCounterValue( m_device->Device(), m_semaphore, &value );
    return value;
}

/*
==============================================
crvkSemaphoreTimeline::CounterValue
==============================================
*/
VkSemaphoreSubmitInfo crvkSemaphoreTimeline::SignalSubmitInfo( const VkPipelineStageFlags2 in_stageFlags2 )
{
    m_value.fetch_add( 1 );
    VkSemaphoreSubmitInfo singal{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, m_value, in_stageFlags2, 0 }; 
    return singal;
}

/*
==============================================
crvkSemaphoreTimeline::CounterValue
==============================================
*/
VkSemaphoreSubmitInfo crvkSemaphoreTimeline::WaitLastSubmitInfo( const VkPipelineStageFlags2 in_stageFlags2 )
{
    VkSemaphoreSubmitInfo wait{ VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, nullptr, m_semaphore, m_value - 1, in_stageFlags2, 0 }; 
    return wait;
}
