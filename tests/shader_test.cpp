#include <movutl/mu.hpp>

void get_shader_error_msg(GLuint s) {
  char buffer[4096];
  glGetShaderInfoLog(s, 4096, NULL, buffer);
  std::cout << "Compile Error = \n----------------------------\n" << buffer << "-------------\n";
}

int main() {
  mu::instance::init();
  mu::ui::create_window("test", 640, 480);

  const char* vs_src = R"(
    #version 400
    uniform vec2 uvsize;
    uniform mat4 proj;
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec2 vuv;
    layout (location = 2) in vec3 color;
    out vec2 Frag_uv;
    out vec3 outColor;
    void main(){
        Frag_uv= vec2(vuv.x / uvsize.x, vuv.y /uvsize.y);
        outColor= vec3(color) / 256.0;
        gl_Position = proj* vec4(vec2(position.xy),0.0 ,1.0);
    }
  )";

  const char* fs_src = R"(
    #version 130
    in vec2 Frag_uv;
    in vec3 outColor;
    uniform sampler2D texture;
    out vec4 col;
    void main(void) {
      col= vec4(outColor, 1.0);
      if(Frag_uv.x != 0.0 && Frag_uv.y != 0.0){
        col.a = texture2D(texture, Frag_uv).x ;
      }else{
        col.a = 1.0;
      }
    };
  )";


  const auto vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vs_src, NULL);
  glCompileShader(vs);

  const auto fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fs_src, NULL);
  glCompileShader(fs);

  const auto shader_id = glCreateProgram();

  GLint success = 0;
  glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
  if(success == GL_FALSE) {
    LOGE << "[ERROR] Vertex shader Compile Failed\n";
    get_shader_error_msg(vs);
    return -1;
  } else {
    LOGI << "vertex Shader compile success!!\n";
  }

  glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
  if(success == GL_FALSE) {
    LOGE << "[ERROR] Fragment shader Compile Failed\n";
    DISP(fs_src);
    get_shader_error_msg(fs);
    return -1;
  } else {
    LOGI << "fragment Shader compile success!!\n";
  }
  glAttachShader(shader_id, vs);
  glAttachShader(shader_id, fs);
  glLinkProgram(shader_id);
  LOGI << "shader compile finished";
  return 0;
}

