#pragma once

#include <cmath>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>

#include <movutl/core/assert.hpp>

/* #pragma GCC diagnostic push */
/* #pragma GCC diagnostic ignored "-Wsign-compare" // May be used uninitialized 'return {};'. */

namespace mu::core {

#ifdef MU_USE_CONCEPTS
#include <concepts>
template <typename T>
concept RealNumberConcept = requires(T a) {
                              (double)a;
                              (int)a;
                            };
#endif

template <typename T, unsigned int LEN> class _Vec {
public:
  T value[LEN];
  _Vec() { reset(); }
  _Vec(std::initializer_list<T> init) {
    MU_ASSERT(init.size() >= LEN);
    auto idx = 0;
    for(auto i = init.begin(); i < init.end(); i++) {
      value[idx] = *i;
      idx++;
    }
  }

  // template<class... Args>
  // _Vec(Args... args){
  //     constexpr std::size_t size = sizeof...(Args);
  //     static_assert(size == LEN);
  //     std::vector<T> tt{ args... };
  //     for(size_t i=0; i<LEN; i++){value[i] = tt[i];}
  // }
  _Vec(const T x_, const T y_) {
    static_assert(LEN >= 2);
    value[0] = x_;
    value[1] = y_;
  }
  _Vec(const T x_, const T y_, const T z_) {
    static_assert(LEN >= 3);
    value[0] = x_;
    value[1] = y_;
    value[2] = z_;
  }
  _Vec(const T x_, const T y_, const T z_, const T a) {
    static_assert(LEN >= 4);
    value[0] = x_;
    value[1] = y_;
    value[2] = z_;
    value[3] = a;
  }
  _Vec(const T x_, const T y_, const T z_, const T a, const T b) {
    static_assert(LEN >= 5);
    value[0] = x_;
    value[1] = y_;
    value[2] = z_;
    value[3] = a;
    value[4] = b;
  }
  _Vec(const T x_, const T y_, const T z_, const T a, const T b, const T c) {
    static_assert(LEN >= 6);
    value[0] = x_;
    value[1] = y_;
    value[2] = z_;
    value[3] = a;
    value[4] = b;
    value[5] = c;
  }

  inline void reset() {
    for(size_t i = 0; i < LEN; i++) value[i] = 0;
  }

  template <typename U> inline _Vec operator+=(const _Vec<U, LEN>& other) {
    for(size_t i = 0; i < LEN; i++) value[i] += other[i];
    return *this;
  }
  template <typename U> inline _Vec operator-=(const _Vec<U, LEN>& other) {
    for(size_t i = 0; i < LEN; i++) value[i] -= other[i];
    return *this;
  }
  template <typename U> inline _Vec operator+=(const U& other) {
    for(size_t i = 0; i < LEN; i++) value[i] += other;
    return *this;
  }
  template <typename U> inline _Vec operator-=(const U& other) {
    for(size_t i = 0; i < LEN; i++) value[i] -= other;
    return *this;
  }
  template <typename U> inline _Vec operator*=(const U& other) {
    for(size_t i = 0; i < LEN; i++) value[i] *= other;
    return *this;
  }
  template <typename U> inline _Vec operator/=(const U& other) {
    for(size_t i = 0; i < LEN; i++) value[i] /= other;
    return *this;
  }

  template <typename U> inline _Vec operator+(const _Vec<U, LEN>& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] + other[i];
    return t;
  }
  template <typename U> inline _Vec operator-(const _Vec<U, LEN>& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] - other[i];
    return t;
  }
  template <typename U> inline _Vec operator*(const _Vec<U, LEN>& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] * other[i];
    return t;
  }
  template <typename U> inline _Vec operator/(const _Vec<U, LEN>& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] / other[i];
    return t;
  }

#ifdef VKUI_USE_CONCEPTS
  template <RealNumberConcept U> inline _Vec operator+(const U& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) {
      t[i] = value[i] + other;
    }
    return t;
  }
  template <RealNumberConcept U> inline _Vec operator-(const U& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) {
      t[i] = value[i] - other;
    }
    return t;
  }
  template <RealNumberConcept U> inline _Vec operator*(const U& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) {
      t[i] = value[i] * other;
    }
    return t;
  }
  template <RealNumberConcept U> inline _Vec operator/(const U& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) {
      t[i] = value[i] / other;
    }
    return t;
  }
