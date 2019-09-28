cmake_minimum_required(VERSION 3.10)
project(dlskeleton C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(dlskeleton
        randall.c
        randcpuid.h
        randlib.h
        randcpuid.c
        randlibhw.c
        randlibsw.c
        randmain.c)
