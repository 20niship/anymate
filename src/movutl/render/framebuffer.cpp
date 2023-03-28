#include <movutl/render/framebuffer.hpp>

namespace mu::render{

  Framebuffer::Framebuffer(){

  }

  void Framebuffer::set_size(int w, int h){
    m_width = w;
    m_height = h;
  }

  void Framebuffer::create(){
    if(m_initialized)return;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height); 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo); 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        LOGE << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_initialized = true;
  }

  bool Framebuffer::check_fbo_setup_complete(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE);
  }

  void Framebuffer::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  }

  void Framebuffer::unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  Framebuffer::~Framebuffer(){
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteFramebuffers(1, &m_rbo);
    glDeleteTextures(1,&m_texture_id);
  }
}
