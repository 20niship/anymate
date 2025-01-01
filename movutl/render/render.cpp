#include <codecvt>
#include <iostream>
#include <locale>
#include <string>
#include <utility>

#include <movutl/instance/instance.hpp>
#include <movutl/render/render.hpp>
#include <movutl/ui/colors.hpp>

namespace mu::render {

Render::Render() {
  mesh_col = nullptr;
  mesh_3d  = nullptr;
  mesh_ui  = nullptr;
  shader   = nullptr;
  wnd      = nullptr;
}

void Render::clear_not_default_mesh() {
  meshes.clear();
  if(mesh_3d != nullptr) meshes.push_back(mesh_3d);
  if(mesh_ui != nullptr) meshes.push_back(mesh_ui);
  if(mesh_col != nullptr) meshes.push_back(mesh_col);
}

void Render::add_mesh(db::Mesh* m) {
  MU_ASSERT(m != nullptr);
  MU_ASSERT(m->get_mesh_type() == db::_MeshBase::MeshType::Object);
  meshes.push_back(m);
}
void Render::add_mesh(db::MeshCol* m) {
  MU_ASSERT(m != nullptr);
  MU_ASSERT(m->get_mesh_type() == db::_MeshBase::MeshType::Col);
  meshes.push_back(m);
  if(mesh_col == nullptr) mesh_col = m;
}
void Render::add_mesh(db::Mesh2D* m) {
  MU_ASSERT(m != nullptr);
  MU_ASSERT(m->get_mesh_type() == db::_MeshBase::MeshType::UI);
  meshes.push_back(m);
  if(mesh_ui == nullptr) mesh_ui = m;
}

void Render::add_body(db::Body* b) {
  MU_ASSERT(b != nullptr);
  bodies.push_back(b);
}

void Render::create_default_mesh() {
  add_mesh(instance::create_mesh_2d());
  add_mesh(instance::create_mesh_col());
  add_mesh(instance::create_mesh_3d());
}

void Render::triangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3b& col1, const Vec3b& col2, const Vec3b& col3) {
  MU_ASSERT(mesh_col != nullptr);
  mesh_col->add_point(pos1, col1);
  mesh_col->add_point(pos2, col2);
  mesh_col->add_point(pos3, col3);
}

void Render::triangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3b& col1, const Vec3b& col2, const Vec3b& col3, const float width) {
  MU_ASSERT(mesh_col != nullptr);
  line(pos1, pos2, col1, col2, width);
  line(pos2, pos3, col2, col3, width);
  line(pos3, pos1, col3, col1, width);
}

void Render::line(const Vec3& p1, const Vec3& p2, const Vec3b& col1, const Vec3b& col2, const float width) {
  MU_ASSERT(mesh_col != nullptr);
  auto dp = p2 - p1;
  dp      = dp / dp.norm();
  Vec3 e1, e2;
  if(dp[0] >= dp[1] && dp[0] >= dp[2]) { // X軸に近い直線
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[2], dp[1], dp[0]};
  } else if(dp[1] >= dp[0] && dp[1] >= dp[2]) {
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[0], dp[2], dp[1]};
  } else {
    e1 = {dp[2], dp[1], dp[0]};
    e2 = {dp[0], dp[2], dp[1]};
  }
  e1 *= width;
  e2 *= width;
  quad(p1, p1 + e1, p2 + e1, p2, col1, col1, col2, col2);
  quad(p1, p1 + e2, p2 + e2, p2, col1, col1, col2, col2);
  const auto p3 = p1 + e1 + e2;
  const auto p4 = p2 + e1 + e2;
  quad(p3, p3 - e1, p4 - e1, p4, col1, col1, col2, col2);
  quad(p3, p3 - e2, p4 - e2, p4, col1, col1, col2, col2);
}

void Render::line(const Vec3& pos1, const Vec3& pos2, const Vec3b& col, const float width) {
  MU_ASSERT(mesh_col != nullptr);
  line(pos1, pos2, col, col, width);
}

void Render::triangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3b& col, const float width) { triangle(pos1, pos2, pos3, col, col, col, width); }

void Render::triangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3b& col) { triangle(pos1, pos2, pos3, col, col, col); }

