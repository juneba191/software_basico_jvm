//
// Created by wagner on 16/08/17.
//

#ifndef SOFTWAREBASICOJVM_UTILSNUMEROS_H
#define SOFTWAREBASICOJVM_UTILSNUMEROS_H


#include <stdint.h>


class UtilsNumeros {
public:
    //! Byte swap unsigned short
    static uint16_t swap_uint16( uint16_t val );
    //!swap iint 16
    static int16_t swap_int16( int16_t val );
    //! swap uint 32
    static uint32_t swap_uint32( uint32_t val );
    //! swap int 32
    static int32_t swap_int32( int32_t val );
    //! swap int 64
    static  int64_t swap_int64( int64_t val );
    //!swap uint 64
    static uint64_t swap_uint64( uint64_t val );

};


#endif //SOFTWAREBASICOJVM_UTILSNUMEROS_H
