#pragma once
#include <movutl/core/rect.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/db/primitives.hpp>

#ifndef MAGGI_MATH_MIN
#define MAGI_NATH_MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAGI_NATH_MAX(x, y) ((x) < (y) ? (y) : (x))
#define MAGI_NATH_CLAMP(x, a, b) ((x) <= (a) ? (a) : ((x) >= (b)) ? b : x)

#define MAGI_MATH_SIN_RAD(x) ((x) - (x) * (x) * (x) / 6)
#define MAGI_MATH_COS_RAD(x) (1 - (x) * (x) / 2 + (x) * (x) * (x) * (x) / 24)
#define MAGI_MATH_TAN_RAD(x, y) ((x) - (x) * (x) * (x) / 3 + (x) * (x) * (x) * (x) * (x)*2 / 15)
#endif


namespace mu::Magi {


using ClusterType = mu::db::Primitive::PrimitiveType;

struct FittingResult {
  core::Vec3 xyz{0.f, 0.f, 0.f};
  core::Vec3 axis{0.f, 0.f, 0.f};
  core::Vec3 size{0.f, 0.f, 0.f};
  std::vector<core::Vec3> polygons;
  core::Rect roi;
  core::Rect3D roi3d;
  float error{0.0f};
  ClusterType type = ClusterType::NotDefined;
  db::Primitive primitive;
  void display() const {
    std::cout << "Fitting result :" << std::endl;
    std::cout << "pos   = " << xyz << std::endl;
    std::cout << "axis  = " << axis << std::endl;
    std::cout << "size  = " << size << std::endl;
    std::cout << "err   = " << error << std::endl;
  }

