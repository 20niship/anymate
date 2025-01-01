#include <movutl/core/logger.hpp>
#include <movutl/db/shader.hpp>
#include <movutl/render/render.hpp>

namespace mu::db {

Shader::Shader() { compiled = false; }
void Shader::use() { glUseProgram(shader_id); }
unsigned int Shader::get_shader_id() { return shader_id; }

Shader::Shader(const char* vs_, const char* fs_) {
  compiled = false;
  set_shader_str(vs_, fs_);
  compile();
}

void Shader::set_shader_str(const char* vs_, const char* fs_) {
  fs_src = fs_;
  vs_src = vs_;
}

void Shader::set_default_ui_shader_src() {
#if 0
  vertex_shader = /* "#version 400\n" */
    "#version 130\n"
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in vec2 vuv;\n"
    "layout(location = 2) in vec3 color;\n"
    "uniform mat4 projectionMatrix;"
    "uniform vec2 uvsize;"
    "out vec2 Frag_uv;"
    "out vec3 outColor;"
    "void main(void) {"
    "outColor = color;"
    "Frag_uv= vec2(vuv.x*uvsize.x, vuv.y*uvsize.y);"
    //"Frag_uv= vuv;n"
    "gl_Position = projectionMatrix *  vec4(position, 0.0f, 1.0f);"
    "}\n";
#else
  // #version 130
  vs_src = R"(
    #version 450 
    uniform vec2 uvsize;
    uniform mat4 proj;
    in vec2 position;
    in vec2 vuv;
    in vec3 color;
    out vec2 Frag_uv;
    out vec3 outColor;
    void main(){
        Frag_uv= vec2(vuv.x / uvsize.x, vuv.y /uvsize.y);
        outColor= vec3(color) / 256.0;
        gl_Position = proj* vec4(vec2(position.xy),0.0 ,1.0);
    }
  )";

#endif
  fs_src = R"(
    #version 450 
    in vec2 Frag_uv;
    in vec3 outColor;
    uniform sampler2D mysampler;
    out vec4 col;
    void main(void) {
      col= vec4(outColor, 1.0);
      if(Frag_uv.x != 0.0 && Frag_uv.y != 0.0){
        col.a = texture(mysampler, Frag_uv).x ;
      }else{
        col.a = 1.0;
      }
    };
  )";
}

void Shader::set_default_3dcol_shader_src() {
  vs_src = R"(
    #version 450 
    uniform mat4 proj;
    uniform mat4 model;
    uniform mat4 view;
    uniform vec2 uvsize;
    in vec3 position;
    in vec3 color;
    in vec2 vuv;
    out vec3 outColor;
    out vec2 Frag_uv;
    void main() {
      gl_Position = proj *model* vec4(position, 1.0);
      Frag_uv= vec2(vuv.x / uvsize.x, vuv.y /uvsize.y);
      outColor= vec3(color) / 256.0;
    }
  )";

  fs_src = R"(
    #version 450 
    in vec2 Frag_uv;
    in vec3 outColor;
    uniform sampler2D mysampler;
    out vec4 col;
    void main(void) {
      col= vec4(outColor, 1.0);
      if(Frag_uv.x != 0.0 || Frag_uv.y != 0.0){
        col.a = texture(mysampler, Frag_uv).x;
      }else{
        col.a = 1.0;
      }
    };
  )";
}

void Shader::set_default_default_shader_src() {
  vs_src = R"(
    #version 450 
    uniform mat4 proj;
    uniform mat4 model;
    uniform mat4 view;
    uniform vec2 uvsize;
    in vec3 position;
    in vec3 norm;
    in vec2 vuv;
    in vec3 tangent;
    in vec3 bitangent;
    in vec4 bone;
    in vec4 weight;

    out vec3 Normal;
    out vec2 Frag_uv;
    out vec3 FragPos;

    void main() {
      gl_Position = proj * model * vec4(position, 1.0);
      Frag_uv= vec2(vuv.x / uvsize.x, vuv.y /uvsize.y);
      FragPos = vec3(model * vec4(position, 1.0));
      /* Normal= normalize(mat3(transpose(inverse(model))) * norm); */
      Normal= normalize(norm);
    }
  )";

  fs_src = R"(
    #version 450 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

/* uniform DirLight dir_light; */
/* uniform Material material; */

uniform sampler2D mysampler;
uniform vec3 viewPos;