/* inline void AddTriangle(const Vec3 &pos1, const Vec3 &pos2, const Vec3 &pos3, const Vec2 uv1, const Vec2 &uv2, const Vec2 &uv3, const Vec3b &col){ */
/*   __AddPointSizeZero(pos1, col); */
/*   __AddPointSizeZero(pos2, col); */
/*   __AddPointSizeZero(pos3, col); */
/* } */

void Render::quad(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3& pos4, const Vec3b& col, const float width) {
  line(pos1, pos2, col, width);
  line(pos2, pos3, col, width);
  line(pos3, pos4, col, width);
  line(pos4, pos1, col, width);
}

void Render::quad(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p4, const Vec3b& c1, const Vec3b& c2, const Vec3b& c3, const Vec3b& c4, const float width) {
  line(p1, p2, c1, c2, width);
  line(p2, p3, c2, c3, width);
  line(p3, p4, c3, c4, width);
  line(p4, p1, c4, c1, width);
}

void Render::quad(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p4, const Vec3b& c1, const Vec3b& c2, const Vec3b& c3, const Vec3b& c4) {
  triangle(p1, p2, p3, c1, c2, c3);
  triangle(p1, p3, p4, c1, c3, c4);
}

void Render::quad(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3, const Vec3& pos4, const Vec3b& col) {
  triangle(pos1, pos2, pos3, col);
  triangle(pos1, pos3, pos4, col);
}

void Render::rect(const Vec3& pos, const Vec2& size, const Vec3& normal, const Vec3b& col) {
  const auto hs = size / 2;
  const auto dp = normal.normalize();
  Vec3 e1, e2;
  if(dp[0] >= dp[1] && dp[0] >= dp[2]) { // X軸に近い直線
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[2], dp[1], dp[0]};
  } else if(dp[1] >= dp[0] && dp[1] >= dp[2]) {
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[0], dp[2], dp[1]};
  } else {
    e1 = {dp[2], dp[1], dp[0]};
    e2 = {dp[0], dp[2], dp[1]};
  }
  e1 = dp.cross(e1);
  e1 = e1.normalize();
  e2 = dp.cross(e1);
  e2 = e2.normalize();
  quad(pos + Vec3{-hs[0], hs[1], 0}, pos + Vec3{-hs[0], -hs[1], 0}, pos + Vec3{hs[0], -hs[1], 0}, pos + Vec3{hs[0], hs[1], 0}, col);
}

void Render::plane(const Vec3& pos, const float size, const Vec3& normal, const Vec3b& col) {
  const auto hs = size / 2;
  const auto dp = normal.normalize();
  Vec3 e1, e2;
  if(dp[0] >= dp[1] && dp[0] >= dp[2]) { // X軸に近い直線
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[2], dp[1], dp[0]};
  } else if(dp[1] >= dp[0] && dp[1] >= dp[2]) {
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[0], dp[2], dp[1]};
  } else {
    e1 = {dp[2], dp[1], dp[0]};
    e2 = {dp[0], dp[2], dp[1]};
  }
  e1                 = dp.cross(e1);
  e1                 = e1.normalize();
  e2                 = dp.cross(e1);
  e2                 = e2.normalize();
  const Vec3 poses[] = {
    pos + e1 * hs + e2 * hs,
    pos + e1 * hs - e2 * hs,
    pos - e1 * hs + e2 * hs,
    pos - e1 * hs - e2 * hs,
  };
  quad(poses[0], poses[1], poses[2], poses[3], col);
}

void Render::plane(const Vec3& pos, const float size, const Vec3& normal, const Vec3b& col, float width) {
  const auto hs = size / 2;
  const auto dp = normal.normalize();
  Vec3 e1, e2;
  if(dp[0] >= dp[1] && dp[0] >= dp[2]) { // X軸に近い直線
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[2], dp[1], dp[0]};
  } else if(dp[1] >= dp[0] && dp[1] >= dp[2]) {
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[0], dp[2], dp[1]};
  } else {
    e1 = {dp[2], dp[1], dp[0]};
    e2 = {dp[0], dp[2], dp[1]};
  }
  e1                 = dp.cross(e1);
  e1                 = e1.normalize();
  e2                 = dp.cross(e1);
  e2                 = e2.normalize();
  const Vec3 poses[] = {
    pos + e1 * hs + e2 * hs,
    pos + e1 * hs - e2 * hs,
    pos - e1 * hs - e2 * hs,
    pos - e1 * hs + e2 * hs,
  };
  quad(poses[0], poses[1], poses[2], poses[3], col, width);
}

