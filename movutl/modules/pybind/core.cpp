#include <iostream>
#include <movutl/core/core.hpp>
#include "pybind.hpp"

using namespace mu::core;

void bind_core_module(pybind11::module &m) {
  pybind11::module core= m.def_submodule("core");
  core.doc() = "pybind11 example plugin";
  core.def("red_to_rag", &rad_to_deg<float>, "convert radian to degrees");
  core.def("deg_to_rad", &deg_to_rad<float>, "convert degrees to radian");
  core.def("lookAt", &lookAt, "opengl lookAt function");

  auto vec3 =  pybind11::class_<Vec3>(core, "vec3");
  vec3.def(pybind11::init<>());
  /* vec3.def_property("values", &Vec3::value, "values"); */
  vec3.def("display", &Vec3::display, "display ");
  vec3.def("zero", &Vec3::zero, "zero reset");
  vec3.def("one", &Vec3::ones, "set all values to one");
}

