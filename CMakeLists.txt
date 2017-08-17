cmake_minimum_required(VERSION 3.8)
project(SoftwareBasicoJvm)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ClassFile.cpp ClassFile.h UtilsNumeros.cpp UtilsNumeros.h Debug.h)
add_executable(SoftwareBasicoJvm ${SOURCE_FILES})