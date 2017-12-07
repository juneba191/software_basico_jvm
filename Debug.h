//
// Created by wagner on 16/08/17.
//

#ifndef SOFTWAREBASICOJVM_DEBUG_H
#define SOFTWAREBASICOJVM_DEBUG_H
#define USEDEBUG 0

#ifdef USEDEBUG
#define Debug(x) std::cout << x << std::endl;
#else
#define Debug(x)

#endif

#endif //SOFTWAREBASICOJVM_DEBUG_H