in vec2 Frag_uv;
in vec3 FragPos;
in vec3 Normal;

out vec4 col;

vec3 CalcDirLight(DirLight light, vec3 n, vec3 dir){
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, n);

Material material;
material.diffuse=vec3(0.1, 0.2, 0.3);
material.specular=vec3(1.0, 1.0, 1.0);
material.shininess = 32.0;

    float spec = pow(max(dot(dir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return ambient + diffuse + specular;
}

void main(void) {
    vec3 dir = normalize(viewPos - FragPos);

DirLight dir_light;
    dir_light.diffuse = vec3(1.0, 1.0, 1.0);
    dir_light.ambient = vec3(1, 1, 1);
    dir_light.specular= vec3(10.0, 10.0, 8.0);
    dir_light.direction = vec3(1.0, 1.0, 1.0);

    vec3 result = CalcDirLight(dir_light, Normal ,dir);
    col= vec4(result, 1.0) + texture(mysampler, Frag_uv).x *0.01;
    col.a = 1.0;
};
  )";
}


void get_shader_error_msg(GLuint s) {
  char buffer[4096];
  glGetShaderInfoLog(s, 4096, NULL, buffer);
  std::cout << "Compile Error = \n----------------------------\n" << buffer << "-------------\n";
}

bool Shader::compile() {
  LOGI << "compiling shader......";
  assert(strlen(vs_src) > 0);
  assert(strlen(fs_src) > 0);

  try {

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, NULL);
    glCompileShader(vs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(fs);

    shader_id = glCreateProgram();

    GLint success = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
      LOGE << "[ERROR] Vertex shader Compile Failed\n";
      DISP(vs_src);
      get_shader_error_msg(vs);
      compiled = false;
      return false;
    } else {
      LOGI << "vertex Shader compile success!!\n";
    }

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
      LOGE << "[ERROR] Fragment shader Compile Failed\n";
      DISP(fs_src);
      get_shader_error_msg(fs);
      compiled = false;
      return false;
    } else {
      std::cout << "fragment Shader compile success!!\n";
    }
    glAttachShader(shader_id, vs);
    glAttachShader(shader_id, fs);
    glLinkProgram(shader_id);
    compiled = true;
    LOGI << "shader compile finished";
  } catch(std::runtime_error& e) {
    LOGE << "Shader compile Error : " << e.what();
    std::cerr << "vertex shader = " << vs_src << std::endl;
    std::cerr << "fragment shader = " << fs_src << std::endl;
    compiled = false;
  }
  return true;
}


bool Shader::set_value(const char* name, float value) {
  MU_ASSERT(compiled);
  const auto at = glGetUniformLocation(shader_id, name);
  if(at < 0) {
    std::stringstream str;
    str << value;
    spdlog::warn("shader attribute not found {} = {}", name, str.str());
  } else {
    glUniform1fARB(at, value);
  }
  return true;
}
bool Shader::set_value(const char* name, const core::Mat4x4f& value) {
  MU_ASSERT(compiled);
  const auto at = glGetUniformLocation(shader_id, name);
  if(at < 0) {
    std::stringstream str;
    str << value;
    spdlog::warn("shader attribute not found {} = {}", name, str.str());
  } else {
    glUniformMatrix4fv(at, 1, GL_FALSE, value.value);
  }
  return true;
}

bool Shader::set_value(const char* name, core::Vec2f value) {
  MU_ASSERT(compiled);
  const auto at = glGetUniformLocation(shader_id, name);
  if(at < 0) {
    std::stringstream str;
    str << value;
    spdlog::warn("shader attribute not found {} = {}", name, str.str());
  } else {
    glUniform2f(at, value[0], value[1]);
  }
  return true;
}
bool Shader::set_value_img(const char* name, int texture_id, int index) {
  MU_ASSERT(compiled);
  const auto at = glGetUniformLocation(shader_id, name);
  if(at < 0) {
    spdlog::warn("shader attribute not found   : {} = {} {}", name, texture_id, index);
  } else {
    /* glActiveTexture(GL_TEXTURE0); */
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(at, index);
    glBindTexture(GL_TEXTURE_2D, texture_id);
  }
  return true;
}


int Shader::get_uni_loc(const char* name) { return glGetUniformLocation(shader_id, name); }
int Shader::get_attrib_loc(const char* name) { return glGetAttribLocation(shader_id, name); }

Shader::~Shader() { glDeleteShader(shader_id); }
} // namespace mu::db
