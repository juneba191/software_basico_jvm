//
// Created by wagner on 16/08/17.
//

#ifndef SOFTWAREBASICOJVM_UTILSNUMEROS_H
#define SOFTWAREBASICOJVM_UTILSNUMEROS_H
#include "ClassFile.h"
#include <stdint.h>


class UtilsNumeros {
public:
    //! Byte swap unsigned short
    static u2 swap_u2( u2 val );
    //! swap uint 32
    static u4 swap_u4( u4 val );
    //!swap uint 64
    static u8 swap_u8( u8 val );
};


#endif //SOFTWAREBASICOJVM_UTILSNUMEROS_H