// draw cyliinder surface
void Render::cylinder(const Vec3& p, const Vec3& n, const double r, const Vec3b& col) {
  constexpr int N = 20; // 側面分割数
  const auto dp   = n.normalize();
  Vec3 e1, e2;
  if(dp[0] >= dp[1] && dp[0] >= dp[2]) { // X軸に近い直線
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[2], dp[1], dp[0]};
  } else if(dp[1] >= dp[0] && dp[1] >= dp[2]) {
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[0], dp[2], dp[1]};
  } else {
    e1 = {dp[2], dp[1], dp[0]};
    e2 = {dp[0], dp[2], dp[1]};
  }

  e1 = dp.cross(e1);
  e1 = e1.normalize();
  e2 = dp.cross(e1);
  e2 = e2.normalize();
  for(int i = 0; i < N + 1; i++) {
    const double theta1 = 6.28 * (float)i / N;
    const double theta2 = 6.28 * (float)(i + 1) / N;
    const auto v1       = (e1 * std::cos(theta1) + e2 * std::sin(theta1)) * r;
    const auto v2       = (e1 * std::cos(theta2) + e2 * std::sin(theta2)) * r;
    quad(p - n / 2 + v1, p - n / 2 + v2, p + n / 2 + v2, p + n / 2 + v1, col);
    triangle(p - n / 2, p - n / 2 + v2, p - n / 2 + v1, col);
    triangle(p + n / 2, p + n / 2 + v1, p + n / 2 + v2, col);
  }
}


void Render::cylinder(const Vec3& p, const Vec3& n, const double r, const Vec3b& col, const float width) {
  constexpr int N = 20; // 側面分割数
  const auto dp   = n.normalize();
  Vec3 e1, e2;
  if(dp[0] >= dp[1] && dp[0] >= dp[2]) { // X軸に近い直線
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[2], dp[1], dp[0]};
  } else if(dp[1] >= dp[0] && dp[1] >= dp[2]) {
    e1 = {dp[1], dp[0], dp[2]};
    e2 = {dp[0], dp[2], dp[1]};
  } else {
    e1 = {dp[2], dp[1], dp[0]};
    e2 = {dp[0], dp[2], dp[1]};
  }
  e1 = dp.cross(e1);
  e1 = e1.normalize();
  e2 = dp.cross(e1);
  e2 = e2.normalize();
  for(int i = 0; i < N + 1; i++) {
    const double theta1 = 6.28 * (float)i / N;
    const double theta2 = 6.28 * (float)(i + 1) / N;
    const auto v1       = (e1 * std::cos(theta1) + e2 * std::sin(theta1)) * r;
    const auto v2       = (e1 * std::cos(theta2) + e2 * std::sin(theta2)) * r;
    quad(p - n / 2 + v1, p - n / 2 + v2, p + n / 2 + v2, p + n / 2 + v1, col, width);
  }
}

void Render::coord(const Vec3& pos, const Vec3& axis, const double width) {
  const double l = axis[2];
  line(pos, pos + Vec3(l, 0, 0), colors::red, width);
  line(pos, pos + Vec3(0, l, 0), colors::blue, width);
  line(pos, pos + Vec3(0, 0, l), colors::green, width);
  sphere_20(pos, l / 10, colors::silver);
  sphere_20(pos + Vec3(l, 0, 0), l / 10, colors::red);
  sphere_20(pos + Vec3(0, l, 0), l / 10, colors::green);
  sphere_20(pos + Vec3(0, 0, l), l / 10, colors::blue);
}

void Render::cross(const Vec3& pos, const Vec3b& col, const double length, const double width) {
  const Vec3 halfs[] = {Vec3(length / 2, 0, 0), Vec3(0, length / 2, 0), Vec3(0, 0, length / 2)};
  line(pos - halfs[0], pos + halfs[0], col, width);
  line(pos - halfs[1], pos + halfs[1], col, width);
  line(pos - halfs[2], pos + halfs[2], col, width);
}

