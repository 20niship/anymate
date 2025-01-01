cmake_minimum_required( VERSION 3.16)
project(movutl C CXX)

option(BUILD_DOC "Build documentation" OFF)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

if(BUILD_DOC)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/doxygen.cmake)
endif()

add_subdirectory(movutl)

add_subdirectory(tests)
add_subdirectory(examples)

add_executable(movutl ${CMAKE_CURRENT_SOURCE_DIR}/lancher/main.cpp)
target_link_libraries(movutl ${MOVUTL_LIBRARIES})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake)

