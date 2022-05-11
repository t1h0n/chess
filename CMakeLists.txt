cmake_minimum_required(VERSION 3.14.0)
project(chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_INIT "-Wpedantic -Wall") # fix this

include(cmake/chess.sources)
add_library(chess_main ${SOURCES})
target_include_directories(chess_main PUBLIC include std_extensions)

# Copy resources file
add_custom_target(
  copy_resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/resources
          ${PROJECT_BINARY_DIR}/resources
  COMMENT "Copying resources into binary directory")

# Copying resources is a dependency of backend TODO: should it realy be a
# dependency? should prevous directory be first removed?
add_dependencies(chess_main copy_resources)

add_executable(${PROJECT_NAME} src/main.cpp)
find_package(
  SFML 2.5
  COMPONENTS graphics window system
  REQUIRED)
target_link_libraries(${PROJECT_NAME} chess_main sfml-graphics sfml-window
                      sfml-system)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 703bd9caab50b139428cea1aaff9974ebee5742e)
FetchContent_MakeAvailable(googletest)

set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

enable_testing()
include(cmake/test.sources)
add_executable(chess_test ${TEST_SOURCES})

target_link_libraries(chess_test gtest_main chess_main)
include(GoogleTest)
gtest_discover_tests(chess_test)
