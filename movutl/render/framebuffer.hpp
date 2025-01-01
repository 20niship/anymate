#pragma once
#include <movutl/render/render.hpp>

namespace mu::render{

class Framebuffer{
private:

  bool m_initialized=false;
  int m_width = -1;
  int m_height= -1;

  GLuint m_fbo, m_rbo, m_texture_id;

  public:
    Framebuffer();
    void create();
    bool check_fbo_setup_complete();
    void bind();
    void unbind();
    void set_size(int, int);

    ~Framebuffer();

    GLuint get_fbo_id()const{return m_fbo;}
    GLuint get_texture_id()const{return m_texture_id;}
};

}

