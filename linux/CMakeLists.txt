cmake_minimum_required(VERSION 3.10)
project(CaesarTCP C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(tcp_server server.c caesar.c)
add_executable(tcp_client client.c caesar.c)
