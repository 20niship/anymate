//#include <pybind11/pybind11.hpp>
#include "pybind.hpp"

PYBIND11_MODULE(mymodule, m) {
  bind_core_module(m);
  // bind_render_module(m);
}

