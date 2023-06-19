cmake_minimum_required(VERSION 3.0.0)
project(cplus_study VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options("-Wall")

include(CTest)
enable_testing()

add_executable(address address.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(memory)
add_subdirectory(template)