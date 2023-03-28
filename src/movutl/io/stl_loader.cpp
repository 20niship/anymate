#include <fstream>
#include <iostream>
#include <vector>

#include <movutl/core/vector.hpp>
#include <movutl/instance/instance.hpp>
#include <movutl/io/object_loader.hpp>


namespace mu::io {
#include <tuple>
#include <vector>


// 座標
struct coord {
  float x, y, z;
};

// ポリゴン
struct polygon {
  // 頂点座標
  coord v0, v1, v2;
  // 法線ベクトル
  coord normal;
  // セット
  void set(coord v0_, coord v1_, coord v2_, coord normal_) {
    v0     = v0_;
    v1     = v1_;
    v2     = v2_;
    normal = normal_;
  }
};

db::Body* impl_load_stl(const char* fname) {
  DISP(fname);

  std::ifstream fin(fname, std::ios::in | std::ios::binary);
  if(!fin) {
    LOGE << "file is not found." << fname;
    return nullptr;
  }

  db::Body* b = instance::create_body();
  auto m      = instance::create_mesh_3d();
  MU_ASSERT(m);

  // ポリゴン数の読み込み
  unsigned int num_polygons;
  fin.seekg(80, std::ios_base::beg);
  fin.read((char*)&num_polygons, sizeof(unsigned int));

  DISP(num_polygons);

  // 各ポリゴンの法線と座標の読み込み
  m->vertex.resize(num_polygons * 3);

  for(size_t i = 0; i < num_polygons; i++) {
    core::Vec3f norm;

    fin.read((char*)&norm[0], sizeof(float));
    fin.read((char*)&norm[1], sizeof(float));
    fin.read((char*)&norm[2], sizeof(float));

    for(int j = 0; j < 3; j++) {
      core::Vec3f p;
      fin.read((char*)&p[0], sizeof(float));
      fin.read((char*)&p[1], sizeof(float));
      fin.read((char*)&p[2], sizeof(float));
      m->vertex[i*3+j].pos[0] = p[0];
      m->vertex[i*3+j].pos[1] = p[1];
      m->vertex[i*3+j].pos[2] = p[2];
      m->vertex[i*3+j].norm[0]= norm[0];
      m->vertex[i*3+j].norm[1]= norm[1];
      m->vertex[i*3+j].norm[2]= norm[2];

      DISP(core::Vec2(i, j));
      DISP(p);
      DISP(norm);
    }
    fin.seekg(2, std::ios_base::cur);
  }

  fin.close();
  m->primitive_type(db::_MeshBase::PrimitiveType::TRIANGLES);
  b->meshs.push_back(m);
  return b;
}

} // namespace mu::io
