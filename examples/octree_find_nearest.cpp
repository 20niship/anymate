#include <chrono>
#include <math.h>
#include <movutl/core/octree.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <random>

using namespace mu;
using namespace std;
using namespace mu::core;

int main() {
  instance::init();
  mu::ui::create_window("hello", 640, 480);

  const std::string fname = std::string(MOVUTL_DATA_DIR) + "/residential.pcd";
  const auto cloud        = io::impl_load_pcd(fname.c_str());
  const auto m            = (db::MeshCol*)cloud->meshs[0];

  spdlog::set_level(spdlog::level::warn);

  mu::core::Octree2<double, 10> octree;
  std::cout << "inserting......" << std::endl;
  octree.set_dataset(m);
  octree.report();

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> score(0, m->vertex.size());
  DISP(m->get_bbox());

  for(int i = 0; i < 10; i++) {
    const size_t idx = score(mt);
    const Vec3 query(m->vertex[idx].pos[0], m->vertex[idx].pos[1], m->vertex[idx].pos[2]);
    DISP(query);

    const auto start = chrono::system_clock::now();
    const auto res   = octree.findNearest(query, SearchMethod::Sphere, 0.03);
    const auto end   = chrono::system_clock::now();
    double msec      = (double)chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1000.0;

    if(res.size() > 0) {
      for(const auto& i : res) {
        const auto pp   = m->vertex[i].pos;
        const auto p    = Vec3(pp[0], pp[1], pp[2]);
        const auto dist = (p - query).norm();
        std::cout << dist << " at " << i << " =  " << p << std::endl;
      }
    } else {
      spdlog::warn("no  point found!!");
    }
    std::cout << "time:" << msec << std::endl;
  }
  return 0;
}
