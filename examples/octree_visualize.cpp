#include <random>

#include <movutl/core/octree.hpp>
#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace std;
using namespace mu::core;
using namespace mu::render;
using namespace mu::db;
using namespace mu::ui;

using OctreeT = mu::core::Octree2<double, 10>;
using fRange  = Range<float>;

void _display_octree(Render* r, const OctreeT::Branch* b, const Rect3D& re, const int size) {
  if(b == nullptr) return;

  const auto c      = re.center<double>();
  const fRange xr[] = {fRange(re.x.min, c[0]), fRange(c[0], re.x.max)};
  const fRange yr[] = {fRange(re.y.min, c[1]), fRange(c[1], re.y.max)};
  const fRange zr[] = {fRange(re.z.min, c[2]), fRange(c[2], re.z.max)};
  r->cube(c, re.size<double>(), mu::colors::blue, 0.03);

  for(uint8_t i = 0; i < 8; i++) {
    OctreeT::Node* n = b->children[i & 7];
    if(n == nullptr) continue;
    if(size > 1) {
      const auto child = reinterpret_cast<const OctreeT::Branch*>(b->children[i]);
      const uint8_t xi = i & 1;
      const uint8_t yi = ((i & 2) > 0) ? 1 : 0;
      const uint8_t zi = i / 4;
      const Rect3D child_r(xr[xi], yr[yi], zr[zi]);
      /* MU_ASSERT(!re.contains(child_r)); */
      _display_octree(r, child, child_r, size / 2);
    } else {
      /* const auto l = reinterpret_cast<const OctreeT::Leave*>(n); */
      /* for(int k = 0; k < l->indices.size(); k++) */
    }
  }
}

void display_octree(Render* r, const OctreeT* octree) {
  const auto re   = octree->get_bbox();
  const auto root = octree->root();
  MU_ASSERT(root != nullptr);
  _display_octree(r, root, re, octree->resolution() / 2);
}

std::random_device rd;
std::mt19937 mt(rd());

mu::core::Vec<mu::core::Vec3> dataset;

class DisplayRadiusSearch : public OctreeT::SearchBase {
public:
  Render* render;
  const OctreeT* o;
  Vec3d radius;
  DisplayRadiusSearch() = delete;
  DisplayRadiusSearch(const Vec3d& tar, Vec3d radius_) {
    this->target_pos = tar;
    radius           = radius_;
  }

  void search(const OctreeT::Branch* b, Vec3d t, const unsigned size) {
    MU_ASSERT(b != nullptr);
    MU_ASSERT(isPow2(size));
    if(size == 1) return;
    const uint8_t start_i = this->get_start_idx(size);
    for(uint8_t i = start_i; i < (start_i + 8); ++i) {
      OctreeT::Node* n = b->children[i & 7];
      if(n == nullptr) continue;
      const unsigned child_x = (i & 1) ? (t[0] + size) : t[0];
      const unsigned child_y = (i & 2) ? (t[1] + size) : t[1];
      const unsigned child_z = (i & 4) ? (t[2] + size) : t[2];
      const Vec3d branch_bbox_min_pos(child_x, child_y, child_z);

      if(this->check_branch_radius(branch_bbox_min_pos, size, radius)) {
        {
          const Vec3 raw = o->bin_to_raw(branch_bbox_min_pos);
          Vec3 raw_size;
          raw_size[0] = (float)size / o->resolution() * o->r.x.length();
          raw_size[1] = (float)size / o->resolution() * o->r.y.length();
          raw_size[2] = (float)size / o->resolution() * o->r.z.length();
          render->cube(raw + raw_size / 2.0, raw_size, mu::colors::orange, 0.1);
        }
        search(reinterpret_cast<OctreeT::Branch*>(n), branch_bbox_min_pos, size / 2);
      }
    }
  }
};

void display_find_nearest(Render* r, const OctreeT* octree) {
  const auto re   = octree->get_bbox();
#if DEBUG
  const auto root = octree->root();
  MU_ASSERT(root != nullptr);
#endif
  static size_t N = 30;
  static size_t M = 0;
  static Vec3 query;
  static Vec3d bin;

  if(N == 30) {
    std::uniform_real_distribution<float> score_x(re.x.min, re.x.max);
    std::uniform_real_distribution<float> score_y(re.y.min, re.y.max);
    std::uniform_real_distribution<float> score_z(re.z.min, re.z.max);
    query = Vec3(score_x(mt), score_y(mt), score_z(mt));
    query = dataset[M % (dataset.size() - 1)];
    bin   = octree->apply_bbox(query);
    N     = 0;
    M++;
  }
  N++;

  const float radius = 0.1;
  const auto radius_bin = octree->raw_to_bin_scale(radius);
  r->cross(query, mu::colors::red, 10, 0.05);
  DisplayRadiusSearch searcher(bin, radius_bin);
  searcher.render = r;
  searcher.o      = octree;
  searcher.search(octree->root(), {0, 0, 0}, octree->resolution() / 2);
}

void create_dataset(const int nData) {
  dataset.resize(nData);
  std::uniform_real_distribution<float> score_x(-20.0, 20.0);
  std::uniform_real_distribution<float> score_y(-20.0, 20.0);
  std::uniform_real_distribution<float> score_z(-20.0, 20.0);
  for(int i = 0; i < nData; i++) dataset[i] = {score_x(mt), score_y(mt), score_z(mt)};
}

int main() {
  cout << "start" << endl;

  create_dataset(200);
  OctreeT octree;
  octree.set_dataset(&dataset);
  octree.report();

  mu::instance::init();
  auto wnd = mu::ui::create_window("hello", 640, 480);
  auto cam = wnd->getCameraPtr();
  // clang-format off
  auto frame = mu::ui::frame("main", {
     mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
     mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
     mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
   });
  // clang-format on

  frame->setSize(200, 480);
  frame->setPos(0, 0);
  wnd->addWidget(frame);
  cam->pos   = {10, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.3;

  const auto bbox = octree.get_bbox();
  while(!wnd->should_close()) {
    wnd->draw_ui();
    auto r = wnd->get_renderer();
    r->coord({0, 0, 0});
    r->cube(bbox.center<double>(), bbox.size<double>(), mu::colors::green, 0.05);
    display_octree(r, &octree);
    display_find_nearest(r, &octree);
    for(size_t i = 0; i < dataset.size(); i++) r->cube(dataset[i], 0.1, mu::colors::red);
    /* r->summary(); */
    mu::ui::update(true);
  }

  wnd->terminate();
  glfwTerminate();
  return 0;
}
