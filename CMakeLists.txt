cmake_minimum_required(VERSION 3.15)
project(stu CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Symlink compile_commands.json to root for LSP
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

add_library(stu INTERFACE)

option(STU_SINGLE_HEADER "Test against generated root stu.hpp instead of src/" OFF)
if(STU_SINGLE_HEADER)
    target_include_directories(stu INTERFACE ${PROJECT_SOURCE_DIR})
else()
    target_include_directories(stu INTERFACE ${PROJECT_SOURCE_DIR}/src)
endif()

if(PROJECT_IS_TOP_LEVEL)
    set(STU_DEFAULT_BUILD_TESTS ON)
    set(STU_DEFAULT_BUILD_EXAMPLES ON)
else()
    set(STU_DEFAULT_BUILD_TESTS OFF)
    set(STU_DEFAULT_BUILD_EXAMPLES OFF)
endif()

option(STU_BUILD_TESTS "Build tests" ${STU_DEFAULT_BUILD_TESTS})
option(STU_BUILD_EXAMPLES "Build examples" ${STU_DEFAULT_BUILD_EXAMPLES})

if(STU_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(STU_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
