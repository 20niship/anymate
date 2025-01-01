#include <happly.h>

#include <movutl/core/vector.hpp>
#include <movutl/instance/instance.hpp>
#include <movutl/io/object_loader.hpp>

namespace mu::io {
#include <tuple>
#include <vector>

db::Body* impl_load_ply_happly(const char* fname) {
  DISP(fname);
  db::Body* b = instance::create_body();
  auto m      = instance::create_mesh_col();

  std::cout << "1" << std::endl;
  happly::PLYData plyIn(fname);
  /* auto camera_el    = plyIn.getElement("camera"); */
  auto vx = plyIn.getElement("vertex").getProperty<double>("x");
  auto vy = plyIn.getElement("vertex").getProperty<double>("y");
  auto vz = plyIn.getElement("vertex").getProperty<double>("z");

  std::vector<uint8_t> vr, vg, vb;
  std::vector<float> nx, ny, nz;

  const bool with_color = plyIn.getElement("vertex").hasProperty("red") && plyIn.getElement("vertex").hasProperty("blue") && plyIn.getElement("vertex").hasProperty("green");

  if(with_color) {
    vr = plyIn.getElement("vertex").getProperty<uint8_t>("red");
    vg = plyIn.getElement("vertex").getProperty<uint8_t>("green");
    vb = plyIn.getElement("vertex").getProperty<uint8_t>("blue");
  }

  std::cout << "read vertices : size = " << vx.size() << std::endl;
  std::cout << "getting information,,,,,,,," << std::endl;
  m->vertex.reserve(vx.size());
  for(size_t i = 0; i < vx.size(); i++) {
    const core::Vec3 p(vx[i], vy[i], vz[i]);
    const core::Vec3b c = with_color ? core::Vec3b(vr[i], vg[i], vb[i]) : core::Vec3b(255, 255, 255);
    /* std::cout << (int)c[0] << "\t" << (int)c[1] << "\t" << (int)c[2] << std::endl; */
    /* MU_ASSERT(vr[i] > 255); */
    /* MU_ASSERT(vg[i] > 255); */
    /* MU_ASSERT(vb[i] > 255); */
    m->vertex.push_back(std::move(db::MeshCol::Vertex(p, c)));
  }
  LOGI << "Done!";
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);

  b->meshs.push_back(m);
  return b;
}

} // namespace mu::io
  //
