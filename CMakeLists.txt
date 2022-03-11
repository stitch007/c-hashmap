cmake_minimum_required(VERSION 3.21)
project(hashmap C)

set(CMAKE_C_STANDARD 11)

add_executable(hashmap main.c hashmap.c hashmap.h)
