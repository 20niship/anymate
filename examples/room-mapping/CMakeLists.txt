# https://github.com/PointCloudLibrary/pcl/issues/3680
if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

find_package(PCL QUIET)

if(PCL_FOUND)
     include_directories(${PCL_INCLUDE_DIRS})
     link_directories(${PCL_LIBRARY_DIRS})
     add_definitions(${PCL_DEFINITIONS})

     add_executable(droid ${CMAKE_CURRENT_SOURCE_DIR}/droid.cpp)
     target_include_directories(droid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
     target_link_libraries(droid ${ALL_LIBS} ${PCL_LIBRARIES})

     # add_executable(pcd2ply ${CMAKE_CURRENT_SOURCE_DIR}/export_ply.cpp)
     # target_link_libraries (pcd2ply ${PCL_LIBRARIES} ${ALL_LIBS})
else()
     message  ("PCLが見つからなかったのでDroidのビルドはしません")
endif()

add_executable(normaltest ${CMAKE_CURRENT_SOURCE_DIR}/normal_test.cpp)
target_link_libraries (normaltest ${ALL_LIBS})

add_executable(boxtest ${CMAKE_CURRENT_SOURCE_DIR}/box_test.cpp)
target_link_libraries (boxtest ${ALL_LIBS})
