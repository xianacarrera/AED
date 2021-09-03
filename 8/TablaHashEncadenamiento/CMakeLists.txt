cmake_minimum_required(VERSION 3.15)
project(TablaHashEncadenamiento C)

set(CMAKE_C_STANDARD 11)

add_executable(TablaHashEncadenamiento main.c lista.h lista.c tabla_hash_encadenamiento.h tabla_hash_encadenamiento.c)