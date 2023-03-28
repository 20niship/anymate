#include <cwchar>
#include <fstream>
#include <movutl/experimental/pickle.hpp>
#include <spdlog/spdlog.h>

namespace mu::experimental {
using namespace mu::core;

static bool exist_file(const std::string& fname) {
  std::ifstream file(fname);
  return file.good();
  if(!file.good()) {
    return false;
  }
  return true;
}

static void writeData(const void* data, const std::string & filename, size_t size) {
DISP(size);
  std::ofstream ofs(filename, std::ios::binary);
  ofs.write((char*)data, size);
  if(ofs.bad())
    spdlog::error("unable to write data filename = {} size = {}",filename, size );
  ofs.close();
}

template <typename T> void save_array(const std::vector<T>& data, const std::string& fname) {
  if(exist_file(fname)) spdlog::warn("file exists : {} overwriting.", fname);
  writeData((const void *)data.data(), fname, data.size() * sizeof(T));
}

template <typename T> void save_array(const core::Vec<T>& data, const std::string& fname) {
  if(exist_file(fname)) spdlog::warn("file exists : {} overwriting.", fname);
  writeData((const void *)data.data(), fname, data.size_in_bytes());
}

template <typename T> core::Vec<T> load_array(const std::string& fname) {
  if(!exist_file(fname)) {
    spdlog::error("file not found : {}", fname);
    return {};
  }

  std::ifstream ifs(fname, std::ios::binary);
  if(!ifs) {
    spdlog::error("unable to open file: {}", fname);
    return {};
  }
  ifs.seekg(0, std::ios::end);
  size_t size = ifs.tellg();
  ifs.seekg(0);
  Vec<T> result;
  result.resize(size / sizeof(T));
  ifs.read((char *)result.data(), size);
  ifs.close();
  return result;
}

template <typename T> std::vector<T> load_array_std(const std::string& fname) {
  if(!exist_file(fname)) {
    spdlog::error("file not found : {}", fname);
    return {};
  }
  std::ifstream ifs(fname, std::ios::binary);
  if(!ifs) {
    spdlog::error("unable to open file: {}", fname);
    return {};
  }
  
  ifs.seekg(0, std::ios::end);
  long long int size = ifs.tellg();
  std::vector<T> result(size / sizeof(T));
  ifs.seekg(0);
  ifs.read((char *)result.data(), size);
  return result;
}

#define EXPLICIT_INSTANCIATE_PICKLE_FUNCS(T) \
template core::Vec<T> load_array<T>(const std::string&); \
template std::vector<T> load_array_std<T>(const std::string&); \
template void save_array<T>(const std::vector<T> &, const std::string&); \
template void save_array<T>(const Vec<T> &, const std::string&);

EXPLICIT_INSTANCIATE_PICKLE_FUNCS(double);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(float);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(int);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(char);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(void *);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(unsigned char);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(short);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(size_t);

EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec2);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec3);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec4);

EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec2d);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec3d);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec4d);

EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec2b);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec3b);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Vec4b);

EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Mat2x2);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Mat3x3);
EXPLICIT_INSTANCIATE_PICKLE_FUNCS(Mat4x4);

} // namespace mu::experimental
