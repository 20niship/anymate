#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <movutl/experimental/picking_object.hpp>

using namespace mu;
using namespace mu::db;
using namespace mu::experimental;

int main(int argc, char *argv[]) {
  spdlog::set_level(spdlog::level::warn);
  instance::init();
  auto wnd  = mu::ui::create_window("object picking", 1080, 720);
  auto rwnd = wnd->get_renderer();
  auto cam  = wnd->getCameraPtr();
  PointCloudPicker p;
  std::vector<size_t> indices, indices_tmp;

  struct Config {
    Vec3b bg;
    float radius = 5;

    enum SelectType { Circle, Rect, Point };
    SelectType type = Point;
    void update(render::Render* rwnd) { rwnd->background_color(bg); }
  } config;

  auto txt = (ui::uiLabel*)ui::label("0");

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
        mu::ui::slider("point", &rwnd->gl_point_size, {1, 10}, 1),
      }),
      mu::ui::collapse("hoge", {
        mu::ui::Col("bg", &config.bg),
        mu::ui::slider("r", &config.radius, {0,100}),
        mu::ui::button("circle", [&]{
            config.type = Config::Circle;
            }),
        mu::ui::button("rect", [&]{
            config.type = Config::Rect;
            }),
        mu::ui::button("point", [&]{
            config.type = Config::Point;
            }),
        mu::ui::button("clear", [&]{
            indices.clear();
            }),
        txt
      }),
  });
  // clang-format on
  frame->setSize(240, 600);
  wnd->addWidget(frame);

  // instance::get_engine()->init_after_surface_creation();
  const std::string fname = (argc > 1) ? argv[1] : ( std::string(MOVUTL_DATA_DIR) + "/residential.pcd");
  const auto cloud        = io::impl_load_pcd(fname.c_str());
  const auto m            = (db::MeshCol*)cloud->meshs[0];
  auto mselect            = instance::create_mesh_col();

  p.setup();
  p.set_mesh(m);

  rwnd->add_mesh(m);
  rwnd->add_mesh(mselect);
  rwnd->point_size(2);
  rwnd->camera.pos   = {20, -16, -16};
  rwnd->camera.dir   = {0, 0, 0};
  rwnd->camera.scale = 0.7;

  do {
    rwnd->coord({0, 0, 0}, {0, 0, 0.5}, 0.01);
    p.update(rwnd->camera);

    const auto io = instance::get_io();
    {
      const auto pos = io->mouse_pos;
      switch(int(config.type)) {
        case Config::Circle:
          if(io->is_mouse_left_pressing()) {
            const auto index = p.get_point_circle(pos, config.radius);
            for(const auto i : index) indices.push_back(i);
          }
          break;
        case Config::Rect:
          if(io->is_mouse_left_pressing()) {
            indices_tmp.clear();
            const auto startpos = io->button[(int)ui::uiMouseButton::Left].start_pos;
            const auto index    = p.get_point_rect(startpos, pos);
            rwnd->rectPS(startpos, pos - startpos, colors::blue, 1);
            for(const auto i : index) indices_tmp.push_back(i);
          } else {
            for(const auto i : indices_tmp) indices.push_back(i);
            indices_tmp.clear();
          }

        case Config::Point:
          if(io->is_mouse_left_pressing()) {
            const auto index = p.get_point(pos);
            if(index >= 0) indices.push_back(index);
          }
          break;
      }
    }

    {
      const auto pos = io->mouse_pos;

      std::stringstream ss;
      ss << "select " << indices.size() << "\n\n";
      ss << "tmp    " << indices_tmp.size() << "\n\n";
      switch(int(config.type)) {
        case Config::Circle:
          ss << "[circle] " << config.radius << "\n\n";
          rwnd->circle(pos, config.radius, colors::red, 2);
          break;
        case Config::Rect: ss << "[rect] \n\n"; break;
        case Config::Point:
          rwnd->circle(pos, 2, colors::red, 2);
          ss << "[point] \n\n";
          break;
      }
      txt->setText(ss.str());
    }

    if(0) {
      mselect->clear();
      for(const auto i : indices) {
        const auto p  = m->vertex[i].pos;
        const auto pp = Vec3(p[0], p[1], p[2]);
        mselect->add_point(pp, colors::red);
      }

      for(const auto i : indices_tmp) {
        const auto p  = m->vertex[i].pos;
        const auto pp = Vec3(p[0], p[1], p[2]);
        mselect->add_point(pp, colors::blue);
      }
    } else {
      for(const auto i : indices) {
        m->vertex[i].col[0] = 255;
        m->vertex[i].col[1] = 0;
        m->vertex[i].col[2] = 0;
      }

      for(const auto i : indices_tmp) {
        m->vertex[i].col[2] = 255;
        m->vertex[i].col[1] = 0;
        m->vertex[i].col[0] = 0;
      }
    }

    {
      rwnd->coord({0, 0, 0}, {0, 0, 0.5}, 0.01);
      wnd->drawDevelopperHelps();
      wnd->draw_ui();
      mu::ui::update(true);
    }
    cv::waitKey(1);
  } while(!wnd->should_close());
  glfwTerminate();
  return 0;
}
