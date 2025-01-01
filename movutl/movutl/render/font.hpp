#pragma once
#include <ft2build.h>
#include <freetype/ftsynth.h>
#include <iostream>

#include <movutl/core/vector.hpp>
/* #include FT_FREETYPE_H */

namespace mu::render {

using namespace mu::core;

// 1つの文字（グリフ）に対するデータ
struct uiGlyph {
  // unsigned int    Codepoint : 31;     // 0x0000..0xFFFF
  // unsigned int    Visible : 1;        // Flag to allow early out when rendering
  unsigned int U0, V0, U1, V1; // UVテクスチャの座標
  int dHeight;                 // グリフの最上点から基準ラインまでの高さ
};


enum class FontLanguage {
  Japansese,
  English,
  Chinese,
  Korean,
  Thai,
  Vietnamese,
  // JPN,ENG,CHI,
};


using uiWchar = unsigned short;

class uiFont {
  // private:
public:
  FT_Library library;
  FT_Face face;
  FT_GlyphSlot slot; // グリフへのショートカット

  // uiFontFlags    flags;
  unsigned short desiredTextSize;

  float Spacing;                   // 字間、config()関数内でuiStyle->FontSpacingの値が設定される
  float FontSize;                  // フォントサイズ、config()関数内でuiStyle->FontSpacingの値が設定される　
  unsigned int TexWidth;           // build()関数で作成される文字テクスチャのサイズ＜GL_MAX_TEXTURE_SIZE.
  unsigned int TexHeight;          // build()関数で作成される文字テクスチャのサイズ＜GL_MAX_TEXTURE_SIZE.
  unsigned int TexHeight_capacity; // 予約済み　のテクスちゃ老域の全体高さ
  bool isBuildFinished;            // フォントの作成が終わったかどうか
  FontLanguage language;
  uiWchar *GlyphRanges, *IconGlyphRanges;
  unsigned int nGlyph, nIconGlyph; // GlyphRangesの配列の数（つまりグリフの数）
  std::string FontName;
  std::string iconFontName;

  // bool              MouseCursorIcons;   //
  Vec2d TexUvWhitePixel;    // Texture coordinates to a white pixel
  Vec<uiWchar> IndexLookup; // 12-16 // out //            // Sparse. Index glyphs by Unicode code-point.
  Vec<uiGlyph> Glyphs;      // すべての文字に対するグリフ
  uiGlyph* FallbackGlyph;   // FinGlyphで上手くいかなかったときのGlyph（□の文字化けのやつ）

  unsigned char* _Data; // テクスチャデータ
                        // float Scale;
  void AddGlyph(uiWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
  void AddRemapChar(uiWchar dst, uiWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.
  void SetGlyphVisible(uiWchar c, bool visible);
  bool IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last);

  uiWchar* GetGlyphRangesKorean();
  uiWchar* GetGlyphRangesJapanese();
  uiWchar* GetGlyphRangesEnglish();
  uiWchar* GetGlyphRangesChinese();
  uiWchar* GetGlyphRangesCyrillic();
  uiWchar* GetGlyphRangesThai();
  uiWchar* GetGlyphRangesVietnamese();
  uiWchar* getGlyphRangeIcon();

public:
  uiFont();
  ~uiFont();
  void init();
  bool setLanguage(FontLanguage l);
  /* void setStyle(uiStyle* style); */
  bool build();                  //フォンﾄトをレンダリングする
  uiGlyph* FindGlyph(uiWchar c); //
  bool getSize(uiWchar c, Vec2d* size, unsigned int* dH);
  Vec2f CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const; // utf8
  char* CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;

  inline unsigned int getTexWidth() { return TexWidth; }
  inline unsigned int getTexHeight() { return TexHeight; }
  inline unsigned char* getData() { return _Data; }
  inline unsigned int getGlyphNum() { return Glyphs.size(); }
  void setFontSize(const int s);
  void setFontSpacing(const int s);
  void setFontName(const std::string& s);
  void setIconFontName(const std::string& s);

  bool build_internal(const std::string& fontname, const int fontsize, const uiWchar* glyphRange, const size_t n);
};

} // namespace mu::render
