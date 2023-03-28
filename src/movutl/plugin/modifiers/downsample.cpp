#include <movutl/plugins/plugin_base.hpp>

namespace mu::plugin {

#if 0
static copy_body(db::Body* in, db::Body* out) {
  in->meshs    = out->meshs;
  in->textures = out->textures;
  in->pos      = out->pos;
  in->bones    = out->bones;
}

class DownSamplePlugin : PluginBase {

  virtual PluginInfo get_plugin_info() {
    PluginInfo info;
    info.major_version = 0;
    info.minor_version = 0;
    info.name          = "downsampling";
    info.description   = "downsamples meshes of object";
    info.type          = PluginInfo::PluginType::Modifier;
    return info;
  }

  virtual void registered() { std::cout << "registerd!" << std::endl; }
  virtual void unregistered() { std::cout << "unregisterd!" << std::endl; }

  virtual void apply(db::Body* in, db::Body* out) {
    *out   = *in;
    auto m = in->meshs;
    if(m.size() == 0) return;
    for(auto&& m_ : m) {
      db::_MeshBase outmesh;
    }
  }
};

#endif

} // namespace mu::plugin
