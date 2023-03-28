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

bool in(std::vector<size_t> vec, int idx) {
  for(size_t i = 0; i < vec.size(); i++)
    if(vec[i] == (size_t)idx) return true;
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
  db::MeshCol* bbox = new db::MeshCol();
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);
  r->add_mesh(m);
  r->add_mesh(bbox);

  mu::Magi::PlaneFitting fit;
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
      fit.setCloud(cloud.begin(), cloud.size());
      fit.estimate();
      res = fit.result();
      res.display();
      for(const auto &c:cloud)
        m->add_point(c, colors::blue);
    }

    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 6});
    r->plane(res.xyz, res.size[0], res.axis, colors::yellow, 0.3);
    mu::ui::update(true);
  }

  wnd->terminate();
  return 0;
}