void Render::cube(const Vec3& pos, const Vec3& whd, const Vec3b& col) {
  const auto half_size = whd / 2;
  const Vec3 points[8] = {
    pos + Vec3(-half_size[0], -half_size[1], -half_size[2]), pos + Vec3(-half_size[0], -half_size[1], half_size[2]), pos + Vec3(-half_size[0], half_size[1], half_size[2]), pos + Vec3(-half_size[0], half_size[1], -half_size[2]),
    pos + Vec3(half_size[0], -half_size[1], -half_size[2]),  pos + Vec3(half_size[0], -half_size[1], half_size[2]),  pos + Vec3(half_size[0], half_size[1], half_size[2]),  pos + Vec3(half_size[0], half_size[1], -half_size[2]),
  };
  quad(points[0], points[1], points[2], points[3], col);
  quad(points[3], points[2], points[6], points[7], col);
  quad(points[7], points[6], points[5], points[4], col);
  quad(points[4], points[5], points[1], points[0], col);
  quad(points[0], points[3], points[7], points[4], col);
  quad(points[5], points[6], points[2], points[1], col);
}

void Render::point(const Vec3& pos, const Vec3b& col, const double size) { cube(pos, size, col); }

void Render::cube(const Vec3& pos, const Vec3& size, const Vec3b& col, float width) {
  const auto half_size = size / 2;
  // clang-format off
  const Vec3 points[8] = {
    pos + Vec3(-half_size[0], -half_size[1], -half_size[2]), 
    pos + Vec3(-half_size[0], -half_size[1], half_size[2]), 
    pos + Vec3(-half_size[0], half_size[1], half_size[2]), 
    pos + Vec3(-half_size[0], half_size[1], -half_size[2]),
    pos + Vec3(half_size[0], -half_size[1], -half_size[2]),  
    pos + Vec3(half_size[0], -half_size[1], half_size[2]),  
    pos + Vec3(half_size[0], half_size[1], half_size[2]),  
    pos + Vec3(half_size[0], half_size[1], -half_size[2]),
  };

  // clang-format on

  line(points[0], points[1], col, width);
  line(points[1], points[2], col, width);
  line(points[2], points[3], col, width);
  line(points[3], points[0], col, width);

  line(points[4], points[5], col, width);
  line(points[5], points[6], col, width);
  line(points[6], points[7], col, width);
  line(points[7], points[4], col, width);

  line(points[0], points[4], col, width);
  line(points[1], points[5], col, width);
  line(points[2], points[6], col, width);
  line(points[3], points[7], col, width);
}
void Render::cube(const Vec3& pos, float size, const Vec3b& col, float width) { cube(pos, Vec3(size, size, size), col, width); }

void Render::cube(const Vec3& pos, float size, const Vec3b& col) { cube(pos, Vec3(size, size, size), col); }

void Render::cube(const Rect3D& r, const Vec3b& col, float width) { cube(r.center<double>(), r.size<double>(), col, width); }

void Render::sphere_20(const Vec3& pos, const float size, const Vec3b& col, [[maybe_unused]] float width) {
  sphere_20(pos, size, col);
  LOGE << "IMPLEMENT SPHERE 20 WIRED!";
}

void Render::sphere_20(const Vec3& pos, const float size, const Vec3b& col) {
  // 正20面体
  const double t      = (1 + sqrt(5)) / 2;
  const Vec3 points[] = {
    pos + Vec3(0.0f, -1.0f, t) * size, pos + Vec3(0.0f, 1.0f, t) * size,   pos + Vec3(t, 0.0f, 1.0f) * size,  pos + Vec3(-t, 0.0f, 1.0f) * size,  pos + Vec3(1.0f, -t, 0.0f) * size,  pos + Vec3(1.0f, t, 0.0f) * size,
    pos + Vec3(-1.0f, t, 0.0f) * size, pos + Vec3(-1.0f, -t, 0.0f) * size, pos + Vec3(t, 0.0f, -1.0f) * size, pos + Vec3(-t, 0.0f, -1.0f) * size, pos + Vec3(0.0f, -1.0f, -t) * size, pos + Vec3(0.0f, 1.0f, -t) * size,
  };

  triangle(points[0], points[2], points[1], col);
  triangle(points[1], points[2], points[5], col);
  triangle(points[1], points[5], points[6], col);
  triangle(points[1], points[6], points[3], col);
  triangle(points[1], points[3], points[0], col);
  triangle(points[0], points[3], points[7], col);
  triangle(points[0], points[7], points[4], col);
  triangle(points[0], points[4], points[2], col);
  triangle(points[11], points[8], points[10], col);
  triangle(points[11], points[5], points[8], col);
  triangle(points[11], points[6], points[5], col);
  triangle(points[11], points[9], points[6], col);
  triangle(points[11], points[10], points[9], col);
  triangle(points[9], points[10], points[7], col);
  triangle(points[10], points[4], points[7], col);
  triangle(points[10], points[8], points[4], col);
  triangle(points[5], points[2], points[8], col);
  triangle(points[3], points[6], points[9], col);
  triangle(points[7], points[3], points[9], col);
  triangle(points[2], points[4], points[8], col);
}


