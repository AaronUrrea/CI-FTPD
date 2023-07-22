cmake_minimum_required(VERSION 3.24)
project(Program_2 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Program_2
        ciftpd.c
        tftp.h)
