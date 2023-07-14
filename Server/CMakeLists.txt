cmake_minimum_required(VERSION 3.26)

project(TetrisServer)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(YOJIMBO_DIR ../External/yojimbo)
set(COMMON_DIR ../Common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

aux_source_directory(src SOURCES)

aux_source_directory(include HEADERS)


add_executable(TetrisServer ${SOURCES} ${HEADERS})

# Specify the include directories
target_include_directories(TetrisServer PUBLIC include)
target_include_directories(TetrisServer PUBLIC ${YOJIMBO_DIR})
target_include_directories(TetrisServer PUBLIC ${COMMON_DIR}/include)

# Link any necessary libraries (e.g., SDL or SFML)
target_link_libraries(TetrisServer -lyojimbo -lsodium -lmbedtls -lmbedx509 -lmbedcrypto)
target_link_libraries(TetrisServer -lsfml-system)

