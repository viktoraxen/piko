cmake_minimum_required(VERSION 3.10)

project(Text)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(Text src/main.cpp src/texteditor.cpp src/keyhandler.cpp)
target_include_directories(Text PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(Text PRIVATE ncursesw)

# Source and destination directories
set(RESOURCE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/res)
set(RESOURCE_DEST_DIR ${CMAKE_BINARY_DIR}/res)

add_custom_target(
  copy_resources ALL
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${RESOURCE_DEST_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCE_SOURCE_DIR}
          ${RESOURCE_DEST_DIR}
  COMMENT "Copying resources to build directory")

# Copy resources to build directory
add_dependencies(Text copy_resources)

# Needs to be in top-level CMakeLists
enable_testing()

add_subdirectory(test)
