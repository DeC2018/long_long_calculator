cmake_minimum_required(VERSION 3.28)
project(long_long_calculator C)

set(CMAKE_C_STANDARD 17)

add_executable(long_long_calculator calc.c
        bigint.h
        bigint.c)
