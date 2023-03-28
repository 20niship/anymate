# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/simulator)

file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(path IN LISTS sources)
  get_filename_component(target ${path} NAME_WE)
  add_executable(${target} ${path} )
target_link_libraries(${target} ${ALL_LIBS})
endforeach()