  FittingResult()  = default;
  ~FittingResult() = default;
};


template <class PointType> class FittingBase {
protected:
  FittingResult result_;

public:
  virtual void reset()    = 0; // すべての値をリセットする
  virtual void estimate() = 0; // 計算を実行する
  /* virtual double get_error(const PointType *points, const size_t cloud_size)=0;  // 他のオブジェクトのEstimatorと比較する場合があるので正規化すること */
  virtual void operator<<(const PointType point) = 0; // 点群データを1つ追加する
  // virtual operator<<(::Magi::Point point){static_assert(true, "Overwrite this virtual function");} // 点群データを1つ追加する
  virtual void setCloud(PointType* cloud, const size_t) = 0;
  const FittingResult& result() const { return result_; }
};

class FittingLinear : public FittingBase<core::Vec2> {
private:
  double y_sum, x_sum, xx_sum, xy_sum;
  double err;

public:
  size_t n;
  core::Vec2 result; // ax**2 + by + c
  FittingLinear() { reset(); }
  void reset() override;
  void estimate() override;
  void setCloud(core::Vec2* p, const size_t _n) override;
  void add(const core::Vec2 p);
  void operator<<(const core::Vec2 p) override;
};

class FittingParabola : public FittingBase<core::Vec2> {
private:
  double y_sum, x_sum, xx_sum, xxx_sum, x4_sum, xxy_sum, xy_sum;
  double err;

public:
  size_t n;
  core::Vec3 result; // ax**2 + by + c
  FittingParabola() { reset(); }
  void reset() override;
  void estimate() override;
  void setCloud(core::Vec2* p, const size_t _n) override;
  void operator<<(const core::Vec2 p) override;
};

class PlaneFitting : public FittingBase<core::Vec3> {
private:
  double A, B, C, D, E, F, G, H;
  size_t n = 0;
  core::Vec2 minMax[3]; // X, Y, Z座標それぞれのMinとMax
public:
  PlaneFitting() { reset(); }
  void reset() override;
  int get_n() { return n; }
  void estimate() override;
  void setCloud(core::Vec3* cloud, const size_t pc_size) override;
  void operator<<(const core::Vec3 p) override;
};

class SphereFitting : public FittingBase<core::Vec3> {
private:
  double mat[4][4];
  double vec[4];
  core::Vec3* cloud;
  int* indices;
  size_t cloud_whole_size;
  bool with_indices;
  float threshould;
  bool estimated;

public:
  double err;
  size_t n;
  core::Vec3 pos;
  double r;
  core::Vec2 minMax[3]; // X, Y, Z座標それぞれのMinとMax
  SphereFitting() {
    reset();
    reset_result();
  }
  void reset() override;
  void reset_result();
  void set_threshould(float th);
  float get_threshould();
  void operator<<(const core::Vec3 p) override;
  void setCloud(core::Vec3* _cloud, int* _indices, const size_t _n);
  void setCloud(core::Vec3* _cloud, const size_t _n) override;
  void CalcAll();
  void estimate() override;
};

class CircleFitting : public FittingBase<core::Vec2> {
private:
  double xx_sum, xy_sum, x_sum, y_sum, yy_sum, xxx_sum, yyy_sum, xxy_sum, xyy_sum;
  double err;

public:
  size_t n;
  CircleFitting() { reset(); }
  void reset() override;
  void estimate() override;
  void operator<<(const core::Vec2 ) override;
  void setCloud(core::Vec2* points, const size_t cloud_size) override;
};

class CylinderFitting : public FittingBase<core::Vec3> {
private:
  double mat[4][4];
  double vec[4];
  size_t n;
  double err;
  core::Vec3 pos, rotation; // rotation = [Qx, Qy, 0]
  core::Vec2 minMax[3];     // X, Y, Z座標それぞれのMinとMax
  core::Vec3* cloud;

public:
  CylinderFitting() { reset(); }
  void operator<<(const core::Vec3) override {}
  void reset() override;
  void setCloud(core::Vec3* points, const size_t cloud_size) override;
  void estimate() override;
};

class CylinderFittingRANSAC : public FittingBase<core::Vec3> {
private:
  //double mat[4][4];
  //double vec[4];
  size_t n;
  double err;
  core::Vec3 pos, rotation; // rotation = [Qx, Qy, 0]
  core::Vec2 minMax[3];     // X, Y, Z座標それぞれのMinとMax
  core::Vec3* cloud;

public:
  CylinderFittingRANSAC() { reset(); }
  void operator<<(const core::Vec3) override {}
  void reset() override;
  void setCloud(core::Vec3* points, const size_t cloud_size) override;
  void estimate() override;
};

class CylinderFittingNormal : public FittingBase<core::Vec3> {
private:
  double mat[4][4];
  double vec[4];
  size_t n;
  double err;
  core::Vec3 pos, rotation; // rotation = [Qx, Qy, 0]
  core::Vec2 minMax[3];     // X, Y, Z座標それぞれのMinとMax
  core::Vec3* cloud;
  core::Vec3* normal;

public:
  CylinderFittingNormal() { reset(); }
  void operator<<(const core::Vec3 p) override;
  void reset() override;
  void setCloud(core::Vec3* points, const size_t cloud_size) override;
  void setCloud(core::Vec3* points, core::Vec3* norm, const size_t cloud_size);
  void estimate() override;
};

class CylinderFitting2 : public FittingBase<core::Vec3> {
private:
  size_t n;
  core::Vec3* cloud;

public:
  CylinderFitting2() { reset(); }
  void operator<<(const core::Vec3 p) override;
  void reset() override;
  void setCloud(core::Vec3* points, size_t cloud_size) override;
  void setCloud(core::Vec3* points, core::Vec3* norm, const size_t cloud_size);
  void estimate() override;
};


class TerrainFitting: public FittingBase<core::Vec3> {
private:
  size_t n;
  core::Vec3* cloud;
  core::Rect3D bbox;
  std::vector<std::vector<core::Vec3>> terrain;
  core::Vec2 minMax[3]; // X, Y, Z座標それぞれのMinとMax
  int get_idx(const core::Vec3 &)const;
public:
  int sp_x, sp_y;

  TerrainFitting() { reset(); }
  void operator<<(const core::Vec3 p) override;
  void reset() override;
  void setCloud(core::Vec3* points, size_t cloud_size) override;
  void setCloud(core::Vec3* points, core::Vec3* norm, const size_t cloud_size);
  void estimate() override;
  void nSplit(int x, int y);
  auto get_terrain()const{return terrain;}
  auto get_bbox()const{return bbox;}
};


} // namespace mu::Magi
