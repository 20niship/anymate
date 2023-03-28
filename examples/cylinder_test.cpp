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
  std::uniform_real_distribution<> rd_r(1, 10);
  std::uniform_real_distribution<> rd_h(20, 40);
  std::uniform_real_distribution<> rd_h2(0.0,1.0);
  std::uniform_real_distribution<> error(-range * error_ratio, range * error_ratio);

  /* const auto normal = Vec3(xd(engine), xd(engine), xd(engine)).normalize(); */
  const auto normal = Vec3(0,0,1).normalize();
  const auto center = Vec3(xd(engine), xd(engine), xd(engine)).normalize();
  const double r = rd_r(engine);
  const double h = rd_h(engine);

  const auto e12 = get_vert_vec(normal);

  Vec<Vec3> cloud;
  for(size_t i = 0; i < npoint; ++i) {
    const auto theta  = xd(engine);
    const auto h2 = rd_h2(engine) * h;
    const auto p = e12[0] * std::sin(theta) + e12[1] * std::cos(theta);
    const auto ptr = p*r + center + normal*h2;
    cloud.push_back(ptr);
  }

  DISP(normal);
  DISP(center);
  return cloud;
}

int main() {
  instance::init();
  auto wnd    = mu::ui::create_window("hello", 640, 480);
  auto cam    = wnd->getCameraPtr();
  auto r      = wnd->get_renderer();

  cam->pos   = {10, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.3;
  r->gl_point_size = 3;

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

  db::MeshCol* m    = new db::MeshCol();
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);
  r->add_mesh(m);

  mu::Magi::CylinderFitting fit;
  mu::Magi::FittingResult res;

  auto start = std::chrono::system_clock::now();
  while(!wnd->should_close()) {
    const auto now = std::chrono::system_clock::now();
    double msec    = (double)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
    if(msec > 1000) {
      std::cout << "update!!" << std::endl;
      start = now;
      m->clear();
      auto cloud = gen_cloud();
      for(int i = 0; i < npoint; i++)
        m->add_point(cloud[i], colors::red);

      fit.setCloud(cloud.begin(), cloud.size());
      fit.estimate();
      res = fit.result();
      res.display();
    }

    wnd->draw_ui();
    r->cylinder(res.xyz, res.axis, res.size.norm(), colors::yellow, 0.3);
    r->coord({0, 0, 0}, {0, 0, 6});
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();

  return 0;
}
