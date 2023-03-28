#pragma once
#include <movutl/core/rect.hpp>
#include <movutl/db/body.hpp>
#include <movutl/db/collection.hpp>
#include <vector>

namespace mu::plugin {

using MU_Handle = void*;
using MU_Char   = const char*;

struct PluginParams {
  enum class ParamType {
    RESERVED = -1,
    LAYER    = 0,
    DOUBLE,
    VEC2,
    VEC3,
    VEC4,
    COLOR_RGB,
    COLOR_RGBA,
    BOOL,
    ARBITARY,
    POINT2D,
    POINT3D,
    RECT,
    RECT3D,
  };

  union ParamDefUnion {
    db::Collection* l;
    double d;
    core::Vec2 v2;
    core::Vec3 v3;
    core::Vec4 v4;
    core::Vec3b c3;
    core::Vec4b c4;
    bool b;
    void* v;
    core::Rect r;
    core::Rect3D r3;
    ParamDefUnion() {}
    ParamDefUnion(double v_) { d = v_; }
    ParamDefUnion(core::Vec2 v_) { v2 = v_; }
    ParamDefUnion(core::Vec3 v_) { v3 = v_; }
    ParamDefUnion(core::Vec4 v_) { v4 = v_; }
    ParamDefUnion(core::Vec3b v_) { c3 = v_; }
    ParamDefUnion(core::Vec4b v_) { c4 = v_; }
    ParamDefUnion(core::Rect v_) { r = v_; }
    ParamDefUnion(core::Rect3D v_) { r3 = v_; }
    ParamDefUnion(void* v_) { v = v_; }
    ~ParamDefUnion() = default;
  };

  using ParamName = const char*;

  struct PluginParam {
    const char* name;
    ParamType t;
    ParamDefUnion u;
    PluginParam() = default;
    PluginParam(const char* name_, const ParamType t_, const ParamDefUnion u_) {
      name = name_;
      t    = t_;
      u    = u_;
    }
  };
  std::vector<PluginParam> p;
  const char* name;
  void add_param(const char* name, double v) { p.push_back(PluginParam(name, ParamType::DOUBLE, v)); }
  void add_param(const char* name, core::Vec2 v) { p.push_back(PluginParam(name, ParamType::VEC2, v)); }
  void add_param(const char* name, core::Vec3 v) { p.push_back(PluginParam(name, ParamType::VEC3, v)); }
  void add_param(const char* name, core::Vec4 v) { p.push_back(PluginParam(name, ParamType::VEC4, v)); }
  void add_param(const char* name, core::Vec3b v) { p.push_back(PluginParam(name, ParamType::COLOR_RGB, v)); }
  void add_param(const char* name, core::Vec4b v) { p.push_back(PluginParam(name, ParamType::COLOR_RGBA, v)); }
  void add_param(const char* name, const core::Rect& v) { p.push_back(PluginParam(name, ParamType::RECT, v)); }
  void add_param(const char* name, core::Rect3D v) { p.push_back(PluginParam(name, ParamType::RECT3D, v)); }
  void add_param(const char* name, bool v) { p.push_back(PluginParam(name, ParamType::BOOL, v)); }
  bool has_param(const char* name) {
    return std::find_if(p.begin(), p.end(), [=](auto x) { return strcmp(x.name, name) == 0; }) == p.end();
  }
  bool has_param(const char* name, const ParamType t) {
    return std::find_if(p.begin(), p.end(), [=](auto x) { return strcmp(x.name, name) == 0 && x.t == t; }) == p.end();
  }
};

/**
 * Pluginに関するクラス。Shared libとしての運用も考えるため、仮想関数を使ったクラスでプラグインを実装する
 */

struct PluginInfo {
  enum class PluginType {
    Importer,
    Exporter,
    Modifier,
    UI,
    Material,
    Fx,
    Audio,
  };
  int major_version;
  int minor_version;
  MU_Char name;
  MU_Char description;
  MU_Char vendor_name;
  MU_Char url;
  bool is_experimental;
  PluginType type;
};

class PluginBase {
public:
  PluginBase()                                    = default;
  virtual PluginInfo get_plugin_info()            = 0; /// get version
  virtual void apply(db::Body* in, db::Body* out) = 0; /// process in Body data and store to out Body data;
  virtual void registered()                       = 0; /// calls on register
  virtual void unregistered()                     = 0; /// calls on un register
  virtual void set_param(const PluginParams& p)   = 0; /// calls on un register
  virtual PluginParams get_param() const          = 0; /// calls on un register
};


} // namespace mu::plugin
