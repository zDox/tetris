cmake_minimum_required(VERSION 3.26)

project(TetrisServer)

# Binary paths
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# CMake Settings
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Libs Header dirs
set(YOJIMBO_DIR ../External/yojimbo)
set(ZPP_BITS_DIR ../External/zpp_bits)
set(COMMON_DIR ../Common)


aux_source_directory(src SOURCES)
aux_source_directory(include HEADERS)

add_executable(TetrisServer ${SOURCES} ${HEADERS})

# Specify the include directories
target_include_directories(TetrisServer PUBLIC include)
target_include_directories(TetrisServer PUBLIC ${YOJIMBO_DIR})
target_include_directories(TetrisServer PUBLIC ${ZPP_BITS_DIR})
target_include_directories(TetrisServer PUBLIC ${COMMON_DIR}/include)

# Link any necessary libraries (e.g., SDL or SFML)
target_link_libraries(TetrisServer -lyojimbo -lsodium -lmbedtls -lmbedx509 -lmbedcrypto)
target_link_libraries(TetrisServer -lsfml-system)

