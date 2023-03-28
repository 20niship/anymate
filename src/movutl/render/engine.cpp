#include <movutl/instance/instance.hpp>
#include <movutl/render/engine.hpp>

namespace mu::render {

bool Engine::init() {
  if(init_finished) {
    LOGD << "Engine::init already finished! skipping.......";
    return false;
  }
  if(!glfwInit()) {
    LOGE << "ERROR: could not start GLFW3\n";
    return false;
  }
#ifdef WITH_OPENGL
  LOGI << "OpenGL Mode Initialization;";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  // 古い機能を削除したプロファイルを使用するか
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // OpenGLのプロファイルを指定する
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
  // glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  // glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
  // glfwDisable(GLFW_AUTO_POLL_EVENTS);
#ifdef DEBUG
  // glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#else
  LOGI << "Vulkan Mode Initialization;";
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
  init_finished = true;
  return true;
}


const char* get_glenum_source_str(GLenum source) {
  switch(source) {
    case GL_DEBUG_SOURCE_API: return "API";

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";

    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";

    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";

    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";

    case GL_DEBUG_SOURCE_OTHER: return "OTHER";

    default: return "UNKNOWN"; break;
  }
}

bool Engine::init_after_surface_creation() {
  if(!init_finished) {
    LOGE << "Windowが作られる前にEngine::init_after_surface_creationが呼ばれた!";
    init();
  }
  glewExperimental = GL_TRUE;
  const auto err   = glewInit();
  if(err != GLEW_OK) {
    LOGE << "glewInit() failed!";
    std::cerr << "GLEW Error : " << glewGetErrorString(err) << std::endl;
    return false;
  }

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(
    [](GLenum source, auto type, auto id, auto severity, auto length, const auto* msg, [[maybe_unused]] const void* userProgram) {
      const bool iserror = type == GL_DEBUG_TYPE_ERROR;
      const auto t       = iserror ? "*** GL DEBUG ERROR ***" : "";
      if(iserror)
        spdlog::error("OpenGL Callback  {}  type={}  severity = {}  msg = {} source={} id={} length={}", t, type, severity, msg, get_glenum_source_str(source), id, length);
      else
        spdlog::debug("OpenGL Callback  {}  type={}  severity = {}  msg = {} source={} id={} length={}", t, type, severity, msg, get_glenum_source_str(source), id, length);

#if 0
      GLenum error_code = glGetError();
      do {
        const char* msg = "";
        switch(error_code) {
          case GL_INVALID_ENUM: msg = "INVALID_ENUM"; break;
          case GL_INVALID_VALUE: msg = "INVALID_VALUE"; break;
          case GL_INVALID_OPERATION: msg = "INVALID_OPERATION"; break;
          case GL_OUT_OF_MEMORY: msg = "OUT_OF_MEMORY"; break;
          case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "INVALID_FRAMEBUFFER_OPERATION"; break;
          default: msg = "Unknown"; break;
        }
        if(error_code != GL_NO_ERROR)
          spdlog::error("glGetError() ERROR : code = {}, msg={}", error_code, msg);
        error_code = glGetError();
#ifdef _WIN32
      } while(false);
#else
      } while(error_code != GL_NO_ERROR);
#endif
#endif

      if(type == GL_DEBUG_TYPE_ERROR) {
        throw std::runtime_error("OpenGL Callback ERrror!");
      }
    },
    0);

  const GLubyte* renderer;
  const GLubyte* version;
  renderer = glGetString(GL_RENDERER);
  version  = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  init_shaders();
  init_font_renderer();
  init_textures();
  LOGI << "engine init finish!";
  init_finished = true;
  return true;
}


bool Engine::init_shaders() {
  shaders.resize(3);

  shaders[0].set_default_ui_shader_src();
  shaders[0].compile();

  shaders[1].set_default_default_shader_src();
  shaders[1].compile();

  shaders[2].set_default_3dcol_shader_src();
  shaders[2].compile();
  return true;
}

bool Engine::init_font_renderer() {
  text_renderer.init();
  text_renderer.setLanguage(FontLanguage::Japansese);
  text_renderer.build();
  return true;
}

bool Engine::init_textures() {
  if(textures.size() > 0) return false;
  textures.clear();
  textures.resize(1);
  textures[0].width(text_renderer.getTexWidth());
  textures[0].height(text_renderer.getTexHeight());
  textures[0].set_data(text_renderer.getData());
  textures[0].set_format(db::Image::Format::GRAYSCALE);
  textures[0].set_filter(db::Image::Filter::NEAREST);
  textures[0].set_type(GL_UNSIGNED_BYTE);
  std::cout << "texture valid = " << (textures[0].isvalid() ? "OK" : "NG") << std::endl;
  textures[0].set_to_gpu();

  font_texture_idx = 0;

  return true;
}

Engine::~Engine() { init_finished = false; }
Engine::Engine() { /*init();*/
}

bool Engine::terminate() {
  LOGE << "TODO! Engine::terminate!";
  shaders.clear();
  textures.clear();
  text_renderer.~uiFont();
  glfwTerminate();
  return true;
}

char const* gl_error_string(GLenum const err) noexcept {
  switch(err) {
    // opengl 2 errors (8)
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    case GL_TABLE_TOO_LARGE: return "GL_TABLE_TOO_LARGE";
    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default: assert(!"unknown error"); return nullptr;
  }
}


bool Engine::error_check() {
  auto err = glGetError();
  if(err != GL_NO_ERROR) {
    auto errString = gl_error_string(err);
    LOGE << "## OpenGL ERROR ## " + std::string((char*)errString);
  }
  return err == GL_NO_ERROR;
}

} // namespace mu::render
