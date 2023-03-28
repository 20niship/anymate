#pragma once
#include <movutl/core/core.hpp>
#include <movutl/core/vector.hpp>

namespace mu::core {
inline double RGB2H(const Vec3b& col) {
  double Hue     = 0; //色相
  const double R = col[0];
  const double G = col[1];
  const double B = col[2];
  double MAX     = max(R, G, B);
  double MIN     = min(R, G, B);

  if(MAX == MIN) {
    Hue = 0;
  } else {
    if(MAX == R)
      Hue = 60.0 * (G - B) / (MAX - MIN) + 0;
    else if(MAX == G)
      Hue = 60.0 * (B - R) / (MAX - MIN) + 120.0;
    else if(MAX == B)
      Hue = 60.0 * (R - G) / (MAX - MIN) + 240.0;
    else
      Hue = 0;
  }
  return Hue;
}

inline double RGB2S(const Vec3b& col) {
  double Saturation; //彩度
  const double R = col[0];
  const double G = col[1];
  const double B = col[2];
  double MAX     = std::max((std::max(R, G)), B);
  double MIN     = std::min((std::min(R, G)), B);

  if(MAX == MIN) {
    Saturation = 0;
  } else {
    Saturation = (MAX - MIN) / MAX * 100.0;
  }
  return Saturation;
}

inline double RGB2V(const Vec3b& col) {
  const double R   = col[0];
  const double G   = col[1];
  const double B   = col[2];
  const double MAX = max(R, G, B);
  /* double MIN     = min(R, G, B); */
  const double Value = MAX / 256 * 100;
  return Value;
}

template <typename T = double> inline _Vec<T, 3> RGB2HSV(const Vec3b& col) {
  const double R = col[0];
  const double G = col[1];
  const double B = col[2];
  double MAX     = std::max((std::max(R, G)), B);
  double MIN     = std::min((std::min(R, G)), B);
  double Value, Saturation, Hue;

  Value = MAX / 256 * 100;
  if(MAX == MIN) {
    Hue        = 0;
    Saturation = 0;
  } else {
    if(MAX == R)
      Hue = 60.0 * (G - B) / (MAX - MIN) + 0;
    else if(MAX == G)
      Hue = 60.0 * (B - R) / (MAX - MIN) + 120.0;
    else
      Hue = 60.0 * (R - G) / (MAX - MIN) + 240.0;
    if(Hue > 360.0)
      Hue = Hue - 360.0;
    else if(Hue < 0)
      Hue = Hue + 360.0;
    Saturation = (MAX - MIN) / MAX * 100.0;
  }
  return {Hue, Saturation, Value};
}

template <typename T> inline Vec3b HSVtoRGB(const _Vec<T, 3>& hsv) {
  const double H = hsv[0];
  const double S = hsv[1];
  const double V = hsv[2];
  if(H > 360 || H < 0 || S > 100 || S < 0 || V > 100 || V < 0) {
    std::cout << "The givem HSV values are not in valid range" << std::endl;
    return {0, 0, 0};
  }
  float s = S / 100;
  float v = V / 100;
  float C = s * v;
  float X = C * (1 - std::abs(fmod(H / 60.0, 2) - 1));
  float m = v - C;
  float r, g, b;
  if(H >= 0 && H < 60) {
    r = C, g = X, b = 0;
  } else if(H >= 60 && H < 120) {
    r = X, g = C, b = 0;
  } else if(H >= 120 && H < 180) {
    r = 0, g = C, b = X;
  } else if(H >= 180 && H < 240) {
    r = 0, g = X, b = C;
  } else if(H >= 240 && H < 300) {
    r = X, g = 0, b = C;
  } else {
    r = C, g = 0, b = X;
  }
  uint8_t R = (r + m) * 255;
  uint8_t G = (g + m) * 255;
  uint8_t B = (b + m) * 255;
  return {R, G, B};
}


} // namespace mu::core
