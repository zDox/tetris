cmake_minimum_required(VERSION 3.26)
project(TetrisMultiplayer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

# Add the subdirectories for client and server
add_subdirectory(Client)
add_subdirectory(Server)
