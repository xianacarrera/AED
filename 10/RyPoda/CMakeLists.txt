cmake_minimum_required(VERSION 3.15)
project(RyPoda C)

set(CMAKE_C_STANDARD 11)

add_executable(RyPoda main.c ramificacion_poda.c ramificacion_poda.h lista.c lista.h)