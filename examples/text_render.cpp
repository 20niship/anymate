
#include <codecvt>
#include <movutl/instance/instance.hpp>
#include <iostream>
#include <locale>

using namespace mu::core;
using namespace mu::db;

void print_str(const std::string& str) {
  const std::u32string u32str = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str);
  const auto t                = mu::instance::get_text_renderer();
  MU_ASSERT(t->isBuildFinished);
  const auto tex = mu::instance::get_text_texture();
  MU_ASSERT(tex->isvalid());

  const int w       = tex->width();
  const uint8_t* da = tex->data();

  for(size_t i = 0; i < u32str.size(); i++) {
    std::cout << std::endl << "--------------" << std::endl;
    const auto glyph = t->FindGlyph(u32str[i]);
    for(size_t y = glyph->V0; y <= glyph->V1; y++) {
      for(size_t x = glyph->U0; x <= glyph->U1; x++) {
        const uint8_t v = da[y * w + x];
        std::cout << (v > 100 ? '#' : ' ');
      }
      std::cout << std::endl;
    }
    std::cout << std::endl << "--------------" << std::endl;
  }
}


int main() {
  mu::instance::init();
  auto wnd = mu::ui::create_window("hello", 640, 480);
  print_str("あいうえお");
  wnd->terminate();
  glfwTerminate();
  return 0;
}