#else
  inline _Vec operator+(const double& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] + other;
    return t;
  }
  inline _Vec operator-(const double& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] - other;
    return t;
  }
  inline _Vec operator*(const double& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] * other;
    return t;
  }
  inline _Vec operator/(const double& other) const {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) t[i] = value[i] / other;
    return t;
  }
#endif

  template <typename U> inline double dot(const _Vec<U, LEN>& other) const {
    double s{0};
    for(size_t i = 0; i < LEN; i++) {
      s += value[i] * other[i];
    }
    return s;
  }
  inline _Vec operator-() {
    _Vec<T, LEN> t;
    for(size_t i = 0; i < LEN; i++) {
      t[i] = -value[i];
    }
    return t;
  }

  template <typename U> bool operator<(const _Vec<U, LEN>& other) const {
    for(size_t i = 0; i < LEN; i++) {
      if(value[i] >= other[i]) return false;
    }
    return true;
  }
  template <typename U> bool operator<=(const _Vec<U, LEN>& other) const {
    for(size_t i = 0; i < LEN; i++) {
      if(value[i] > other[i]) return false;
    }
    return true;
  }
  template <typename U> bool operator>(const _Vec<U, LEN>& other) const {
    for(size_t i = 0; i < LEN; i++) {
      if(value[i] <= other[i]) return false;
    }
    return true;
  }
  template <typename U> bool operator>=(const _Vec<U, LEN>& other) const {
    for(size_t i = 0; i < LEN; i++) {
      if(value[i] < other[i]) return false;
    }
    return true;
  }
  template <typename U> bool operator==(const _Vec<U, LEN>& other) const {
    for(size_t i = 0; i < LEN; i++) {
      if(value[i] != other[i]) return false;
    }
    return true;
  }
  template <typename U> bool operator!=(const _Vec<U, LEN>& other) const {
    for(size_t i = 0; i < LEN; i++) {
      if(value[i] == other[i]) return false;
    }
    return true;
  }
  // template<typename U, int LEN2> inline _Mat outer_product(const _Vec<U, LEN2>& other)     {
  //     _Mat<T, LEN1, LEN2> s; for(size_t i=0; i<LEN; i++){ for(int j=0; j<LEN2; j++){ s(i, j) = value[i] * other[j]; } }
  //     return s;
  // }

  inline const T& operator[](int i) const {
    MU_ASSERT((size_t)i < LEN);
    return value[i];
  }
  inline T& operator[](int i) {
    MU_ASSERT((size_t)i < LEN);
    return value[i];
  }
  inline double norm() const {
    double s = 0;
    for(size_t i = 0; i < LEN; i++) s += value[i] * value[i];
    return sqrtf(s);
  }
  inline double norm_sq() const {
    double s = 0;
    for(size_t i = 0; i < LEN; i++) s += value[i] * value[i];
    return s;
  }
  inline _Vec normalize() const {
    const auto n = norm();
    return n == 0 ? (*this) : (*this) / norm();
  }
  template <typename U> inline _Vec<T, 3> cross(const _Vec<U, 3>& other) const {
    static_assert(LEN == 3);
    _Vec<T, 3> res;
    res[0] = value[1] * other[2] - value[2] * other[1];
    res[1] = value[2] * other[0] - value[0] * other[2];
    res[2] = value[0] * other[1] - value[1] * other[0];
    return res;
  }

  inline void display() {
    std::cout << "value = [";
    for(size_t i = 0; i < LEN; i++) {
      std::cout << value[i] << ", ";
    }
    std::cout << "\n";
  }
  inline _Vec all(const T& v) {
    for(size_t i = 0; i < LEN; i++) value[i] = v;
    return *this;
  }
  inline void setMax() { all(std::numeric_limits<T>::max()); }
  inline void setMin() { all(std::numeric_limits<T>::lowest()); }
  inline void zero() { all(0); }
  inline void ones() { all(1); }
  T min() const {
    T m = value[0];
    for(size_t i = 1; i < LEN; i++) m = std::min(m, value[i]);
    return m;
  }
  T max() const {
    T m = value[0];
    for(size_t i = 1; i < LEN; i++) m = std::max(m, value[i]);
    return m;
  }
  T sum() const {
    T m = value[0];
    for(size_t i = 1; i < LEN; i++) m += m;
    return m;
  }
  double avg() const { return (double)sum() / LEN; }
};


