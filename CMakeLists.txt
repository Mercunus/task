cmake_minimum_required(VERSION 3.20)

project(message_server VERSION 1.0)
project(message_client VERSION 1.0)

add_executable(message_server server.c)
add_executable(message_client client.c)