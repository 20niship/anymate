find_package(realsense2 QUIET)
find_package(OpenCV QUIET)

if(realsense2_FOUND)

file(GLOB MODULE_SOURCE_1 ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp) 
file(GLOB MODULE_SOURCE_2 ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp) 
set(MODULE_SRC ${MODULE_SOURCE_1} ${MODULE_SOURCE_2} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(mu_realsense SHARED ${MODULE_SRC})
target_link_libraries( 
  mu_realsense
  ${OpenCV_LIBS}
  ${realsense2_LIBRARY}
  movutl
)

target_include_directories(
  mu_realsense PUBLIC
  ${OpenCV_INCLUDE_DIR}
  ${realsense2_INCLUDE_DIR}
  ${ALL_INCLUDES}
)

else()
  message("realsense api not found")
endif()


