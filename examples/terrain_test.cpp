#include <chrono>
#include <fstream>
#include <movutl/tools/magi/fitting.hpp>
#include <random>

#include <movutl/core/vector.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/mu.hpp>
#include <movutl/tools/magi/fitting.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu;
using namespace std::chrono;
using namespace mu::Magi;
using namespace mu::core;

const double range           = 70;
constexpr int npoint         = 5000;
constexpr double error_ratio = 0.01;

auto gen_cloud() {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_real_distribution<> xd(-range, range);
  std::uniform_real_distribution<> yd(-range, range);
  std::uniform_real_distribution<> error(-range * error_ratio, range * error_ratio);

  std::uniform_real_distribution<> ss(0.01, 0.2);
  const auto xp = ss(engine);
  const auto yp = ss(engine);

  Vec<Vec3> cloud;
  for(size_t i = 0; i < npoint; ++i) {
    const auto x   = xd(engine);
    const auto y   = yd(engine);
    const auto z   = 10 * std::sin(x * xp) + 12 * std::cos(y * yp) + error(engine);
    const auto ptr = Vec3(x, y, z);
    cloud.push_back(ptr);
  }
  return cloud;
}

int main() {
  instance::init();
  auto wnd    = mu::ui::create_window("hello", 640, 480);
  auto cam    = wnd->getCameraPtr();
  auto r      = wnd->get_renderer();
  Vec2d nsplit = Vec2d(10,10);

  cam->pos   = {10, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.1;
  r->gl_point_size = 3;

  // clang-format off
  auto frame = mu::ui::frame("main", {
     mu::ui::collapse("camera",
      {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
        mu::ui::uivec2("sp", &nsplit, {8, 30}, 1),
      }),
   });
  // clang-format on

  wnd->addWidget(frame);

  db::MeshCol* m    = new db::MeshCol();
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);
  r->add_mesh(m);

  mu::Magi::TerrainFitting fit;
  m->clear();
  int i = 0;

  auto start = system_clock::now();
  while(!wnd->should_close()) {
    const auto now = system_clock::now();
    double msec    = (double)duration_cast<milliseconds>(now - start).count();
    if(msec > 1000) {
      std::cout << "update!!" << std::endl;
      start = now;
      m->clear();
      auto cloud = gen_cloud();
      fit.nSplit(nsplit[0], nsplit[1]);
      fit.setCloud(cloud.begin(), cloud.size());
      fit.estimate();
      for(const auto& c : cloud) m->add_point(c, colors::blue);
    }

    if(fit.get_terrain().size() > 0){
      const auto te = fit.get_terrain();
      for(int y = 0; y < fit.sp_y; y++) {
        for(int x = 0; x < fit.sp_x; x++) {
          r->sphere_20(te[x][y], 0.2, colors::yellow);
        }
      }
      r->cube(fit.get_bbox(), colors::red, 0.3);

      for(int y = 0; y < fit.sp_y-1; y++) {
        for(int x = 0; x < fit.sp_x-1; x++) {
          r->triangle(
              te[x][y],
              te[x][y+1],
              te[x+1][y],
              colors::yellow, 0.3);
          r->triangle(
              te[x+1][y],
              te[x][y+1],
              te[x+1][y+1],
              colors::yellow, 0.3);
        }
      }
    }

    r->camera.pos = {std::sin(i * 0.01) * 10, std::cos(i * 0.01) * 10, -16};
    i++;

    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 15});
    mu::ui::update(true);
  }

  wnd->terminate();
  return 0;
}
