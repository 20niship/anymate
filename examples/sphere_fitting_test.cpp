#include <chrono>
#include <fstream>
#include <random>

#include <movutl/core/vector.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/mu.hpp>
#include <movutl/tools/magi/fitting.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu;
using namespace mu::Magi;
using namespace mu::core;
using namespace std::chrono;

const double range           = 30;
constexpr int npoint         = 100;
constexpr double error_ratio = 0.03;

#define MAGI_MATH_PI 3.141592653

auto gen_cloud() {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_real_distribution<> pos_rd(-range, range);
  std::uniform_real_distribution<> radius_rd(-range, range);
  std::uniform_real_distribution<> theta_rd(0, 2.0 * MAGI_MATH_PI);
  std::uniform_real_distribution<> error(-range * error_ratio, range * error_ratio);

  const Vec3 pos = {
    pos_rd(engine),
    pos_rd(engine),
    pos_rd(engine),
  };
  const auto r = radius_rd(engine);

  Vec<Vec3> cloud;
  for(size_t i = 0; i < npoint; ++i) {
    const auto th  = theta_rd(engine);
    const auto th2 = theta_rd(engine);
    const Vec3 k   = {
        (r + error(engine)) * sin(th) * cos(th2) + error(engine),
        (r + error(engine)) * sin(th) * sin(th2) + error(engine),
        (r + error(engine)) * cos(th) + error(engine),
    };
    const auto ptr = pos + k;
    cloud.push_back(ptr);
  }
  return cloud;
}

int main() {
  instance::init();
  auto wnd    = mu::ui::create_window("hello", 640, 480);
  auto cam    = wnd->getCameraPtr();
  auto r      = wnd->get_renderer();

  cam->pos   = {10, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.1;

  // clang-format off
  auto frame = mu::ui::frame("main", {
     mu::ui::collapse("camera",
      {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
      }),
   });
  // clang-format on

  wnd->addWidget(frame);
  cam->pos         = {10, 10, 10};
  r->gl_point_size = 3;

  db::MeshCol* m    = new db::MeshCol();
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);
  r->add_mesh(m);

  mu::Magi::SphereFitting fit;
  mu::Magi::FittingResult res;

  auto start = system_clock::now();
  while(!wnd->should_close()) {
    const auto now = system_clock::now();
    double msec    = (double)duration_cast<milliseconds>(now - start).count();
    if(msec > 1000) {
      std::cout << "update!!" << std::endl;
      start = now;
      m->clear();
      auto cloud = gen_cloud();
      fit.reset();
      fit.setCloud(cloud.begin(), cloud.size());
      fit.estimate();
      res = fit.result();
      res.display();

      for(size_t i = 0; i < cloud.size(); i++) {
        const auto pos = cloud[i];
        const auto col = colors::red;
        m->add_point(pos, col);
      }
    }

    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 6});
    r->sphere_20(res.xyz, res.size[0], colors::yellow);

    mu::ui::update(true);
  }

  wnd->terminate();
  return 0;
}
