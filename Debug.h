//
// Created by wagner on 16/08/17.
//

#ifndef SOFTWAREBASICOJVM_DEBUG_H
#define SOFTWAREBASICOJVM_DEBUG_H
#define USEDEBUG 1

#ifdef USEDEBUG
#define Debug(x) std::cout << "Debug: " x << "\n"
#else
#define Debug(x)

#endif

#endif //SOFTWAREBASICOJVM_DEBUG_H



