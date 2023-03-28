#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <opencv2/opencv.hpp>

using namespace mu;
using namespace mu::db;

const int width=1080;
const int height= 720;

mu::render::Render r;
mu::render::Framebuffer fb;

void setup_renderer(){
  fb.set_size(width, height);
  fb.create();
}

void render_frame(){
  r.push();
  r.sort();
  r.draw();
  r.clear_drawlist();
  if(r.mesh_col != nullptr)r.mesh_col->clear();
  if(r.mesh_ui != nullptr) r.mesh_ui->clear();
  if(r.mesh_3d != nullptr) r.mesh_3d->clear();
}


int main() {
  instance::init();
  glfwWindowHint( GLFW_VISIBLE, 0 );
  mu::ui::create_window("teapot", 640, 480);
  setup_renderer();

  //instance::get_engine()->init_after_surface_creation();
  auto path     = std::string(MOVUTL_DATA_DIR) + "/teapot.stl";
  db::Body* teapot= io::impl_load_obj(path.c_str());

  r.init();
  r.m_framebuffer_id= fb.get_fbo_id();
  auto s = instance::get_ui_shader();
  r.set_shader(s);
  r.add_mesh(reinterpret_cast<db::Mesh *>(teapot->meshs[0]));
  r.camera.pos   = {20, -16, -16};
  r.camera.dir   = {0, 0, 0};
  r.camera.scale = 0.7;

  int i=0;
  do {
    r.camera.pos   = {
      std::sin(i*0.1)*10,
      std::cos(i*0.1)*10,
      -16
    };
    i+= 1;
    DISP(r.m_bg);
    r.coord({0, 0, 0}, {0, 0, 0.5}, 0.01);

    render_frame();
  {
    const int ch = 3;
    GLubyte* pixels = new GLubyte[width * height * ch];
    glBindTexture(GL_TEXTURE_2D, fb.get_texture_id());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    cv::Mat img(height, width, CV_8UC3, pixels);
    cv::imshow("img", img);
    delete[] pixels;
  }

  }while(cv::waitKey(1) != 27);
  r.terminate();
  glfwTerminate();
  return 0;
}
