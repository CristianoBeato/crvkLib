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

#ifndef __CRVK_POINTER_HPP__
#define __CRVK_POINTER_HPP__

template< typename _t >
class crvkPointer
{
public:
    typedef _t&         reference;
    typedef const _t&   const_reference;
    typedef _t*         pointer;
    typedef const _t*   const_pointer;

    crvkPointer( void );
    crvkPointer( const uint32_t in_count );
    ~crvkPointer( void );

    template<typename... Args>
    void    New( Args&&... args );
    void    Alloc( const uint32_t in_count, const uint32_t in_alignament = 8, const int in_initial = 0xFFFFFFFF );
    void    Realloc( const uint32_t in_count, const uint32_t in_alignament = 8 );
    void    Free( void );
    void    Delete( void );
    void    Memcpy( const_pointer in_source, const uint32_t in_offset, const uint32_t in_count );
    void    Memset( const int in_val );
    size_t  Size( void ) const { return sizeof( _t ) * m_count; }

    /// @brief Return the internal element count 
    /// @return 
    const uint32_t Count( void ) const { return m_count; }

    pointer operator->( void ) { return m_data; }
    const_pointer operator->( void ) const { return m_data; }

    pointer         Pointer( void ) { return m_data; } 
    const_pointer   Pointer( void ) const { return m_data; } 

    pointer         operator&( void ) { return m_data; }
    const_pointer   operator&( void ) const { return m_data;} 

    reference       operator[]( const uint32_t i ) { return m_data[i]; }
    const_reference operator[]( const uint32_t i ) const { return m_data[i]; }

private:
    uint32_t        m_count;
    pointer         m_data;

    // disable reference and copy 
    crvkPointer( const crvkPointer & ) = delete;
    crvkPointer operator =( const crvkPointer & ) const = delete;
};

template< typename _t >
inline crvkPointer<_t>::crvkPointer( void ) : m_count( 0 ), m_data( nullptr )
{
}

template <typename _t>
inline crvkPointer<_t>::crvkPointer( const uint32_t in_count ) : m_count( 0 ), m_data( nullptr )
{
    Alloc( in_count );
}

template< typename _t >
crvkPointer<_t>::~crvkPointer( void )
{
    Free();
}

template <typename _t>
inline void crvkPointer<_t>::Alloc( const uint32_t in_count, const uint32_t in_alignament, const int in_initial )
{
    m_count = in_count;
    m_data = static_cast<pointer>( SDL_malloc( sizeof( _t) * m_count ) );
    if ( in_initial != 0xFFFFFFFF )
        std::memset( m_data, in_initial, sizeof( _t) * m_count );
    
    SDL_assert( m_data != nullptr );
}

template <typename _t>
inline void crvkPointer<_t>::Realloc( const uint32_t in_count, const uint32_t in_alignament )
{
    m_data = static_cast<pointer>( SDL_realloc( reinterpret_cast<void*>( m_data ), sizeof(_t) * in_count ) );
    m_count = in_count;
}

template <typename _t>
inline void crvkPointer<_t>::Free(void)
{
    if ( m_data != nullptr )
    {
        SDL_free( reinterpret_cast<void*>( m_data ) );
        m_data = nullptr;
    }
    
    m_count = 0;
}

template <typename _t>
inline void crvkPointer<_t>::Delete(void)
{
    if ( m_data != nullptr )
        reinterpret_cast<pointer>( m_data )->~_t();

    Free();
}

template <typename _t>
inline void crvkPointer<_t>::Memcpy(const_pointer in_source, const uint32_t in_offset, const uint32_t in_count)
{
    std::memcpy( &m_data[in_offset], in_source, sizeof( _t ) * in_count );
}

template <typename _t>
inline void crvkPointer<_t>::Memset( const int in_val )
{
    std::memset( m_data, in_val, sizeof( _t ) * m_count );
}

#endif //!__CRVK_POINTER_HPP__