template <typename T, unsigned int LEN> bool operator>(_Vec<T, LEN>& t1, _Vec<T, LEN>& t2) {
  bool r = true;
  for(size_t i = 0; i < LEN; i++) r &= t1[i] > t2[i];
  return r;
}

template <typename T, unsigned int LEN> bool operator<(_Vec<T, LEN>& t1, _Vec<T, LEN>& t2) {
  bool r = true;
  for(size_t i = 0; i < LEN; i++) r &= t1[i] < t2[i];
  return r;
}

template <typename T, unsigned int LEN> std::ostream& operator<<(std::ostream& os, const _Vec<T, LEN>& t) {
  os << " _Vector<" << std::string(typeid(T).name()) << ", " << LEN << "> [ ";
  for(size_t i = 0; i < LEN; i++) {
    os << t[i];
    if(i != LEN - 1) {
      os << ", ";
    }
  }
  os << " ] ";
  return os;
}

using Vec2 = _Vec<double, 2>;
#ifndef USE_CUSTOM_VECTOR3
using Vec3 = _Vec<double, 3>;
#endif
using Vec6 = _Vec<double, 6>;
using Vec4 = _Vec<double, 4>;

using Vec2b = _Vec<unsigned char, 2>;
using Vec3b = _Vec<unsigned char, 3>;
using Vec4b = _Vec<unsigned char, 4>;
using Vec5b = _Vec<unsigned char, 5>;
using Vec6b = _Vec<unsigned char, 6>;

using Vec2s = _Vec<uint16_t, 2>;
using Vec3s = _Vec<uint16_t, 3>;
using Vec4s = _Vec<uint16_t, 4>;
using Vec5s = _Vec<uint16_t, 5>;
using Vec6s = _Vec<uint16_t, 6>;

using Vec2f = _Vec<float, 2>;
using Vec3f = _Vec<float, 3>;
using Vec4f = _Vec<float, 4>;
using Vec5f = _Vec<float, 5>;
using Vec6f = _Vec<float, 6>;

using Vec2d = _Vec<int64_t, 2>;
using Vec3d = _Vec<int64_t, 3>;
using Vec4d = _Vec<int64_t, 4>;
using Vec5d = _Vec<int64_t, 5>;
using Vec6d = _Vec<int64_t, 6>;

using Vec3S16 = _Vec<int16_t, 3>;
using Vec3U16 = _Vec<uint16_t, 3>;
using Vec3S32 = _Vec<int32_t, 3>;
using Vec3U32 = _Vec<uint32_t, 3>;

template <typename T, unsigned int W, unsigned int H> struct _Mat {
  class CommaInput {
  private:
    _Mat* m;
    int index;

  public:
    CommaInput(_Mat* m_, int i) {
      m     = m_;
      index = i;
    }
    CommaInput& operator,(T v) {
      m->value[index] = v;
      index++;
      return *this;
    }
  };

  T value[W * H];
  _Mat() { reset(); }
  template<typename U>
  _Mat(const _Mat<U, W, H>& t) {
    for(size_t i = 0; i < W * H; i++) {
      value[i] = t[i];
    }
  }
  _Mat(const std::initializer_list<T> init) {
    MU_ASSERT(init.size() >= W * H);
    int idx = 0;
    for(auto i = init.begin(); i < init.end(); i++) {
      value[idx] = *i;
      idx++;
    }
  }
  _Mat<T, W, H>& operator=(const _Mat<T, W, H>& o) = default;
  inline void reset() {
    for(size_t i = 0; i < W * H; i++) value[i] = 0;
  }
  template <typename U> inline _Mat operator+=(const _Mat<U, W, H>& other) {
    for(size_t i = 0; i < W * H; i++) value[i] += other[i];
    return *this;
  }
  template <typename U> inline _Mat operator-=(const _Mat<U, W, H>& other) {
    for(size_t i = 0; i < W * H; i++) value[i] -= other[i];
    return *this;
  }
  template <typename U> inline _Mat operator+(const _Mat<U, W, H>& other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] + other[i];
    return t;
  }
  template <typename U> inline _Mat operator-(const _Mat<U, W, H>& other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] - other[i];
    return t;
  }

