#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <movutl/tools/mesh/normal_estimation.hpp>
#include <movutl/tools/colormap.hpp>

using namespace mu;
using namespace mu::core;

int main() {
  instance::init();
  auto wnd      = mu::ui::create_window("obj_view", 640, 480);
  auto cam      = wnd->getCameraPtr();
  auto r = wnd->get_renderer();

  cam->pos   = {20, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 1.0;

#if 1
  auto m = instance::create_mesh_3d();
  instance::add_mesh_sphere(m, {0, 0, 0}, 5.0, 50, 50);
#else
  const char* fname = "/home/owner/Downloads/iphone/ply/inside.ply";
  db::Body* obj = io::impl_load_ply(fname);
  auto m = reinterpret_cast<db::MeshCol *>(obj->meshs[0]);
#endif 

  mu::tools::NormalEstimation n;
  n.setRadius(2);
  n.setMesh(m);
  n.compute();
  n.flip();
  
  auto colored_mesh = mu::instance::create_mesh_col();
  mu::tools::colormap_by_normal(m, colored_mesh, n.normal);
  DISP(colored_mesh->get_vertices_size());

  /* r->add_mesh(m); */
  r->add_mesh(colored_mesh);

  while(!wnd->should_close()) {
    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 0.5}, 0.01);
    r->point_size(2.0);
    const auto bbox = m->get_bbox();
    r->cube(bbox.center<double>(), bbox.size<double>(), mu::colors::red, 0.03);

    for(size_t i=0; i<n.normal.size(); i++){
      constexpr float line_width = 5;
      Vec3 p;
      p[0] = m->vertex[i].pos[0];
      p[1] = m->vertex[i].pos[1];
      p[2] = m->vertex[i].pos[2];
      const Vec3 p2 = p + n.normal[i] * line_width;
      r->line(p, p2, colors::red, 0.03);
    }
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
