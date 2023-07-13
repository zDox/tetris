cmake_minimum_required(VERSION 3.26)

project(TetrisClient)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})


set(CMAKE_CXX_STANDARD 20)

aux_source_directory(src SOURCES)

aux_source_directory(include HEADERS)

add_executable(TetrisClient ${SOURCES} ${HEADERS})

# Specify the include directories
target_include_directories(TetrisClient PUBLIC include)

# Link any necessary libraries (e.g., SDL or SFML)
target_link_libraries(TetrisClient -ltgui -lsfml-graphics -lsfml-window -lsfml-system)