#ifdef VKUI_USE_CONCEPTS
  template <RealNumberConcept U> inline _Mat operator+=(const U& other) {
    for(size_t i = 0; i < W * H; i++) value[i] += other;
    return *this;
  }
  template <RealNumberConcept U> inline _Mat operator-=(const U& other) {
    for(size_t i = 0; i < W * H; i++) value[i] -= other;
    return *this;
  }
  template <RealNumberConcept U> inline _Mat operator*=(const U& other) {
    for(size_t i = 0; i < W * H; i++) value[i] *= other;
    return *this;
  }
  template <RealNumberConcept U> inline _Mat operator/=(const U& other) {
    for(size_t i = 0; i < W * H; i++) value[i] /= other;
    return *this;
  }
  template <RealNumberConcept U> inline _Mat operator+(const U& other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] + other;
    return t;
  }
  template <RealNumberConcept U> inline _Mat operator-(const U& other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] - other;
    return t;
  }
  template <RealNumberConcept U> inline _Mat operator*(const U& other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] * other;
    return t;
  }
  template <RealNumberConcept U> inline _Mat operator/(const U& other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] / other;
    return t;
  }
#else
  inline _Mat operator+=(const double other) {
    for(size_t i = 0; i < W * H; i++) value[i] += other;
    return *this;
  }
  inline _Mat operator-=(const double other) {
    for(size_t i = 0; i < W * H; i++) value[i] -= other;
    return *this;
  }
  inline _Mat operator*=(const double other) {
    for(size_t i = 0; i < W * H; i++) value[i] *= other;
    return *this;
  }
  inline _Mat operator/=(const double other) {
    for(size_t i = 0; i < W * H; i++) value[i] /= other;
    return *this;
  }
  inline _Mat operator+(const double other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] + other;
    return t;
  }
  inline _Mat operator-(const double other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] - other;
    return t;
  }
  inline _Mat operator*(const double other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] * other;
    return t;
  }
  inline _Mat operator/(const double other) {
    _Mat<T, W, H> t;
    for(size_t i = 0; i < W * H; i++) t[i] = value[i] / other;
    return t;
  }