void Render::triangle(const Vec2d& pos1, const Vec2d& pos2, const Vec2d& pos3, const Vec3b& col1, const Vec3b& col2, const Vec3b& col3, const Vec2d& uv1, const Vec2d& uv2, const Vec2d& uv3) {
  assert(mesh_ui != nullptr);
  assert(mesh_ui->type == db::Mesh2D::PrimitiveType::TRIANGLES);
  const auto s = mesh_ui->get_vertices_size() - 1;
  mesh_ui->add_point(pos1, col1, uv1);
  mesh_ui->add_point(pos3, col3, uv3);
  mesh_ui->add_point(pos2, col2, uv2);
  mesh_ui->indices.push_back(s);
  mesh_ui->indices.push_back(s + 1);
  mesh_ui->indices.push_back(s + 2);
}
void Render::triangle(const Vec2d& pos1, const Vec2d& pos2, const Vec2d& pos3, const Vec3b& col1, const Vec3b& col2, const Vec3b& col3) {
  assert(mesh_ui != nullptr);
  assert(mesh_ui->type == db::Mesh2D::PrimitiveType::TRIANGLES);
  const auto s = mesh_ui->get_vertices_size() - 1;
  mesh_ui->add_point(pos1, col1);
  mesh_ui->add_point(pos3, col3);
  mesh_ui->add_point(pos2, col2);
  mesh_ui->indices.push_back(s);
  mesh_ui->indices.push_back(s + 1);
  mesh_ui->indices.push_back(s + 2);
}
// TODO: クリッピングする実装
void Render::triangle(const Vec2d& pos1, const Vec2d& pos2, const Vec2d& pos3, const Vec2d uv1, const Vec2d& uv2, const Vec2d& uv3, const Vec3b& col) {
  assert(mesh_ui != nullptr);
  assert(mesh_ui->type == db::Mesh2D::PrimitiveType::TRIANGLES);
  const auto s = mesh_ui->get_vertices_size() - 1;
  mesh_ui->add_point(pos1, col, uv1);
  mesh_ui->add_point(pos3, col, uv3);
  mesh_ui->add_point(pos2, col, uv2);
  mesh_ui->indices.push_back(s);
  mesh_ui->indices.push_back(s + 1);
  mesh_ui->indices.push_back(s + 2);
}
void Render::triangle(const Vec2d& pos1, const Vec2d& pos2, const Vec2d& pos3, const Vec3b& col) {
  assert(mesh_ui != nullptr);
  assert(mesh_ui->type == db::Mesh2D::PrimitiveType::TRIANGLES);
  const auto s = mesh_ui->get_vertices_size() - 1;
  mesh_ui->add_point(pos1, col);
  mesh_ui->add_point(pos3, col);
  mesh_ui->add_point(pos2, col);
  mesh_ui->indices.push_back(s);
  mesh_ui->indices.push_back(s + 1);
  mesh_ui->indices.push_back(s + 2);
}

void Render::quad(const Vec2d& pos1, const Vec2d& pos2, const Vec2d& pos3, const Vec2d& pos4, const Vec2d& uv1, const Vec2d& uv2, const Vec2d& uv3, const Vec2d& uv4, const Vec3b& col) {
  triangle(pos1, pos2, pos3, uv1, uv2, uv3, col);
  triangle(pos1, pos3, pos4, uv1, uv3, uv4, col);
}
void Render::quad(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, const Vec3b& col) {
  triangle(p1, p2, p3, col);
  triangle(p1, p3, p4, col);
}
void Render::quad(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, const Vec3b& c1, const Vec3b& c2, const Vec3b& c3, const Vec3b& c4) {
  triangle(p1, p2, p3, c1, c2, c3);
  triangle(p1, p3, p4, c1, c3, c4);
}
void Render::rectTB(const Vec2d& top, const Vec2d& btm, const Vec3b& col) {
  const Vec2d pos[4] = {
    top,
    Vec2d(top[0], btm[1]),
    btm,
    Vec2d(btm[0], top[1]),
  };
  triangle(pos[0], pos[1], pos[2], col);
  triangle(pos[0], pos[2], pos[3], col);
}

