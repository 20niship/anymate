
#pragma once
#include <movutl/core/vector.hpp>
#include <movutl/db/block.hpp>
#include <string>

#ifdef WITH_VULKAN
#include <vulkan/vulkan.hpp>
#endif

namespace mu::db {

class Shader {
private:
  bool compiled;
  char *fs_path, vs_path, gs_path;
  const char *fs_src, *vs_src, *gs_src;
#ifdef WITH_VULKAN
  vk::UniqueShaderModule vx, fs;
#else
  unsigned int vs, fs, shader_id;
#endif

public:
  bool compile();
  void use();
  bool enabled() { return compiled; }
  Shader();
  Shader(const char* vs, const char* fs);
  ~Shader();
  void set_shader_str(const char* vs, const char* fs);
  void set_shader_file(const char* vs_path, const char* fs_path);

  void set_default_ui_shader_src();
  void set_default_3dcol_shader_src();
  void set_default_default_shader_src();
  unsigned int get_shader_id();

  int get_uni_loc(const char* name);
  int get_attrib_loc(const char* name);
  const char *get_vs_src(){ return vs_src; }
  const char *get_fs_src(){ return fs_src; }

  bool set_value(const char* name, float value);
  bool set_value(const char* name, const core::Mat4x4f& value);
  bool set_value(const char* name, core::Vec2f value);
  bool set_value_img(const char* name, int texture_id, int value);
};

} // namespace mu::db
