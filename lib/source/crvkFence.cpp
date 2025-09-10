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
#include "crvkFence.hpp"

typedef struct crvkFenceHandler_t
{
    VkFence     fence = nullptr;        // fence handle 
    VkDevice    device = nullptr;       // logic device handle
} crvkFenceHandler_t;

/*
==============================================
crvkFence::crvkFence
==============================================
*/
crvkFence::crvkFence( void ) : m_handle( nullptr )
{
    m_handle = new crvkFenceHandler_t(); 
}

/*
==============================================
crvkFence::~crvkFence
==============================================
*/
crvkFence::~crvkFence( void )
{
    Destroy();
    if ( m_handle != nullptr )
    {
        delete m_handle;
        m_handle = nullptr;
    }
}

/*
==============================================
crvkFence::Create
==============================================
*/
void crvkFence::Create( const VkDevice in_logicDevice, const VkFenceCreateFlags in_flags )
{
    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    fenceCI.flags = in_flags;
    m_handle->device = in_logicDevice; 
    vkCreateFence( m_handle->device, &fenceCI, k_allocationCallbacks, &m_handle->fence ); 
}

/*
==============================================
crvkFence::Destroy
==============================================
*/
void crvkFence::Destroy( void )
{
    if ( m_handle == nullptr )
        return;
    
    if ( m_handle->fence != nullptr )
    {
        vkDestroyFence( m_handle->device, m_handle->fence, k_allocationCallbacks );
        m_handle->fence = nullptr;
    }
    
    m_handle->device = nullptr;
}

/*
==============================================
crvkFence::Reset
==============================================
*/
VkResult crvkFence::Reset( void ) const
{
    return vkResetFences( m_handle->device, 1, &m_handle->fence );
}


/*
==============================================
crvkFence::Wait
==============================================
*/
VkResult crvkFence::Wait( const uint64_t in_timeout ) const
{
    return vkWaitForFences( m_handle->device, 1, &m_handle->fence, VK_FALSE, in_timeout );
}

/*
==============================================
crvkFence::GetStatus
==============================================
*/
VkResult crvkFence::GetStatus( void ) const
{
    return vkGetFenceStatus( m_handle->device, m_handle->fence );
}