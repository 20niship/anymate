#include <chrono>
#include <fstream>
#include <opencv2/core/persistence.hpp>
#include <random>

#include <movutl/core/vector.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/mu.hpp>
#include <movutl/tools/magi/convex_hull.hpp>
#include <movutl/tools/magi/fitting.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

#include <movutl/tools/magi/surfacing.hpp>

using namespace mu;
using namespace mu::Magi;
using namespace mu::core;

const double range           = 30;
constexpr int npoint         = 500;
constexpr double error_ratio = 0.01;

auto gen_cloud() {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_real_distribution<> xd(-range, range);
  std::uniform_real_distribution<> yd(-range, range);
  std::uniform_real_distribution<> error(-range * error_ratio, range * error_ratio);

  const auto normal = Vec3(xd(engine), xd(engine), xd(engine)).normalize();
  const Vec4 plane  = Vec4(normal[0], normal[1], normal[2], xd(engine) / 10);

  Vec<Vec3> cloud;
  for(size_t i = 0; i < npoint; ++i) {
    const auto x   = xd(engine);
    const auto y   = yd(engine);
    const auto z   = -(plane[0] * x + plane[1] * y + plane[3] + error(engine)) / plane[2];
    const auto ptr = Vec3(x, y, z);
    cloud.push_back(ptr);
  }
  return cloud;
}

bool in(std::vector<size_t> vec, size_t idx) {
  for(size_t i = 0; i < vec.size(); i++)
    if(vec[i] == idx) return true;
  return false;
}

int main() {
  instance::init();
  auto wnd    = mu::ui::create_window("hello", 640, 480);
  auto cam    = wnd->getCameraPtr();
  auto r      = wnd->get_renderer();

  cam->pos   = {10, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.1;
  auto frame = mu::ui::frame("main", {
                                       mu::ui::collapse("camera",
                                                        {
                                                          mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
                                                          mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
                                                          mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
                                                          mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
                                                        }),
                                     });

  wnd->addWidget(frame);
  cam->pos         = {10, 10, 10};
  r->gl_point_size = 3;

  db::MeshCol* m    = new db::MeshCol();
  db::MeshCol* bbox = new db::MeshCol();
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);
  r->add_mesh(m);
  r->add_mesh(bbox);
  r->summary();

  auto start = std::chrono::system_clock::now();
  while(!wnd->should_close()) {
    const auto now = std::chrono::system_clock::now();
    double msec    = (double)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    if(msec > 1000) {
      std::cout << "update!!" << std::endl;
      start = now;
      m->clear();
      auto cloud = gen_cloud();
      Surface surf(cloud.begin(), cloud.size());
      surf.run();
      for(int i = 0; i < npoint; i++) {
        const auto pos = cloud[i];
        if(in(surf.m_convex_hull_idx, i)) continue;
        const auto col = colors::red;
        m->add_point(pos, col);
      }
      bbox->clear();

      {
        const auto col = colors::yellow;
        for(size_t i = 0; i < surf.m_mesh.size() - 1; i++) {
          bbox->add_point(surf.m_mesh[i], col);
          bbox->add_point(surf.m_mesh[i + 1], col);
        }
        bbox->add_point(surf.m_mesh.back(), col);
        bbox->add_point(surf.m_mesh[0], col);
      }
      bbox->primitive_type(db::_MeshBase::PrimitiveType::LINES);
    }

    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 6});
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
