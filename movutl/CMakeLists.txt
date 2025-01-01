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

if(APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(JSONCPP jsoncpp)
  find_package(JsonCpp  QUIET REQUIRED)
  if(NOT JSONCPP_FOUND)
    set(JsonCpp::JsonCpp jsoncpp)
  endif()

  # Appleでは何もしないとOpenMPが見つからないので、Homebrewのパスを指定する
  # https://zenn.dev/arawii/articles/47a24cf2d0a3b2
  cmake_policy(SET CMP0074 NEW) # OpenMP_ROOTの環境変数を使用してパッケージを探す
  set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
endif()


# --------------------------------------------------------------
# ----------------     Renderer (OpenGL / Vulkan) --------------
# --------------------------------------------------------------
set(RENDERER "WITH_OPENGL")
add_compile_definitions(${RENDERER})

set(MOVUTL_LIBRARIES 
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
    ${CMAKE_CURRENT_SOURCE_DIR}/movutl/external/stb/
    ${assimp_INCLUDE_DIR}
    ${glm_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${PCL_INCLUDE_DIRS}
    # ${spdlog_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/movutl
    ${CMAKE_CURRENT_SOURCE_DIR}/movutl/external/loguru/include/
)

set(ALL_INCLUDES ${ALL_INCLUDES} ${GLEW_INCLUDE_DIRS} )
set(MOVUTL_LIBRARIES ${MOVUTL_LIBRARIES} GLEW::GLEW)
include_directories(${ALL_INCLUDES})

add_compile_definitions(MOVUTL_DATA_DIR=\"${movutl-data_SOURCE_DIR}\")
message("movutl data dir = ${movutl-data_SOURCE_DIR}")

file(GLOB DATA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/*)
file(COPY ${DATA_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB MOVUTL_SOURCE_1 ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp) 
file(GLOB MOVUTL_SOURCE_2 ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp) 
file(GLOB MOVUTL_SOURCE_3 ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*.cpp) 
file(GLOB MOVUTL_SOURCE_4 ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*/*.cpp) 
set(MOVUTL_SRC ${MOVUTL_SOURCE_1} ${MOVUTL_SOURCE_2} ${MOVUTL_SOURCE_3} ${MOVUTL_SOURCE_4})
add_library(mucore ${MOVUTL_SRC})
target_link_libraries(mucore ${MOVUTL_LIBRARIES})
set(MOVUTL_LIBRARIES ${MOVUTL_LIBRARIES} movutl)

