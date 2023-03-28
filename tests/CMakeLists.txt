
enable_testing()

file(GLOB test_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(path IN LISTS test_files)
  get_filename_component(target ${path} NAME_WE)
  add_executable(${target} ${path} )
  target_link_libraries(${target} ${ALL_LIBS})
  add_test(NAME run_${target} COMMAND ${target})
endforeach()