void Render::rectTB(const Vec2d& top, const Vec2d& btm, const Vec3b& col, const float width) {
  const Vec2d pos[] = {
    top,
    Vec2d(top[0], btm[1]),
    btm,
    Vec2d(btm[0], top[1]),
  };
  line(pos[0], pos[1], col, width);
  line(pos[1], pos[2], col, width);
  line(pos[2], pos[3], col, width);
  line(pos[3], pos[0], col, width);
}
void Render::rectPS(const Vec2d& pos, const Vec2d& size, const Vec3b& col) { rectTB(pos, pos + size, col); }
void Render::rectPS(const Vec2d& p, const Vec2d& size, const Vec2d& ui1, const Vec2d& ui2, const Vec3b& col) {
  const auto top = p;
  const auto btm = p + size;

  const Vec2d pos[4] = {
    top,
    Vec2d(top[0], btm[1]),
    btm,
    Vec2d(btm[0], top[1]),
  };

  const Vec2d upos[4] = {
    ui1,
    Vec2d(ui1[0], ui2[1]),
    ui2,
    Vec2d(ui2[0], ui1[1]),
  };

  triangle(pos[0], pos[1], pos[2], col, col, col, upos[0], upos[1], upos[2]);
  triangle(pos[0], pos[2], pos[3], col, col, col, upos[0], upos[2], upos[3]);
}

void Render::rectPS(const Vec2d& p, const Vec2d& size, const Vec3b& col1, const Vec3b& col2, const Vec3b& col3, const Vec3b& col4) {
  const auto top     = p;
  const auto btm     = p + size;
  const Vec2d pos[4] = {
    top,
    Vec2d(top[0], btm[1]),
    btm,
    Vec2d(btm[0], top[1]),
  };
  triangle(pos[0], pos[1], pos[2], col1, col2, col3);
  triangle(pos[0], pos[2], pos[3], col1, col3, col4);
}

void rotated_rectPS(const Vec2d& pos, const Vec2d& size, const double theta, const Vec3b& col);
void rotated_rectPS(const Vec2d& pos, const Vec2d& size, const double theta, const Vec3b& col, const float width = 2.0f);
void Render::rectPS(const Vec2d& pos, const Vec2d& size, const Vec3b& col, float width) { rectTB(pos, pos + size, col, width); }
void Render::line(const Vec2d& pos1, const Vec2d& pos2, const Vec3b& col1, const Vec3b& col2, const float width) {
  // const auto dpd = pos2 - pos1;
  // const auto dp = Vec2{ (double)dpd[0], (double)dpd[1] } / dpd.norm();
  const auto dp = pos2 - pos1;
  const Vec2d e = {(int)(dp[1] * width / dp.norm()), -(int)(dp[0] * width / dp.norm())};
  triangle(pos1, pos2, pos2 + e, col1, col2, col2);
  triangle(pos1, pos2 + e, pos2, col1, col2, col2);
  triangle(pos1, pos2 + e, pos1 + e, col1, col2, col1);
  triangle(pos1, pos1 + e, pos2 + e, col1, col2, col1);
}
void Render::line(const Vec2d& pos1, const Vec2d& pos2, const Vec3b& col, const float width) { line(pos1, pos2, col, col, width); }
void Render::arrow(const Vec2d& from, const Vec2d& to, const Vec3b& col, const float width) { line(from, to, col, col, width); }

void Render::quad(const Vec2d& pos1, const Vec2d& pos2, const Vec2d& pos3, const Vec2d& pos4, const Vec3b& col, const float width) {
  line(pos1, pos2, col, width);
  line(pos2, pos3, col, width);
  line(pos3, pos4, col, width);
  line(pos4, pos1, col, width);
}

void Render::check(const Vec2d& pos, const int size, const Vec3b& col, float line_width) {
  line(pos + Vec2({(float)size * 0.1, (float)size * 0.5}), pos + Vec2{(float)size * 0.5, (float)size * 0.9}, col, line_width);
  line(pos + Vec2({(float)size * 0.5, (float)size * 0.9}), pos + Vec2{(float)size * 0.9, (float)size * 0.1}, col, line_width);
}

