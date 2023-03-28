#include <movutl/core/logger.hpp>
#include <movutl/instance/instance.hpp>
#include <movutl/render/render.hpp>
#include <movutl/ui/ui.hpp>


namespace mu::render {

void Render::draw() {
  MU_ASSERT(mesh_ui != nullptr);
  MU_ASSERT(mesh_col != nullptr);
  /* MU_ASSERT(mesh_ui->get_vertices_size() > 0); */
  /* MU_ASSERT(mesh_col->get_vertices_size() > 0); */

  if(wnd != nullptr && wnd != NULL) {
    glfwGetWindowSize(wnd, &m_width, &m_height);
    MU_ASSERT(m_width > 0);
    MU_ASSERT(m_height > 0);

  } else {
    // TODO:
    m_width  = 1080;
    m_height = 720;
  }

  // clang-format off
  const Mat4x4f proj_mat = {
    2.0f / float(m_width), 0.0f, 0.0f, 0.0f, 
    0.0f, -2.0f / float(m_height), 0.0f, 0.0f, 
    0.0f, 0.0f, 1.0f, 0.0f, 
    -1.0f, 1.0f, 0.0f, 1.0f
  };
  // clang-format on

  const auto t = instance::get_text_renderer();
  const Vec2f uvsize(t->getTexWidth(), t->getTexHeight());

  if(wnd == nullptr) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      LOGE << " ERROR framebuffer status failed";
      return;
    }
  }
  glLineWidth(gl_line_width);
  gl_point_size = std::max(1, gl_point_size);
  glPointSize(gl_point_size);
  MU_ASSERT(m_width > 0 && m_height > 0);
  glViewport(0, 0, m_width, m_height);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(m_bg[0], m_bg[1], m_bg[2], m_bg[3]);

  const auto shader_col = instance::get_col_shader();
  const auto shader_ui  = instance::get_ui_shader();
  const auto shader_obj = instance::get_3d_shader();

  MU_ASSERT(shader_col != nullptr);
  MU_ASSERT(shader_col->enabled());
  MU_ASSERT(shader_ui != nullptr);
  MU_ASSERT(shader_ui->enabled());
  MU_ASSERT(shader_obj != nullptr);
  MU_ASSERT(shader_obj->enabled());

  using namespace db;
  Mat4x4f model;
  model.identify();
  for(const auto& m : meshes) {
    MU_ASSERT(m);
    if(m->get_vertices_size() == 0) continue;
    if(m == mesh_ui) continue;
    const auto ty = m->get_mesh_type();
    switch(ty) {
      case _MeshBase::MeshType::Col: {
        const Mat4x4f proj_camera = camera.getCameraProjMatrix();
        const auto sh             = (shader != nullptr) ? shader : shader_col;
        sh->use();
        sh->set_value("proj", proj_camera);
        sh->set_value("uvsize", uvsize);
        sh->set_value("model", model);
        m->draw(sh);
        break;
      }
      case _MeshBase::MeshType::UI: {
        shader_ui->use();
        shader_ui->set_value("proj", proj_mat);
        shader_ui->set_value("uvsize", uvsize);
        m->draw(shader_ui);
        break;
      }
      case _MeshBase::MeshType::Object: {
        const Mat4x4f proj_camera = camera.getCameraProjMatrix();
        shader_obj->use();
        shader_obj->set_value("proj", proj_camera);
        shader_obj->set_value("uvsize", uvsize);
        shader_obj->set_value("model", model);
        m->draw(shader_obj);
        break;
      }
      default: LOGE << "Undefined Mesh type!"; exit(1);
    }
  }

  for(const auto& obj : bodies) {
    const auto model_transform_mat = obj->get_model_transform_matrix();

    MU_ASSERT(obj);
    if(obj->meshs.size() == 0) continue;
    for(const auto& m : obj->meshs) {
      MU_ASSERT(m);
      if(m->get_vertices_size() == 0) continue;
      if(m == mesh_ui) continue;
      const auto ty = m->get_mesh_type();
      switch(ty) {
        case _MeshBase::MeshType::Col: {
          const Mat4x4f proj_camera = camera.getCameraProjMatrix();
          const auto sh             = (shader != nullptr) ? shader : shader_col;
          sh->use();
          sh->set_value("proj", proj_camera);
          sh->set_value("uvsize", uvsize);
          sh->set_value("model", model_transform_mat);
          m->draw(sh);
          break;
        }
        case _MeshBase::MeshType::UI: {
          shader_ui->use();
          shader_ui->set_value("proj", proj_mat);
          shader_ui->set_value("uvsize", uvsize);
          m->draw(shader_ui);
          break;
        }
        case _MeshBase::MeshType::Object: {
          const Mat4x4f proj_camera = camera.getCameraProjMatrix();
          shader_obj->use();
          shader_obj->set_value("proj", proj_camera);
          shader_obj->set_value("uvsize", uvsize);
          shader_obj->set_value("model", model_transform_mat.Trans());
          m->draw(shader_obj);
          break;
        }
        default: LOGE << "Undefined Mesh type!"; exit(1);
      }
    }
  }

  if(mesh_ui != nullptr) {
    glDisable(GL_DEPTH_TEST);
    // glClear(GL_DEPTH_BUFFER_BIT);
    shader_ui->use();
    shader_ui->set_value("proj", proj_mat);
    shader_ui->set_value("uvsize", uvsize);
    mesh_ui->draw(shader_ui);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::summary() const {
  using namespace std;
  using namespace db;
  cout << "----------    Render Summary ----------------- " << std::endl;
  cout << "mesh    count = " << meshes.size() << endl;
  cout << "texture count = " << 0 << endl;
  cout << "# mesh report " << endl;
  cout << "default mesh ui = " << (mesh_ui != nullptr ? "YES" : "NO") << endl;
  cout << "default mesh col = " << (mesh_col != nullptr ? "YES" : "NO") << endl;
  int i = 0;
  core::Vec<Image*> textures;

  for(auto&& m : meshes) {
    i++;
    const auto t     = m->get_mesh_type();
    const auto sv    = m->get_vertices_size();
    const auto si    = m->get_indices_size();
    const auto c     = m->gpu_buf_created();
    const auto u     = m->gpu_buf_updated();
    const auto st    = m->textures.size();
    const char* tstr = (t == _MeshBase::MeshType::Col ? "color" : (t == _MeshBase::MeshType::Object ? "object" : "ui"));

    cout << "  ## mesh " << i << " type = " << tstr << std::endl;
    cout << "    vertex   : " << sv << endl;
    cout << "    indices  : " << si << endl;
    cout << "    textures : " << st << endl;
    cout << "    gpu create: " << c << endl;
    cout << "    gpu update: " << u << endl;
    if(st > 0)
      for(auto&& te : m->textures) textures.push_back(te);
  }

  i = 0;
  cout << "# texture report " << endl;
  cout << "texture count = " << textures.size() << endl;
  if(textures.size() > 0) {
    i++;
    for(auto&& t : textures) {
      const auto w    = t->width();
      const auto h    = t->height();
      const auto name = t->texture_name();
      const auto path = t->path();
      const auto id   = t->get_tex_id();
      const auto va   = t->isvalid();

      cout << "  ## texture" << i << " name = " << name << std::endl;
      cout << "    size  : (" << w << " , " << h << " )" << endl;
      cout << "    path  : " << path << endl;
      cout << "    id    : " << id << endl;
      cout << "    valid : " << va << endl;
    }
  }
  cout << "----------    Render Summary ----------------- " << std::endl;
}

void Render::createSurface(GLFWwindow* glfw_window) {
  assert(glfw_window != nullptr);
  wnd = glfw_window;
  glfwMakeContextCurrent(glfw_window);
  instance::get_engine()->init_after_surface_creation();
  /* glDisable(GL_DEPTH_TEST); */
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Render::terminate() {
  //TODO: Render::terminate();
}
void Render::init() {
  create_default_mesh();
}

cv::Mat Render::get_color_image() {
  MU_ASSERT(m_width > 0);
  MU_ASSERT(m_height > 0);
  cv::Mat img = cv::Mat(cv::Size(m_width, m_height), CV_8UC3);
  glReadPixels(0, 0, m_width, m_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
  cv::flip(img, img, 0);
  return img;
}

cv::Mat Render::get_depth_image() {
  MU_ASSERT(m_width > 0);
  MU_ASSERT(m_height > 0);
  cv::Mat img = cv::Mat(cv::Size(m_width, m_height), CV_32F);
  glReadPixels(0, 0, m_width, m_height, GL_DEPTH_COMPONENT, GL_FLOAT, img.data);
  cv::flip(img, img, 0);
  return img;
}

} // namespace mu::render
