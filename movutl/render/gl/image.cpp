#include <movutl/db/image.hpp>
#include <movutl/instance/instance.hpp>
#include <stb_image.h>

namespace mu::db {

Image::Image() {
  reset();
  tex_id  = 0;
  _data   = nullptr;
  _type   = GL_BITMAP;
  _format = Format::RGB;
  _filter = Filter::NEAREST;
  _width = _height = -1;
  _gpu_updated     = false;
}

Image::Image(const char* image_file_name) {
  glGenTextures(1, &tex_id);
  int nrComponents;
  int w, h;
  _data   = stbi_load(image_file_name, &w, &h, &nrComponents, 0);
  _width  = w;
  _height = h;
  if(_data) {
    GLenum format;
    if(nrComponents == 1)
      format = GL_RED;
    else if(nrComponents == 3)
      format = GL_RGB;
    else if(nrComponents == 4)
      format = GL_RGBA;
    else{
      LOGE << "format not found!!";
      LOGE << "Image::Image() creation failed";
      return;
    }

    glBindTexture(GL_TEXTURE_2D, tex_id);
    if(_data == nullptr){
      LOGW << "image _data is nullptr. send null image to gpu (glTexImage2D)";
      glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, 0);
    }else{
      glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, _data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    _gpu_updated = true;
    /* stbi_image_free(_data); */
  } else {
    std::cout << "Texture failed to load at path: " << image_file_name << std::endl;
    stbi_image_free(_data);
  }
}

Image::Image(int w, int h, const Format format) {
  tex_id       = 0;
  _format      = format;
  _filter      = Filter::NEAREST;
  _width       = w;
  _height      = h;
  _data        = nullptr;
  _type        = GL_UNSIGNED_BYTE;
  _gpu_updated = false;
}

Image::Image(int w, int h, uint8_t* data, const Format format) {
  tex_id       = 0;
  _format      = format;
  _filter      = Filter::NEAREST;
  _width       = w;
  _height      = h;
  _data        = data;
  _type        = GL_BITMAP;
  _gpu_updated = false;
}
Image::Image(int w, int h, uint8_t* data, Format format, Filter filter) {
  tex_id       = 0;
  _format      = format;
  _filter      = filter;
  _width       = w;
  _height      = h;
  _data        = data;
  _type        = GL_BITMAP;
  _gpu_updated = false;
}

Image::Image(int w, int h, uint8_t* data, Format format, Filter filter, GLuint type) {
  tex_id       = 0;
  _format      = format;
  _filter      = filter;
  _width       = w;
  _height      = h;
  _data        = data;
  _type        = type;
  _gpu_updated = false;
}

Image::~Image() { erase(); }

// テクスチャ作成する
// format = GL_RGB : カラー画像、GL_COLOR_INDEX：単一の値で構成されるカラー指標
// filter_type = GL_NEAREST, GL_LINEARがある
bool Image::set_to_gpu() {
  std::cout << "loading texture... (" << _width << ", " << _height << ")" << std::endl;
  MU_ASSERT(_width > 0);
  MU_ASSERT(_height > 0);
  //MU_ASSERT(_data != nullptr);

  glGenTextures(1, &tex_id);
#if 0
  float index[] = {0.0, 1.0};
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, index);
  glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, index);
  glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, index);
  glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, index);
#endif

  // テクスチャをGPUに転送
  glBindTexture(GL_TEXTURE_2D, tex_id);

  /* MU_ASSERT(_type == GL_UNSIGNED_BYTE); */
  /* MU_ASSERT(_format == Format::GRAYSCALE); */
  /* MU_ASSERT((GLenum)_filter == GL_NEAREST); */

  if(_data == nullptr){
    LOGW << "image _data is nullptr. send null image to gpu (glTexImage2D)";
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, (GLenum)_format, GL_UNSIGNED_BYTE, 0);
  }else{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, (GLenum)_format, GL_UNSIGNED_BYTE, _data);
  }
  /* glGenerateMipmap(GL_TEXTURE_2D); */

  // テクスチャを拡大縮小する時のフィルタリング方法を指定
  /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)_filter); */
  /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)_filter); */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //ラッピング方法を指定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); */
  /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */

  // テクスチャのアンバインド
  /* glBindTexture(GL_TEXTURE_2D, 0); */
  /* instance::engine_error_check(); */
  _gpu_updated = true;
  return true;
}

void Image::set_filter(const Filter f) { _filter = f; }
bool Image::isvalid() { return _width > 0 && _height > 0 && _data != nullptr; }
/* void set_type(const Type f); */

void Image::erase() {
  if(_gpu_updated) glDeleteTextures(1, &tex_id);
  LOGE << "destructor called!";
}

uint8_t* Image::get_data_from_gpu() {
  const int ch = 4; // numChannels
  MU_ASSERT(_width > 0);
  MU_ASSERT(_height > 0);
  MU_ASSERT(_gpu_updated);
  MU_ASSERT(_width > 0 &&  _height > 0 && ch > 0);
  GLubyte* pixels = new GLubyte[_width * _height * ch];
  glBindTexture(GL_TEXTURE_2D, tex_id);
  _format = Format::RGBA;
  glGetTexImage(GL_TEXTURE_2D, 0, (GLenum)_format, GL_UNSIGNED_BYTE, pixels);
  return pixels;
}

#if 0
GLuint uiTexture::loadTextureFromFile(std::string filename) {
  // Raw画像の場合
  // // ファイルの読み込み
  // std::ifstream fstr(filename, std::ios::binary);
  // const size_t fileSize = static_cast<size_t>(fstr.seekg(0, fstr.end).tellg());
  // fstr.seekg(0, fstr.beg);
  // char* textureBuffer = new char[fileSize];
  // fstr.read(textureBuffer, fileSize);

  cv::Mat img = cv::imread(filename);
  if(img.empty()) return 0;
  GLuint idtemp = loadTexture(img.data, img.rows, img.cols, GL_RGB, GL_LINEAR);
  texIDs.push_back(idtemp);
  return idtemp;
}

#endif

} // namespace mu::db
