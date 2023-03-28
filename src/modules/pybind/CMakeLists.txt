# sudo apt install python3.6-dev で事前にインストール必要
find_package(PythonLibs QUIET)
find_package(pybind11 QUIET)

if(pybind11_FOUND)

file(GLOB PYBIND_SOURCE_1 ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp) 
file(GLOB PYBIND_SOURCE_2 ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp) 
set(PYBIND_SRC ${PYBIND_SOURCE_1} ${PYBIND_SOURCE_2} )

pybind11_add_module(movutlpy ${PYBIND_SRC} ${MOVUTL_SRC})

target_include_directories(
  movutlpy 
  PUBLIC
  ${Python_INCLUDE_DIRS}
  ${Python_INCLUDE_DIR}
  ${pybind11_INCLUDE_DIRS}
  ${pybind11_INCLUDE_DIR}
  ${ALL_INCLUDES}
)

target_link_libraries(
  movutlpy 
  PUBLIC
  # movutl
  ${ALL_LIBS}
)

message("pybind build!!")
else()
  message("pybindがなかった")
endif()

# add_libraryだとうまく行かな語ったので、pybind11_add_module()を使用する
# $ c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) example.cpp -o example$(python3-config --extension-suffix)
# ( reference : https://pybind11.readthedocs.io/en/stable/basics.html )を参考にすると良さそう

