#include "UtilsNumeros.h"

//! Byte swap unsigned short
u2 UtilsNumeros::swap_u2( u2 val )
{
    return (val << 8) | (val >> 8 );
}
//! Byte swap unsigned int
u4 UtilsNumeros::swap_u4( u4 val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

u8 UtilsNumeros::swap_u8( u8 val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
}