void Render::arrow_down(const Vec2d& pos, const int size, const Vec3b& col) {
  const int h = (float)size * 0.85f;
  triangle(pos, pos + Vec2d{size / 2, h}, pos + Vec2d{size, 0}, col);
}

void Render::arrow_up(const Vec2d& pos, const int size, const Vec3b& col) {
  const int h = (float)size * 0.85f;
  triangle(pos + Vec2d{0, h}, pos + Vec2d{size, h}, pos + Vec2d{size / 2, 0}, col);
}

void Render::arrow_left(const Vec2d& pos, const int size, const Vec3b& col) {
  const int h = (float)size * 0.85f;
  triangle(pos + Vec2d{0, size / 2}, pos + Vec2d{h, size}, pos + Vec2d{h, 0}, col);
}

void Render::arrow_right(const Vec2d& pos, const int size, const Vec3b& col) {
  const int h = (float)size * 0.85f;
  triangle(pos + Vec2d{0, size}, pos + Vec2d{h, size}, pos + Vec2d{h, 0}, col);
}
void Render::arrow_right2(const Vec2d& pos, const int size, const Vec3b& col) {
  const int h = (float)size * 0.85f;
  triangle(pos, pos + Vec2d{0, size}, pos + Vec2d{h, size / 2}, col);
}

void Render::cross_button(const Vec2d& pos, const int size, const Vec3b& bg_col, const Vec3b& line_col, const Vec3b& cross_col) {
  rectTB(pos, pos + size, bg_col);
  rectTB(pos, pos + size, line_col, 1);
  constexpr int _padding_cross    = 2;
  constexpr int _cross_line_width = 2;
  line(pos + _padding_cross, pos + size - _padding_cross, {255, 255, 255}, _cross_line_width);
  line(pos + Vec2d{size - _padding_cross, _padding_cross}, pos + Vec2d{_padding_cross, size - _padding_cross}, cross_col, _cross_line_width);
}

void Render::cross(const Vec2d& center, const int size, const Vec3b& col) {
  constexpr int _cross_line_width = 2;
  const Vec2d pos                 = center - int(size / 2);
  line(pos, pos + size, col, _cross_line_width);
  line(pos + Vec2d{size, 0}, pos + Vec2d{0, size}, col, _cross_line_width);
}

void Render::plus(const Vec2d& center, const int size, const Vec3b& col) {
  constexpr int _cross_line_width = 2;
  const int hs                    = size / 2; // half size
  line(center - Vec2d(hs, 0), center + Vec2d(hs, 0), col, _cross_line_width);
  line(center - Vec2d(0, hs), center + Vec2d(0, hs), col, _cross_line_width);
}

void Render::diamond(const Vec2d& center, const int size, const Vec3b& col) {
  const int hs = size / 2;
  quad(center + Vec2d{hs, 0}, center + Vec2d{0, hs}, center - Vec2d{hs, 0}, center - Vec2d{0, hs}, col);
}

void Render::circle(const Vec2d& pos, const int size, const Vec3b col) {
  if(size < 15) {
    constexpr double s45 = 0.707090402;
    const Vec2 p[8]      = {{0, 1}, {s45, s45}, {1, 0}, {s45, -s45}, {0, -1}, {-s45, -s45}, {-1, 0}, {-s45, s45}};
    for(int i = 0; i < 6; i++) triangle(pos + p[0] * size, pos + p[1 + i] * size, pos + p[i + 2] * size, col);
  } else {
    constexpr double dth = 6.28 / 20.0f;
    for(int i = 0; i < 20; i++) {
      const auto p1 = pos + Vec2d(std::cos(dth * i) * size, std::sin(dth * i) * size);
      const auto p2 = pos + Vec2d(std::cos(dth * (i + 1)) * size, std::sin(dth * (i + 1)) * size);
      triangle(pos, p1, p2, col);
    }
  }
}

void Render::circle(const Vec2d& pos, const int size, const Vec3b col, const int width) {
  constexpr double dth = 6.28 / 20.0f;
  for(int i = 0; i < 20; i++) {
    const auto p1 = pos + Vec2d(std::cos(dth * i) * size, std::sin(dth * i) * size);
    const auto p2 = pos + Vec2d(std::cos(dth * (i + 1)) * size, std::sin(dth * (i + 1)) * size);
    line(p1, p2, col, width);
  }
}

