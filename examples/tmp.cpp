#include <cwchar>
#include <movutl/experimental/pickle.hpp>

#include <fstream>
int main() {
  auto vec = mu::experimental::load_array<float>("distcache.pkl");
  DISP(vec.size());
  const size_t m_width  = 540;
  const size_t m_height = 380;

  std::ofstream myfile;
  myfile.open("distcache.csv");
  int i = 0;
  for(size_t y = 0; y < m_height; y++) {
    for(size_t x = 0; x < m_width - 1; x++) {
      myfile << vec[i] << ",";
      i++;
    }
    myfile << vec[i];
    i++;
    myfile << std::endl;
  }
  myfile.close();

  return 0;
}
