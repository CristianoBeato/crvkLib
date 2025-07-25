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

#ifndef __CRVK_DYNAMIC_VECTOR_HPP__
#define __CRVK_DYNAMIC_VECTOR_HPP__

#include <cstring>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_assert.h>

template<typename _t>
class crvkDynamicVector
{
public:
    typedef _t&         reference;
    typedef const _t&   const_reference;
    typedef _t*         pointer;
    typedef const _t*   const_pointer;

    crvkDynamicVector( void );
    crvkDynamicVector( const crvkDynamicVector& in_ref );
    ~crvkDynamicVector( void );

    void    Clear( void );
    void    Resize( const uint32_t count );
    void    Memcpy( const_pointer in_source, const uint32_t in_offset, const uint32_t in_count );
    void    Memset( const int32_t in_val );

    // insert a elemnt at end of teh vector
    uint32_t Append( const_reference ref );

    /// @brief Return the internal element count 
    /// @return 
    const uint32_t  Count( void ) const { return m_count; }

    pointer         Pointer( void ) { return m_data; } 
    const_pointer   Pointer( void ) const { return m_data; } 

    pointer         operator&( void ) { return m_data; }
    const_pointer   operator&( void ) const { return m_data;} 

    reference       operator[]( const uint32_t i ) { return m_data[i]; }
    const_reference operator[]( const uint32_t i ) const { return m_data[i]; }

    crvkDynamicVector& operator=( const crvkDynamicVector &in_ref );
    const crvkDynamicVector operator=( const crvkDynamicVector &in_ref ) const;

private:
    uint32_t    m_count;
    pointer     m_data;
};

template<typename _t>
crvkDynamicVector<_t>::crvkDynamicVector( void ) : m_count( 0 ), m_data( nullptr )
{
}

template <typename _t>
inline crvkDynamicVector<_t>::crvkDynamicVector(const crvkDynamicVector &in_ref )
{
    // copy the reference content
    Resize( in_ref.Count() );
    Memcpy( &in_ref, 0, in_ref.Count() );
}

template<typename _t>
crvkDynamicVector<_t>::~crvkDynamicVector( void )
{
    Clear();    
}

template<typename _t>
inline void crvkDynamicVector<_t>::Clear( void )
{
    if ( m_data != nullptr )
    {
        SDL_free( m_data );
        m_data = nullptr;
    }

    m_count = 0;
}

template <typename _t>
inline void crvkDynamicVector<_t>::Resize( const uint32_t in_count )
{
    // don't resize 
    if ( in_count < m_count || in_count == 0 )
        return;

    // create a new 
    if( m_data == nullptr )
        m_data = static_cast<pointer>( SDL_malloc( sizeof( _t ) * in_count ) );
    else
        m_data = static_cast<pointer>( SDL_realloc( m_data, sizeof( _t ) * in_count ) );
        
    SDL_assert( m_data != nullptr );
    m_count = in_count;
}

template <typename _t>
inline void crvkDynamicVector<_t>::Memcpy(const_pointer in_source, const uint32_t in_offset, const uint32_t in_count)
{
    std::memcpy( &m_data[in_offset], in_source, sizeof( _t ) * in_count );
}

template <typename _t>
inline void crvkDynamicVector<_t>::Memset( const int32_t in_val )
{
    std::memset( m_data, in_val, sizeof(_t) * m_count );
}

template<typename _t>
inline uint32_t crvkDynamicVector<_t>::Append( const_reference in_ref )
{
    uint32_t index = m_count++;
    m_data = static_cast<pointer>( SDL_realloc( m_data, sizeof(_t) * m_count ) );
    std::memcpy( &m_data[index], &in_ref, sizeof( _t ) );
    return index;
}

template <typename _t>
inline crvkDynamicVector<_t> &crvkDynamicVector<_t>::operator=( const crvkDynamicVector<_t> &in_ref )
{
    Resize( in_ref.Count() ); // alloc array 
    Memcpy( &in_ref, 0, in_ref.Count() ); // copy content
    return *this;
}

template <typename _t>
inline const crvkDynamicVector<_t> crvkDynamicVector<_t>::operator=(const crvkDynamicVector<_t> &in_ref) const
{
    Resize( in_ref.Count() ); // alloc array 
    Memcpy( &in_ref, 0, in_ref.Count() ); // copy content
    return *this;
}

#endif //!__CRVK_DYNAMIC_VECTOR_HPP__