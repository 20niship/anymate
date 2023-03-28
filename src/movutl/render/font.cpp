#include <movutl/core/logger.hpp>
#include <movutl/render/font.hpp>
#include <sstream>

#ifndef GL_MAX_TEXTURE_SIZE
#define GL_MAX_TEXTURE_SIZE 2048
#endif

namespace mu::render {

// -----------------------------------------------------
//    [SECTION] uiFont
// -----------------------------------------------------
#if 0
void uiFont::AddGlyph(uiWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x) {}
void uiFont::AddRemapChar(uiWchar dst, uiWchar src, bool overwrite_dst) {}
void uiFont::SetGlyphVisible(uiWchar c, bool visible) {}
bool uiFont::IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last) { return true; }
#endif

constexpr uiWchar _FallbackGlyphNumber = 0x0030;

uiWchar* uiFont::GetGlyphRangesKorean() { return nullptr; }
uiWchar* uiFont::GetGlyphRangesChinese() { return nullptr; }
uiWchar* uiFont::GetGlyphRangesCyrillic() { return nullptr; }
uiWchar* uiFont::GetGlyphRangesThai() { return nullptr; }
uiWchar* uiFont::GetGlyphRangesVietnamese() { return nullptr; }

uiWchar* uiFont::GetGlyphRangesJapanese() {
  static uiWchar ranges[] = {
#include "../../../data/font/Glyphs/GlyphRange_JPN.csv"
    _FallbackGlyphNumber,
  };
  nGlyph = sizeof(ranges) / sizeof(uiWchar);
  return ranges;
}

uiWchar* uiFont::getGlyphRangeIcon() {
  static uiWchar ranges[] = {
#include "../../../data/font/Glyphs/Icon.csv"
  };
  nIconGlyph = sizeof(ranges) / sizeof(uiWchar);
  return ranges;
}

uiWchar* uiFont::GetGlyphRangesEnglish() {
  static uiWchar ranges[] = {
#include "../../../data/font/Glyphs/GlyphRange_ENG.csv"
    _FallbackGlyphNumber,
  };
  nGlyph = sizeof(ranges) / sizeof(uiWchar);
  return ranges;
}

uiFont::uiFont() : TexWidth(0), TexHeight(0), isBuildFinished(false), _Data(nullptr) { init(); }


uiFont::~uiFont() {
  if(_Data != nullptr) free(_Data);
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  TexWidth  = 0;
  TexHeight = 0;
  // if (FallbackGlyph != nullptr) free(FallbackGlyph);
  isBuildFinished = false;
  IndexLookup.clear();
  Glyphs.clear();
}


void uiFont::init() {
  TexWidth        = 0;
  TexHeight       = 0;
  isBuildFinished = false;

  if(_Data != nullptr) free(_Data);
  _Data = nullptr;

  IndexLookup.clear();
  Glyphs.clear();

  IconGlyphRanges = getGlyphRangeIcon();

  setFontSize(16.0f);
  setFontName(std::string(MOVUTL_DATA_DIR) + "/Meiryo.ttf");
  setIconFontName(std::string(MOVUTL_DATA_DIR) + "/MesloLGS.ttf");
  setFontSpacing(2.0f);
}

bool uiFont::setLanguage(FontLanguage l) {
  // static uiWchar *glyphrange_tmp = nullptr;
  switch(l) {
    case FontLanguage::Japansese: GlyphRanges = GetGlyphRangesJapanese(); break;
    case FontLanguage::Chinese: GlyphRanges = GetGlyphRangesChinese(); break;
    case FontLanguage::English: GlyphRanges = GetGlyphRangesEnglish(); break;
    case FontLanguage::Korean: GlyphRanges = GetGlyphRangesKorean(); break;
    default: return false;
  }
  return true;
}

// #undef RENDER_FONT_UNPACK_ALIGNMENT //
//

unsigned int TexCapacityHeight = 128;

