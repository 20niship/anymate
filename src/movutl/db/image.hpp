#pragma once

#include <istream>
#ifdef WITH_OPENGL // CMakeLists.txtをみてね
#define GLEW_STATIS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <movutl/core/vector.hpp>
#include <movutl/db/block.hpp>

namespace mu::db {

class Image : public Block {
public:
  enum class Format {
    RGB       = GL_RGB,
    GRAYSCALE = GL_RED,
    RGBA      = GL_RGBA,
    BGRA      = GL_BGRA,
    BGR       = GL_BGR,
  };

  enum class Filter { NEAREST = GL_NEAREST, LINEAR = GL_LINEAR };

private:
  GLuint tex_id;
  unsigned int _width, _height;
  Format _format;
  uint8_t* _data;
  size_t capacity;
  Filter _filter;
  GLuint _type;
  const char* _name;
  const char* _path;
  bool _gpu_updated;

  void reserve(size_t new_capacity) {
    if(new_capacity <= capacity) return;
    uint8_t* new_data = (uint8_t*)malloc(new_capacity * sizeof(uint8_t));
    assert(new_data);
    if(_data) {
      memcpy(new_data, _data, capacity * sizeof(uint8_t));
      free(_data);
    }
    _data    = new_data;
    capacity = new_capacity;
  }

public:
  Image();
  Image(const char* image_file_name);
  Image(int w, int h, const Format format);
  Image(int w, int h, uint8_t* data, const Format format);
  Image(int w, int h, uint8_t* data, Format format, Filter filter = Filter::NEAREST);
  Image(int w, int h, uint8_t* data, Format format, Filter filter = Filter::NEAREST, GLuint type = GL_BITMAP);
  ~Image();

  // テクスチャ作成する
  // format = GL_RGB : カラー画像、GL_COLOR_INDEX：単一の値で構成されるカラー指標
  // filter_type = GL_NEAREST, GL_LINEARがある
  bool set_to_gpu();
  bool gpu_updated() { return _gpu_updated; }
  void set_data(uint8_t* data) { _data = data; }
  void set_format(Format f) { _format = f; }
  void set_filter(const Filter f);
  void set_type(const GLuint t) { _type = t; }
  bool isvalid();
  /* void set_type(const Type f); */

  GLuint loadTextureFromFile(std::string filename);
  void erase();
  GLuint get_tex_id() { return tex_id; }
  uint8_t* data() { return _data; }
  unsigned int width() const { return _width; }
  unsigned int height() const { return _height; }
  void width(int w) { _width = w; }
  void height(int h) { _height = h; }
  size_t size() const { return _width * _height; }
  void path(const char* p) { _path = p; }
  auto path() { return _path; }
  void texture_name(const char* n) { _name = n; }
  auto texture_name() { return _name; }
  uint8_t* get_data_from_gpu();

  void reset() {
    _width   = -1;
    _height  = -1;
    _data    = nullptr;
    capacity = 0;
  }
  void fill(const uint8_t& v) {
    for(size_t i = 0; i < size(); i++) _data[i] = v;
  }

  void resize(const core::Vec2d& size) {
    MU_ASSERT(size[0] > 0 && size[1] > 0);
    _width  = size[0];
    _height = size[1];
    reserve(_width * _height);
  }
  void resize(const int _w, const int _h) { resize({_w, _h}); }

  // データへのポインタを指定する。実体はmainプログラムで持っておく
  void setPtr(size_t w, size_t h, void* buf) {
    if(_data != nullptr) {
      free(_data);
    }
    _width  = w;
    _height = h;
    _data   = (uint8_t*)buf;
  }

  const uint8_t& operator[](size_t i) const {
    MU_ASSERT(i < capacity);
    return _data[i];
  }
  uint8_t& operator[](size_t i) {
    assert(i < capacity);
    return _data[i];
  }
  const uint8_t& operator()(const size_t x, const size_t y) const {
    MU_ASSERT(x * y < capacity);
    return _data[y * _width + x];
  }
  uint8_t& operator()(const size_t x, const size_t y) {
    MU_ASSERT(x * y < capacity);
    return _data[y * _width + x];
  }
};

} // namespace mu::db