#endif
  // T operator[](int i)            { MU_ASSERT(i < 3); return (i == 0)? x : (i==1 ? y : z); }
  inline const T& operator[](size_t i) const {
    MU_ASSERT(i < W * H);
    return value[i];
  }
  inline T& operator[](size_t i) {
    MU_ASSERT(i < W * H);
    return value[i];
  }
  inline const T& operator()(size_t x, size_t y) const {
    MU_ASSERT(x * y < W * H);
    return value[y * W + x];
  }
  inline T& operator()(size_t x, size_t y) {
    MU_ASSERT(x * y < W * H);
    return value[y * W + x];
  }
  CommaInput operator<<(size_t v) {
    value[0] = v;
    return CommaInput(this, 1);
  }

  template <typename U> inline bool operator==(const _Mat<U, W, H>& other) const {
    bool s = true;
    for(size_t i = 0; i < W * H; i++) s &= value[i] == other[i];
    return s;
  }
  template <typename U> inline bool operator!=(const _Mat<U, W, H>& other) const { return !(*this == other); }

  inline double norm() const {
    double s = 0;
    for(size_t i = 0; i < W * H; i++) s += value[i] * value[i];
    return s;
  }
  inline void all(T v) {
    for(size_t i = 0; i < W * H; i++) value[i] = v;
  }
  inline void zeros() { all(0); }
  inline void ones() { all(1); }
  inline void identify() {
    for(size_t i = 0; i < W * H; i++) value[i] = (i % (W + 1) == 0) ? 1 : 0;
  }
  inline double trace() {
    double s = 0;
    for(size_t i = 0; i < W * H; i++)
      if(i % (W + 1) == 0) s += value[i];
    return s;
  }
  inline int size() const { return W * H; }
  inline int width() const { return W; }
  inline int height() const { return H; }
  inline T* begin() const { return &value; }
  inline T* end() const { return &value + W * H; }
  [[deprecated]] void transpose() const {
    _Mat<T, H, W> tmp(*this);
    int x, y;
    for(size_t i = 0; i < W * H; i++) {
      x                = i % W;
      y                = i / W;
      value[y * W + x] = tmp[x * H + y];
    }
  }
  template <typename U, unsigned int W2> _Mat operator*(const _Mat<U, W2, W>& other) const {
    _Mat<T, W2, H> out;
    for(size_t x = 0; x < H; x++) {
      for(size_t y = 0; y < W2; y++) {
        T A = 0;
        for(size_t i = 0; i < W; i++) A += other(i, x) * (*this)(y, i);
        out(y, x) = A;
      }
    }
    return out;
  }
  template <typename U> _Vec<T, H> operator*(const _Vec<U, W>& other) const {
    _Vec<T, H> out;
    for(size_t x = 0; x < H; x++) {
      T A = 0;
      for(size_t i = 0; i < W; i++) A += value[x * H + i] * other[i];
      out[x] = A;
    }
    return out;
  }
  _Mat<T, H, W> Trans() const {
    _Mat<T, H, W> out;
    for(size_t x = 0; x < W; x++)
      for(size_t y = 0; y < H; y++) out(y, x) = value[y * W + x];
    return out;
  }

  // T& operator= (const _Mat& other){
  //     if(other.width()*other.height() > W*H){
  //         _grow_capacity(other.width()*other.height());
  //         W = other.width(); H = other.height();
  //     }
  //     std::memcpy(Data, other.begin(), W*H*sizeof(T));
  // }

  float det() const {
    MU_ASSERT(W == H);
    _Mat<T, W, H> tmp = (*this);
    T buf;
    int i, j, k;
    // 三角行列を作成
    for(i = 0; i < W; i++) {
      for(j = 0; j < W; j++) {
        if(i < j) {
          buf = tmp[j * W + i] / tmp[i * W + i];
          for(k = 0; k < W; k++) {
            tmp[j * W + k] -= tmp[i * W + k] * buf;
          }
        }
      }
    }
    // 対角部分の積
    double det = 1.0f;
    for(i = 0; i < W; i++) det *= tmp[i * W + i];
    return det;
  }

  void display() const {
    std::cout << "data = [ " << std::endl;
    for(size_t i = 0; i < W; i++) {
      std::cout << "    [ ";
      for(size_t j = 0; j < H; j++) {
        std::cout << value[j * W + i] << ", ";
      }
      std::cout << " ]" << std::endl;
    }
    std::cout << "]" << std::endl;
  }

  _Mat inv() const {
    MU_ASSERT(W == H);
    _Mat<T, W, W> inverse;
    _Mat<T, W, H> tmp(*this);
    float buf;
    size_t i, j, k;
    for(size_t n = 0; n < W; n++) inverse[n * (W + 1)] = 1.0;
    for(i = 0; i < W; i++) {
      buf = 1.0f / tmp[i * W + i];
      for(j = 0; j < W; j++) {
        tmp[i * W + j] *= buf;
        inverse[i * W + j] *= buf;
      }
      for(j = 0; j < W; j++) {
        if(i != j) {
          buf = tmp[j * W + i];
          for(k = 0; k < W; k++) {
            tmp[j * W + k] -= tmp[i * W + k] * buf;
            inverse[j * W + k] -= inverse[i * W + k] * buf;
          }
        }
      }
    }
    return inverse;
  }
};

template <typename T, unsigned int W, unsigned int H> std::ostream& operator<<(std::ostream& os, const _Mat<T, W, H>& t) {
  os << " _Mat<" << std::string(typeid(T).name()) << ", " << W << "," << H << "> [ ";
  for(size_t i = 0; i < H; i++) {
    for(size_t j = 0; j < W; j++) {
      os << t[i * H + j] << ", ";
    }
    os << "\n";
  }
  os << " ] " << std::endl;
  return os;
}

template <typename T, typename U, unsigned int LEN, unsigned int LEN2> inline _Mat<T, LEN2, LEN> outer_product(const _Vec<T, LEN>& A, const _Vec<U, LEN2>& B) {
  _Mat<T, LEN2, LEN> s;
  for(size_t i = 0; i < LEN; i++) {
    for(size_t j = 0; j < LEN2; j++) {
      s(j, i) = A[i] * B[j];
    }
  }
  return s;
}

using Mat2x2 = _Mat<double, 2, 2>;
using Mat3x3 = _Mat<double, 3, 3>;
using Mat4x4 = _Mat<double, 4, 4>;
using Mat5x5 = _Mat<double, 5, 5>;
using Mat3x6 = _Mat<double, 6, 3>;
using Mat6x6 = _Mat<double, 6, 6>;

