cmake_minimum_required( VERSION 3.16)
project(movutl C CXX)

option(BUILD_DOC "Build documentation" OFF)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

if(BUILD_DOC)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/doxygen.cmake)
endif()

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE LEGACY)

find_package(Threads REQUIRED QUIET)
find_package(glfw3 REQUIRED QUIET)
find_package(Freetype REQUIRED QUIET)
find_package(OpenCV REQUIRED QUIET)
find_package(Vulkan QUIET)
find_package(OpenGL REQUIRED QUIET)
find_package(GLEW REQUIRED QUIET)
find_package(glm REQUIRED QUIET)
find_package(assimp REQUIRED QUIET)

# https://github.com/PointCloudLibrary/pcl/issues/3680
if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()
find_package(PCL REQUIRED QUIET COMPONENTS)

include(FetchContent)
FetchContent_Declare(
  stb 
  GIT_REPOSITORY  https://github.com/nothings/stb.git
  GIT_TAG        origin/master
)

FetchContent_Declare(
  movutl-data 
  GIT_REPOSITORY  https://github.com/20niship/movutl-sample-data.git
  GIT_TAG        origin/main
)

# FetchContent_Declare(
#   cpptoml 
#   GIT_REPOSITORY  https://github.com/skystrife/cpptoml.git
#   GIT_TAG        origin/master
# )# )

FetchContent_Declare(
  happly
  GIT_REPOSITORY https://github.com/nmwsharp/happly.git 
  GIT_TAG        origin/master 
)

set (happly_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/happly-src)
include_directories(${happly_SOURCE_DIR})

message("Downloading external libraries......")
FetchContent_MakeAvailable(stb movutl-data happly)

# --------------------------------------------------------------
# ----------------     Renderer (OpenGL / Vulkan) --------------
# --------------------------------------------------------------
SET(RENDERER "WITH_NONE")
option(SET_RENDER_VULKAN "Vulkanを使用してレンダリングを行います" OFF)

if(SET_RENDER_VULKAN) 
  if(Vulkan_FOUND)
    set(RENDERER "WITH_VULKAN" )
  elseif(OpenGL_FOUND AND GLEW_FOUND)
    message("Vulkanが見つからなかったのでOpenGLを使用します")
    set(RENDERER "WITH_OPENGL")
  endif()
else()
  if(OpenGL_FOUND AND GLEW_FOUND)
    set(RENDERER "WITH_OPENGL")
  endif()
endif()

if(NOT RENDERER STREQUAL "WITH_NONE")
  message("Renderer = ${RENDERER}")
else()
  message(FATAL_ERROR "VulkanをOpenGLのどちらも見つかりませんでした。Cmakeを終了します")
endif()

add_compile_definitions(${RENDERER})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/external/spdlog)

set(ALL_LIBS 
  glfw
  ${GLFW_LIBRARY_DIRS}
  ${CMAKE_DL_LIBS}
  ${FREETYPE_LIBRARIES}
  OpenGL::GL 
  ${OpenCV_LIBRARIES}
  # ${assimp_LIBRARIES}
  assimp::assimp
  ${PCL_LIBRARIES}
)

link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

set(ALL_INCLUDES 
    ${OpenCV_INCLUDE_DIRS} 
    ${stb_SOURCE_DIR}
    ${assimp_INCLUDE_DIR}
    ${glm_INCLUDE_DIRS}
    ${FETCHCONTENT_BASE_DIR}/stb-src
    ${FETCHCONTENT_BASE_DIR}/cpptoml-src/include/
    ${FREETYPE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PCL_INCLUDE_DIRS}
    # ${spdlog_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/external/spdlog/include/
)

if(RENDERER STREQUAL "WITH_OPENGL")
  set(ALL_INCLUDES ${ALL_INCLUDES} ${GLEW_INCLUDE_DIRS} )
  set(ALL_LIBS ${ALL_LIBS} GLEW::GLEW)
else()
  set(ALL_INCLUDES ${ALL_INCLUDES} ${Vulkan_INCLUDE_DIRS} )
  set(ALL_LIBS ${ALL_LIBS} ${Vulkan_LIBRARIES})
endif()

include_directories(${ALL_INCLUDES})

add_compile_definitions(MOVUTL_DATA_DIR=\"${movutl-data_SOURCE_DIR}\")
message("movutl data dir = ${movutl-data_SOURCE_DIR}")

file(GLOB DATA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/*)
file(COPY ${DATA_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB MOVUTL_SOURCE_1 ${CMAKE_CURRENT_SOURCE_DIR}/src/movutl/*.cpp) 
file(GLOB MOVUTL_SOURCE_2 ${CMAKE_CURRENT_SOURCE_DIR}/src/movutl/**/*.cpp) 
file(GLOB MOVUTL_SOURCE_3 ${CMAKE_CURRENT_SOURCE_DIR}/src/movutl/*/*/*.cpp) 
file(GLOB MOVUTL_SOURCE_4 ${CMAKE_CURRENT_SOURCE_DIR}/src/movutl/*/*/*/*.cpp) 
set(MOVUTL_SRC ${MOVUTL_SOURCE_1} ${MOVUTL_SOURCE_2} ${MOVUTL_SOURCE_3} ${MOVUTL_SOURCE_4})
add_library(movutl ${MOVUTL_SRC})
target_link_libraries(movutl ${ALL_LIBS})
set(ALL_LIBS ${ALL_LIBS} movutl)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules.cmake)
SUBDIRLIST(sources ${PROJECT_SOURCE_DIR}/src/modules/)
foreach(path IN LISTS sources)
  get_filename_component(target ${path} NAME_WE)
  message("add module : ${target}")
  add_subdirectory(src/modules/${target})
endforeach()

add_subdirectory(tests)
add_subdirectory(examples)

add_executable(movutl-run ${CMAKE_CURRENT_SOURCE_DIR}/src/movutl/lancher/main.cpp)
target_link_libraries(movutl-run ${ALL_LIBS})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake)
