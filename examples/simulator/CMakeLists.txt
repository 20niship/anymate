add_executable(sim simulator.cpp simulator.hpp) 
target_link_libraries(sim ${ALL_LIBS})

FILE(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(COPY env.toml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
