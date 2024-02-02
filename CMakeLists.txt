cmake_minimum_required(VERSION 3.15)
project(TrabalhoPratico C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wextra")

add_executable(TrabalhoPratico main.c listas.c variaveis.h)