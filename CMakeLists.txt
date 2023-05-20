cmake_minimum_required(VERSION 3.0.0)
project(cplus_study VERSION 0.1.0)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

include(CTest)
enable_testing()

add_executable(address address.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