using Mat2x2f = _Mat<float, 2, 2>;
using Mat3x3f = _Mat<float, 3, 3>;
using Mat4x4f = _Mat<float, 4, 4>;
using Mat5x5f = _Mat<float, 5, 5>;
using Mat3x6f = _Mat<float, 6, 3>;
using Mat6x6f = _Mat<float, 6, 6>;


template <typename T> struct Vec {
  size_t Size, Capacity;
  T* Data;

  // Provide standard typedefs but we don't use them ourselves.
  typedef T value_type;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;

  // Constructors, destructor
  inline Vec() {
    Size = Capacity = 0;
    Data            = NULL;
  }
  inline Vec(const Vec<T>& src) {
    Size = Capacity = 0;
    Data            = NULL;
    operator=(src);
  }
  inline Vec(const std::initializer_list<T>& src) {
    Data = NULL;
    clear();
    if(src.size() == 0) return;
    resize(src.size());
    MU_ASSERT(Size == src.size());
    int idx = 0;
    for(auto i = src.begin(); i < src.end(); i++) {
      Data[idx] = *i;
      idx++;
    }
  }
  inline Vec<T>& operator=(const Vec<T>& src) {
    clear();
    resize(src.size());
    std::memcpy(Data, src.Data, (size_t)Size * sizeof(T));
    return *this;
  }
  inline ~Vec() { clear(); }
  inline bool empty() const { return Size == 0; }
  inline size_t size() const { return Size; }
  inline int size_in_bytes() const { return Size * (int)sizeof(T); }
  inline size_t capacity() const { return Capacity; }
  inline T& operator[](size_t i) {
    MU_ASSERT(i < Size);
    return Data[i];
  }
  inline const T& operator[](size_t i) const {
    MU_ASSERT(i < Size);
    return Data[i];
  }

  inline void clear() {
    Size = Capacity = 0;
    if(Data) free(Data);
    Data = NULL;
  }
  inline T* begin() { return Data; }
  inline T* data() const { return Data; }
  inline const T* begin() const { return Data; }
  inline T* end() { return Data + Size - 1; }
  inline const T* end() const { return Data + Size - 1; }
  inline T& front() {
    MU_ASSERT(Size > 0);
    return Data[0];
  }
  inline const T& front() const {
    MU_ASSERT(Size > 0);
    return Data[0];
  }
  inline T& back() {
    MU_ASSERT(Size > 0);
    return Data[Size - 1];
  }
  inline const T& back() const {
    MU_ASSERT(Size > 0);
    return Data[Size - 1];
  }
  inline void swap(Vec<T>& rhs) {
    int rhs_size = rhs.Size;
    rhs.Size     = Size;
    Size         = rhs_size;
    int rhs_cap  = rhs.Capacity;
    rhs.Capacity = Capacity;
    Capacity     = rhs_cap;
    T* rhs_data  = rhs.Data;
    rhs.Data     = Data;
    Data         = rhs_data;
  }

  inline int _grow_capacity(int sz) const {
    int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8;
    return new_capacity > sz ? new_capacity : sz;
  }
  inline void resize(size_t new_size) {
    if(new_size == 0) {
      Size = 0;
    } else {
      if(new_size > Capacity) reserve(_grow_capacity(new_size));
      Size = new_size;
    }
  }
  inline void resize(int new_size, const T& v) {
    if(new_size > Capacity) reserve(_grow_capacity(new_size));
    if(new_size > Size)
      for(size_t n = Size; n < new_size; n++) std::memcpy(&Data[n], &v, sizeof(v));
    Size = new_size;
  }
  inline void shrink(int new_size) {
    MU_ASSERT(new_size <= Size);
    Size = new_size;
  } // Resize a vector to a smaller size, guaranteed not to cause a reallocation
  inline void reserve(size_t new_capacity) {
    if(new_capacity <= Capacity) return;
    T* new_data = (T*)malloc(new_capacity * sizeof(T));
    if(Data) {
      std::memcpy((void *)new_data, (void *)Data, Size * sizeof(T));
      free(Data);
    }
    Data     = new_data;
    Capacity = new_capacity;
  }

  // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the Vec data itself! e.g. v.push_back(v[10]) is forbidden.
  inline void push_back(const T& v) {
    if(Size == Capacity) reserve(_grow_capacity(Size + Size / 2 + 100));
    std::memcpy(&Data[Size], &v, sizeof(v));
    Size++;
  }
  inline void push_back(const T&& v) {
    if(Size == Capacity) reserve(_grow_capacity(Size + Size / 2 + 100));
    std::memcpy((void *)&Data[Size], &v, sizeof(v));
    Size++;
  }
  /* inline void push_back_ptr(const T* v) { */
  /*   if(Size == Capacity) reserve(_grow_capacity(Size + Size / 2 + 100)); */
  /*   std::memcpy(&Data[Size], v, sizeof(T)); */
  /*   Size++; */
  /* } */
  inline void push_back(std::initializer_list<T> v) {
    for(auto it = v.begin(); it != v.end(); ++it) {
      push_back(*it);
    }
  }
  /* inline void push_back(std::initializer_list<T> v, int n) { */
  /*   T* it; */
  /*   for(size_t i=0; i < n; i++) { */
  /*     for(it = v.begin(); it != v.end(); ++it) { */
  /*       push_back(*it); */
  /*     } */
  /*   } */
  /* } */
  inline void push_back(const T& v, int n) {
    T* it;
    for(size_t i = 0; i < n; i++) {
      push_back(v);
    }
  }
  inline void pop_back() {
    MU_ASSERT(Size > 0);
    Size--;
  }
  inline void push_front(const T& v) {
    if(Size == 0)
      push_back(v);
    else
      insert(Data, v);
  }
  inline T* erase(const T* it) {
    MU_ASSERT(it >= Data && it < Data + Size);
    const ptrdiff_t off = it - Data;
    memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T));
    Size--;
    return Data + off;
  }
  inline T* erase(const T* it, const T* it_last) {
    MU_ASSERT(it >= Data && it < Data + Size && it_last > it && it_last <= Data + Size);
    const ptrdiff_t count = it_last - it;
    const ptrdiff_t off   = it - Data;
    memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - count) * sizeof(T));
    Size -= (int)count;
    return Data + off;
  }
  inline T* erase_unsorted(const T* it) {
    MU_ASSERT(it >= Data && it < Data + Size);
    const ptrdiff_t off = it - Data;
    if(it < Data + Size - 1) std::memcpy(Data + off, Data + Size - 1, sizeof(T));
    Size--;
    return Data + off;
  }
  inline T* insert(const T* it, const T& v) {
    MU_ASSERT(it >= Data && it <= Data + Size);
    MU_ASSERT(it != nullptr);
    const ptrdiff_t off = it - Data;
    if(Size == Capacity) reserve(_grow_capacity(Size + 1));
    if(off < (int)Size) std::memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T));
    std::memcpy(&Data[off], &v, sizeof(v));
    Size++;
    return Data + off;
  }
  void insert(const T* start, const T* end) {
    MU_ASSERT(start != nullptr && end != nullptr);
    for(const T* x = start; x <= end; x++) push_back(*x);
  }
  inline T* insert(T* it, const T* start, const T* end) {
    MU_ASSERT(start != nullptr && end != nullptr && it != nullptr);
    for(const T* x = start; x <= end; x++) push_back(*x);
    MU_ASSERT(it >= Data && it <= Data + Size);
    for(const T* x = start; x <= end; x++) {
      insert(it, *x);
      it++;
    }
    return Data;
  }
  inline bool contains(const T& v) const {
    const T* data     = Data;
    const T* data_end = Data + Size;
    while(data < data_end)
      if(*data++ == v) return true;
    return false;
  }
  inline T* find(const T& v) {
    T* data           = Data;
    const T* data_end = Data + Size;
    while(data < data_end)
      if(*data == v)
        break;
      else
        ++data;
    return data;
  }
  inline const T* find(const T& v) const {
    const T* data     = Data;
    const T* data_end = Data + Size;
    while(data < data_end)
      if(*data == v)
        break;
      else
        ++data;
    return data;
  }
  inline bool find_erase(const T& v) {
    const T* it = find(v);
    if(it < Data + Size) {
      erase(it);
      return true;
    }
    return false;
  }
  inline bool find_erase_unsorted(const T& v) {
    const T* it = find(v);
    if(it < Data + Size) {
      erase_unsorted(it);
      return true;
    }
    return false;
  }
  inline int index_from_ptr(const T* it) const {
    MU_ASSERT(it >= Data && it < Data + Size);
    const ptrdiff_t off = it - Data;
    return (int)off;
  }
  inline void fill(const T& v) const {
    for(size_t i = 0; i < Size; i++) Data[i] = v;
  }
};

} // namespace mu::core
