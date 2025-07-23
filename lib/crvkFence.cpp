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

#include "crvkPrecompiled.hpp"
#include "crvkFence.hpp"

/*
==============================================
crvkFence::crvkFence
==============================================
*/
crvkFence::crvkFence( void ) : m_device( nullptr ), m_fence( nullptr )
{
}


/*
==============================================
crvkFence::~crvkFence
==============================================
*/
crvkFence::~crvkFence( void )
{
}

/*
==============================================
crvkFence::Create
==============================================
*/
void crvkFence::Create( const crvkDevice *in_device, const VkFenceCreateFlags in_flags )
{
    VkFenceCreateInfo fenceCI{};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    fenceCI.flags = in_flags; 
}

/*
==============================================
crvkFence::Destroy
==============================================
*/
void crvkFence::Destroy( void )
{
    if ( m_fence != nullptr )
    {
        vkDestroyFence( m_device->Device(), m_fence, k_allocationCallbacks );
        m_fence = nullptr;
    }
    
    m_device = nullptr;
}

/*
==============================================
crvkFence::Reset
==============================================
*/
VkResult crvkFence::Reset( void ) const
{
    return vkResetFences( m_device->Device(), 1, &m_fence );
}


/*
==============================================
crvkFence::Wait
==============================================
*/
VkResult crvkFence::Wait( const uint64_t in_timeout ) const
{
    return vkWaitForFences( m_device->Device(), 1, &m_fence, VK_FALSE, in_timeout );
}

/*
==============================================
crvkFence::GetStatus
==============================================
*/
VkResult crvkFence::GetStatus( void ) const
{
    return vkGetFenceStatus( m_device->Device(), m_fence );
}