void Render::baloon(const std::string& str, const core::Vec3& pos, const float size, const Vec3b& col, const Vec3b& line) {
  int width, height;
  glfwGetWindowSize(wnd, &width, &height);
  const Vec4 pos4(pos[0], pos[1], pos[2], 1.0);
  const auto p  = camera.project(pos) * 0.5f;
  const auto to = Vec2d(width / 2.0f + p[0] * width, height / 2.0f - p[1] * height);
  baloon(str, to, size, col, line);
}


void Render::baloon(const std::string& str, const Vec2d& to, const float size, const Vec3b& col, const Vec3b& width) {
  const bool is_arrow_up = to[1] > 35; // 通常True
  const auto tsize       = get_text_size(str, size);
  int x, y;
  constexpr int line_width = 2;
  constexpr int leader_len = 25;
  if(is_arrow_up) {
    line(to, to + Vec2d(3, -5), width, line_width);
    line(to, to + Vec2d(leader_len, -leader_len), width, line_width);
    line(to + Vec2d(leader_len, -leader_len), to + Vec2d(leader_len + tsize[0] + 10, -leader_len), width, line_width);
    x = to[0] + leader_len + 5;
    y = to[1] - leader_len - tsize[1] - 10;
  } else {
    line(to, to + Vec2d(3, 5), width, line_width);
    line(to, to + Vec2d(leader_len, leader_len), width, line_width);
    line(to + Vec2d(leader_len, leader_len), to + Vec2d(leader_len + tsize[0] + 10, leader_len), width, line_width);
    x = to[0] + leader_len + 5;
    y = to[1] + leader_len - tsize[1] - 10;
  }
  put_text(str, {x, y}, 1, col);
}

Vec2d Render::get_text_size(const std::string& str, const float size) const {
  const auto u32str = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);
  int x             = 0;
  const auto r      = instance::get_text_renderer();
  const int spacing = instance::get_style()->TextSpacing;

  Vec2d whole_size{0, 0};
  if(u32str.size() == 0) return {0, 0};

  for(size_t i = 0; i < u32str.size(); i++) {
    const auto glyph = r->FindGlyph(u32str[i]);
    const int y      = (glyph->V1 - glyph->V0) * size;
    x += spacing + (glyph->U1 - glyph->U0) * size;
    whole_size[0] = std::max<int>(whole_size[0], x);
    whole_size[1] = std::max<int>(whole_size[1], y);
  }
  return whole_size;
}

Vec2d Render::put_text(const std::string& str, const Vec2d& pos, const float size, const Vec3b& col, const int xlim) {
  const std::u32string u32str = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);
  int x                       = pos[0];
  int y                       = pos[1];
  const auto text_renderer    = instance::get_text_renderer();
  const int spacing           = instance::get_style()->TextSpacing;
  Vec2d whole_size{0, 0};
  // const int w = text_renderer->TexWidth;
  // const int h = text_renderer->TexHeight;
  if(u32str.size() == 0) {
    return {0, 0};
  }

  // y += text_renderer->FindGlyph(u32str[0])->dHeight * size;
  y += instance::get_style()->FontSize + 1;

  for(size_t i = 0; i < u32str.size(); i++) {
    const auto glyph = text_renderer->FindGlyph(u32str[i]);
    const int x2     = x + (glyph->U1 - glyph->U0) * size;
    if(((uiWchar)u32str[i] == '\n' || x2 - pos[0] > xlim) && u32str.size() > i + 1) {
      x = pos[0];
      y = pos[1] + whole_size[1] + text_renderer->FindGlyph(u32str[i + 1])->dHeight * size + spacing;
      continue;
    }
    const int y_tmp = y + (-glyph->dHeight + (int)(glyph->V1 - glyph->V0)) * size;
    const int y1    = std::max<int>(0, y - glyph->dHeight * size);
    const int y2    = std::max<int>(0, y_tmp);
    quad({x, y1}, {x, y2}, {x2, y2}, {x2, y1}, {glyph->U0, glyph->V0}, {glyph->U0, glyph->V1}, {glyph->U1, glyph->V1}, {glyph->U1, glyph->V0}, col);
    x += spacing + (glyph->U1 - glyph->U0) * size;

    whole_size = {std::max(whole_size[0], x - pos[0]), std::max(whole_size[1], y_tmp - pos[1])};
  }
  return whole_size;
}

} // namespace mu::render