bool uiFont::build_internal(const std::string& fontname, const int fontsize, const uiWchar* GlyphRange, const size_t N) {
  FT_Error error, error2;
  {
    //フォントレンダリングエンジンの初期化
    error = FT_Init_FreeType(&library);
    if(error) {
      std::cerr << "[ERROR] Failed to init FreeType library!" << std::endl;
    }

    error2 = FT_New_Face(library, fontname.c_str(), 0, &face);
    if(error2 == FT_Err_Unknown_File_Format) {
      LOGE << "[ERROR] Font file format is not supported!! ";
      LOGE << fontname;
      return false;
    } else if(error2) {
      LOGE << "[ERROR] Font file not found or it is broken! ";
      LOGE << fontname;
      return false;
    }

    // 3. 文字サイズを設定
    // error = FT_Set_Char_Size(face, 0,
    //                 size, // 幅と高さ
    //                 100, 100);  // 水平、垂直解像度
    error2 = FT_Set_Pixel_Sizes(face, fontsize, 0);
    slot   = face->glyph; // グリフへのショートカット

    if(error || error2) {
      LOGE << "an error happened during initializing freetype library\n";
      return false;
    }
  }

  unsigned int CursorY       = TexHeight;
  unsigned int CursorX       = 0;
  unsigned int TextMaxHeight = 0;

  FT_UInt glyph_index;
  int i, j, p, q, index, w, h, x_max, y_max;
  uiGlyph g_tmp;

  int __nGlyph_enabled = 0;
  for(size_t n = 0; n < N; n++) {
    // reder one charf
    glyph_index = FT_Get_Char_Index(face, GlyphRange[n]);
    error       = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    // FT_GlyphSlot_Oblique(face->glyph );
    // FT_GlyphSlot_Embolden(face->glyph );

    error2 = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    if(error || error2) {
      LOGE << "No matching glyph for char-> " + std::to_string(GlyphRange[n]);
    } else {
      w = (slot->bitmap).width;
      h = (slot->bitmap).rows;

      if(w > 30 || h > 30) {
        std::stringstream ss;
        ss << "Font size too large!" << w << " , " << h << " at " << fontname << " " << n << " -- > 0x" << std::hex << GlyphRange[n];
        LOGE << ss.str();
        continue;
      }

      if(w + CursorX > desiredTextSize) {
        CursorX = 0;
        CursorY = TextMaxHeight;
      }

      while(TexCapacityHeight <= CursorY + h + 100) {
        constexpr int glow_dh = 256;
#ifdef RENDER_FONT_UNPACK_ALIGNMENT
        unsigned char* new_data = (unsigned char*)malloc(desiredTextSize * (TexCapacityHeight + glow_dh) / 8);
        if(_Data) {
          memcpy(new_data, _Data, desiredTextSize * TexCapacityHeight / 8);
          free(_Data);
        }
        _Data = new_data;
        memset(_Data + desiredTextSize * TexCapacityHeight / 8, 0x00, desiredTextSize * glow_dh / 8);
#else
        unsigned char* new_data = (unsigned char*)malloc(desiredTextSize * (TexCapacityHeight + glow_dh));
        if(_Data) {
          memcpy(new_data, _Data, desiredTextSize * TexCapacityHeight);
          free(_Data);
        }
        _Data = new_data;
        memset(_Data + desiredTextSize * TexCapacityHeight, 0x00, desiredTextSize * glow_dh);
#endif
        TexCapacityHeight += glow_dh;
      }

      x_max = w + CursorX;
      y_max = h + CursorY;

      for(i = CursorX, p = 0; i < x_max; i++, p++) {
        for(j = CursorY, q = 0; j < y_max; j++, q++) {
          if(p < 0 || q < 0 || p >= w || q >= h) continue;
          index = j * desiredTextSize + i;
#ifdef RENDER_FONT_UNPACK_ALIGNMENT
          _Data[index / 8] |= static_cast<unsigned char>(((slot->bitmap).buffer[q * w + p] > 70 ? 1 : 0) * std::pow(2.0, 7 - index % 8));
#else
          _Data[index] = (slot->bitmap).buffer[q * w + p];
#endif
        }
      }

      // IndexLookup.push_back(__nGlyph_enabled);
      // g_tmp.Codepoint = GlyphRanges[n];
      g_tmp.dHeight = slot->bitmap_top;
      g_tmp.U0      = CursorX;
      g_tmp.V0      = CursorY;
      g_tmp.U1      = CursorX + w;
      g_tmp.V1      = CursorY + h;
      Glyphs.push_back(g_tmp);
      IndexLookup[GlyphRange[n]] = Glyphs.size() - 1;

      // std::cout << "[" << __nGlyph_enabled << "] '" << GlyphRanges[n] << " ("<< char(GlyphRanges[n]) << ")' --> at (" << g_tmp.U0 << ", " << g_tmp.V0 << ", " << g_tmp.U1 << ", " << g_tmp.V1 <<
      // ")\n";

      __nGlyph_enabled++;
      CursorX += w;
      TextMaxHeight = std::max(TextMaxHeight, CursorY + h);
    }
  }

  FallbackGlyph   = &(Glyphs[Glyphs.size() - 1]);
  bool loop       = false;
  i               = 0;
  TexUvWhitePixel = {-1, -1};
  while(loop) {
#ifdef RENDER_FONT_UNPACK_ALIGNMENT
    if(int(int(_Data[i / 8]) / std::pow(2.0, 7 - i % 8)) % 2 == 1) {
      TexUvWhitePixel = {i % desiredTextSize, i / desiredTextSize};
      std::cout << "White texture found!" << std::endl;
      loop = false;
    }
#else
    if(_Data[i] >= 0xF0) {
      TexUvWhitePixel = {i % desiredTextSize, i / desiredTextSize};
      std::cout << "White texture found!" << std::endl;
      loop = false;
    }
#endif
    i++;
  }
  if(TexUvWhitePixel[0] < 0) {
    _Data[0]        = 0xFF;
    TexUvWhitePixel = {0, 0};
    LOGE << "White Texture not found!!"; 
  }

  nGlyph             = __nGlyph_enabled;
  TexWidth           = desiredTextSize;
  TexHeight          = TextMaxHeight; // + CursorY;
  TexHeight_capacity = TexCapacityHeight;
  isBuildFinished    = true;

  std::cout << "uiFont init finished!  " << TexWidth << " ,  " << TexHeight << std::endl;
  return true;
}

