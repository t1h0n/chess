cmake_minimum_required(VERSION 3.0.0)
project(chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chess src/main.cpp src/Board.cpp src/Pieces.cpp)
target_include_directories(chess PUBLIC ${CMAKE_SOURCE_DIR}/include
                                        ${CMAKE_SOURCE_DIR}/std_extensions)
target_link_libraries(chess -lsfml-graphics -lsfml-window -lsfml-system)