//フォンﾄトをレンダリングする
bool uiFont::build() {
  init();
  MU_ASSERT(FontSize > 0);
  MU_ASSERT(GlyphRanges != nullptr);
  MU_ASSERT(isBuildFinished == false);

  { // 色々初期化
    TexWidth        = 0;
    TexHeight       = 0;
    isBuildFinished = false;
    if(_Data != nullptr) free(_Data);
    _Data = nullptr;
    IndexLookup.clear();
    Glyphs.clear();
  }

  // ImFontAtlasBuildInit(atlas);
  // // Clear atlas
  // atlas->TexID = NULL;
  // atlas->TexWidth = atlas->TexHeight = 0;
  // atlas->TexUvScale = ImVec2(0.0f, 0.0f);
  // atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
  // atlas->ClearTexData();

  // // Temporary storage for building
  // ImVector<ImFontBuildSrcData> src_tmp_array;
  // ImVector<ImFontBuildDstData> dst_tmp_array;
  // src_tmp_array.resize(atlas->ConfigData.Size);
  // dst_tmp_array.resize(atlas->Fonts.Size);
  // memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
  // memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

  desiredTextSize = (2048 > GL_MAX_TEXTURE_SIZE) ? int(std::pow(2, int(log2(GL_MAX_TEXTURE_SIZE)))) : 2048;
  std::cout << "desired texture width = " << desiredTextSize << std::endl;

#ifdef RENDER_FONT_UNPACK_ALIGNMENT
  _Data = (unsigned char*)malloc(desiredTextSize * TexCapacityHeight / 8);
  memset(_Data, 0x00, desiredTextSize * TexCapacityHeight / 8);
#else
  _Data = (unsigned char*)malloc(desiredTextSize * TexCapacityHeight);
  memset(_Data, 0x00, desiredTextSize * TexCapacityHeight);
#endif

  Glyphs.clear();
  IndexLookup.resize(65536);
  for(size_t i = 0; i < IndexLookup.size(); i++)
    IndexLookup[i] = 0xFFFF;

  build_internal(FontName, FontSize, GlyphRanges, nGlyph);
  build_internal(iconFontName, FontSize, IconGlyphRanges, nIconGlyph);
  std::cout << "uiFont init finished!" << std::endl;
  return true;
}

// TODO:
/*
void uiFont::setStyle(uiStyle *style){
    FontSize = style->FontSize;
    Spacing  = style->TextSpacing;
    FontName = style->FontName;
}
*/
void uiFont::setFontSize(const int s) { FontSize = s; }
void uiFont::setFontSpacing(const int s) { Spacing = s; }

void uiFont::setFontName(const std::string& s) { FontName = s; }

void uiFont::setIconFontName(const std::string& s) { iconFontName = s; }
uiGlyph* uiFont::FindGlyph(uiWchar c) {
  if(c >= (size_t)IndexLookup.Size) return FallbackGlyph;
  const uiWchar i = IndexLookup.Data[c];
  if(i == 0xFFFF) {
    return FallbackGlyph;
  }
  return &Glyphs[i];
}

bool uiFont::getSize(uiWchar c, Vec2d* size, unsigned int* dH) {
  auto g     = FindGlyph(c);
  (*size)[0] = g->U1 - g->U0;
  (*size)[0] = g->V1 - g->V0;
  *dH        = g->dHeight;
  return !(g == FallbackGlyph);
}


} // namespace mu::render
