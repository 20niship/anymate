#pragma once

#include <movutl/core/core.hpp>
#include <movutl/core/logger.hpp>
#include <movutl/core/quotanion.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/db/block.hpp>
#include <glm/glm.hpp>

namespace mu::db {
using namespace core;

/// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMove { FORWARD, BACKWARD, LEFT, RIGHT };

enum class CamDistModel : int16_t {
  NONE,                   /**< Rectilinear images. No distortion compensation required. */
  MODIFIED_BROWN_CONRADY, /**< Equivalent to Brown-Conrady distortion, except that tangential distortion is applied to radially distorted points */
  INVERSE_BROWN_CONRADY,  /**< Equivalent to Brown-Conrady distortion, except undistorts image instead of distorting it */
  FTHETA,                 /**< F-Theta fish-eye distortion model */
  BROWN_CONRADY,          /**< Unmodified Brown-Conrady distortion model */
  KANNALA_BRANDT4,        /**< Four parameter Kannala Brandt distortion model */
  COUNT,                  /**< Number of enumeration values. Not a valid input: intended to be used in for-loops. */
  R2ONLY                  // r^2だけのパラメータのDistortion model
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vecs and Matrices for use in OpenGL
class Camera : public Block {
public:
  // camera Attributes
  Vec3 Position;
  Vec3 Front;
  Vec3 Up;
  Vec3 Right;
  Vec3 WorldUp;
  // euler Angles
  float Yaw;
  float Pitch;
  // camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // constructor with vectors
  Camera(Vec3 position = Vec3(0.0f, 0.0f, 0.0f), Vec3 up = Vec3(0.0f, -1.0f, 0.0f), float yaw = -90, float pitch = 0) : Front(Vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp  = up;
    Yaw      = yaw;
    Pitch    = pitch;
    updateCameraVecs();
  }
  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(Vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = Vec3(posX, posY, posZ);
    WorldUp  = Vec3(upX, upY, upZ);
    Yaw      = yaw;
    Pitch    = pitch;
    updateCameraVecs();
  }

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  Mat4x4 GetViewMatrix() { return look_at(Position, Position + Front, Up); }

  // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
  void ProcessKeyboard(CameraMove direction, float deltaTime) {
    const float velocity = MovementSpeed * deltaTime;
    if(direction == CameraMove::FORWARD) Position += Front * velocity;
    if(direction == CameraMove::BACKWARD) Position -= Front * velocity;
    if(direction == CameraMove::LEFT) Position -= Right * velocity;
    if(direction == CameraMove::RIGHT) Position += Right * velocity;
  }

  // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if(constrainPitch) {
      if(Pitch > 89.0f) Pitch = 89.0f;
      if(Pitch < -89.0f) Pitch = -89.0f;
    }

    // update Front, Right and Up Vecs using the updated Euler angles
    updateCameraVecs();
  }

  // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if(Zoom < 1.0f) Zoom = 1.0f;
    if(Zoom > 45.0f) Zoom = 45.0f;
  }

private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVecs() {
    // calculate the new Front vector
    Vec3 front;
    front[0] = std::cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front[1] = std::sin(glm::radians(Pitch));
    front[2] = std::sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front    = front.normalize();
    // also re-calculate the Right and Up vector
    Right = (Front.cross(WorldUp)).normalize(); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = (Right.cross(Front)).normalize();
  }
};


struct CamInfo {
  // 試合中変化しないカメラの特性
  int16_t width, height;
  double ppx, ppy, fx, fy;
  CamDistModel dist_model;
  double coeffs[5];
  Vec3 offset;   // 機体の重心（libflierから得られる位置からみたOffset
  Vec3 rotation; // pitch roll yaw (弧度法）
  Mat3x3 R;

  // ロボットのオフセットと回転（Z軸）
  Vec3 robot_offset{0, 0, 0};
  double robot_rotation{0};

  bool connected;
  CamInfo() {
    width     = -1;
    height    = -1;
    for(int i = 0; i < 5; i++) coeffs[i] = 0;
    offset    = {0, 0};
    rotation  = {0, 0, 0};
    ppx = ppy = 0;
    fx = fy   = 0;
    connected = false;
  }
  inline unsigned int size() { return width * height; }
  inline bool enabled() { return width > 0 && height > 0; }
  void print() const;
  void setRotationMat();

  inline Vec3 get_cam_pos(const double depth, const double u, const double v) const {
    const double x = (u - ppx) / fx;
    const double y = (v - ppy) / fy;
    return {depth * x, depth * y, depth};
  }

  inline Vec3 get_cam_pos_acc(const double depth, const double u, const double v) const {
    if(dist_model == CamDistModel::NONE) {
      const double x = (u - ppx) / fx;
      const double y = (v - ppy) / fy;
      return {depth * x, depth * y, depth};
    } else if(dist_model == CamDistModel::R2ONLY) {
      const double r2 = (u - ppx) * (u - ppx) + (v - ppy) * (v - ppy);
      const double u2 = u - coeffs[0] * r2;
      const double v2 = v - coeffs[0] * r2;
      const double x  = (u2 - ppx) / fx;
      const double y  = (v2 - ppy) / fy;
      return {depth * x, depth * y, depth};
    } else if(dist_model == CamDistModel::INVERSE_BROWN_CONRADY) {
      double x        = (u - ppx) / fx;
      double y        = (v - ppy) / fy;
      const double xo = x;
      const double yo = y;
      // need to loop until convergence
      // 10 iterations determined empirically
      for(int i = 0; i < 10; i++) {
        const double r2      = x * x + y * y;
        const double icdist  = (float)1 / (float)(1 + ((coeffs[4] * r2 + coeffs[1]) * r2 + coeffs[0]) * r2);
        const double xq      = x / icdist;
        const double yq      = y / icdist;
        const double delta_x = 2 * coeffs[2] * xq * yq + coeffs[3] * (r2 + 2 * xq * xq);
        const double delta_y = 2 * coeffs[3] * xq * yq + coeffs[2] * (r2 + 2 * yq * yq);
        x                    = (xo - delta_x) * icdist;
        y                    = (yo - delta_y) * icdist;
      }
      return {depth * x, depth * y, depth};
    } else {
      LOGE << "[CamInfo::get_global_pos_acc] Not defined Model!";
      assert("Not defined model! ");
      return {0, 0, 0};
    }
  }

  inline Vec3 get_robot_offset_global() const { return {offset[0] * std::cos(robot_rotation) - offset[1] * std::sin(robot_rotation), offset[1] * std::sin(robot_rotation) + offset[1] * std::cos(robot_rotation), offset[2]}; }

  inline Vec3 get_global_pos(const double depth, const double u, const double v) const {
    const auto robot_offset_r = get_robot_offset_global();
    return R * get_cam_pos(depth, u, v) + robot_offset + robot_offset_r;
  }

  inline Vec3 get_global_pos_acc(const double depth, const double u, const double v) const {
    const auto robot_offset_r = get_robot_offset_global();
    return R * get_cam_pos_acc(depth, u, v) + robot_offset + robot_offset_r;
  }
};

template <typename T = Vec3> Vec2 project_point_to_pixel(const T point, const CamInfo cam);
template <typename T = Vec3> T deproject_pixel_to_point(const Vec2& pixel, const CamInfo& cam, const double depth);


/* cv::Mat CreateHistgram(cv::Mat& channels); */
/* void showImgDiff(const cv::Mat& depth, const cv::Mat& rgb, const std::string& name = "diff_"); */

template <typename ColorType> std::vector<ColorType> get_default_color_map();

Mat3x3 get_rotation_mat_from_vectors(const Vec3&, const Vec3&);

// ロドリゲスの回転公式より回転行列を求める
Mat3x3 rodrigues_rot(const Vec3& p, const Vec3& n0, const Vec3& n1);



struct CameraPosition {
  Vec3 pos;
  Vec3 dir;
  Vec3 u;
  double scale;

  double fov, zNear, zFar, aspect;
  // 画角、Z座標のクリッピング、アスペクト比

  CameraPosition() {
    pos   = {0, 0, 0};
    dir   = {1, 1, 1};
    u     = {0, 0, 1};
    scale = 7;

    fov    = 60.0f * 3.1415 / 180.0;
    zNear  = 1.0;
    zFar   = 600.0;
    aspect = 1.0;
  }

  void move(double n) { pos += dir * n; } // 視点は動かさず、カメラ座標だけ移動する
  void lookAt(float* matrix) const { lookAt(pos[0], pos[1], pos[2], dir[0], dir[1], dir[2], u[0], u[1], u[2], matrix); }

  void go_closer(double delta) {
    const auto start = pos + dir;
    pos              = start - dir * (1.0 + 0.1 * delta);
    dir              = start - pos;
  }

  void lookAt(float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz, float* matrix) const {
    float l;
    tx         = ex - tx;
    ty         = ey - ty;
    tz         = ez - tz;
    l          = sqrtf(tx * tx + ty * ty + tz * tz); // TODO: L = 4のときのエラー処理
    matrix[2]  = tx / l;
    matrix[6]  = ty / l;
    matrix[10] = tz / l;

    tx        = uy * matrix[10] - uz * matrix[6];
    ty        = uz * matrix[2] - ux * matrix[10];
    tz        = ux * matrix[6] - uy * matrix[2];
    l         = sqrtf(tx * tx + ty * ty + tz * tz);
    matrix[0] = tx / l;
    matrix[4] = ty / l;
    matrix[8] = tz / l;

    matrix[1] = matrix[6] * matrix[8] - matrix[10] * matrix[4];
    matrix[5] = matrix[10] * matrix[0] - matrix[2] * matrix[8];
    matrix[9] = matrix[2] * matrix[4] - matrix[6] * matrix[0];

    matrix[12] = -(ex * matrix[0] + ey * matrix[4] + ez * matrix[8]);
    matrix[13] = -(ex * matrix[1] + ey * matrix[5] + ez * matrix[9]);
    matrix[14] = -(ex * matrix[2] + ey * matrix[6] + ez * matrix[10]);

    matrix[3] = matrix[7] = matrix[11] = 0.0f;

    for(int i = 0; i < 3; i++) {
      matrix[i] *= scale;
      matrix[i + 4] *= scale;
      matrix[i + 8] *= scale;
    }

    matrix[2]  = -matrix[2];
    matrix[6]  = -matrix[6];
    matrix[10] = -matrix[10];
    matrix[14] = -matrix[14];

    // matrix[8] = -matrix[8];
    // matrix[9] = -matrix[9];
    // matrix[10] = -matrix[10];
    // matrix[11] = -matrix[11];
    matrix[15] = 1.0f; // 1.0f / scale; //1.0f;
  }

  // 透視投影変換行列を求める
  void perspectiveMatrix(float left, float right, float bottom, float top, float znear, float zfar, float* matrix) {
    float dx = right - left; // TODO: エラー処理（dx != 0)
    float dy = top - bottom;
    float dz = zfar - znear;
    assert(dx != 0);
    assert(dy != 0);
    assert(dz != 0);
    matrix[0]  = 2.0f * znear / dx;
    matrix[5]  = 2.0f * znear / dy;
    matrix[8]  = (right + left) / dx;
    matrix[9]  = (top + bottom) / dy;
    matrix[10] = -(zfar + znear) / dz;
    matrix[11] = -1.0f;
    matrix[14] = -2.0f * zfar * znear / dz;
    matrix[1] = matrix[2] = matrix[3] = matrix[4] = matrix[6] = matrix[7] = matrix[12] = matrix[13] = matrix[15] = 0.0f;
  }

  // 平行投影変換行列を求める
  void orthogonalMatrix(float left, float right, float bottom, float top, float znear, float zfar, float* matrix) {
    float dx = right - left;
    float dy = top - bottom;
    float dz = zfar - znear;
    assert(dx != 0);
    assert(dy != 0);
    assert(dz != 0);

    matrix[0]  = 2.0f / dx;
    matrix[5]  = 2.0f / dy;
    matrix[10] = -2.0f / dz;
    matrix[12] = -(right + left) / dx;
    matrix[13] = -(top + bottom) / dy;
    matrix[14] = -(zfar + znear) / dz;
    matrix[15] = 1.0f;
    matrix[1] = matrix[2] = matrix[3] = matrix[4] = matrix[6] = matrix[7] = matrix[8] = matrix[9] = matrix[11] = 0.0f;
  }

  Mat4x4 lookAtRH(Vec3 const& eye, Vec3 const& center, Vec3 const& up) {
    const auto f((center - eye).normalize());
    const auto s((f.cross(up).normalize()));
    const auto u(s.cross(f));
    Mat4x4 result;
    result.all(1.0);
    result(0, 0) = s[0];
    result(0, 1) = s[1];
    result(0, 2) = s[2];
    result(1, 0) = u[0];
    result(1, 1) = u[1];
    result(1, 2) = u[2];
    result(2, 0) = -f[0];
    result(2, 1) = -f[1];
    result(2, 2) = -f[2];
    result(0, 3) = -(s[0] * eye[0] + s[1] * eye[1] + s[2] * eye[2]);
    result(1, 3) = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
    result(2, 3) = (f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2]);
    return result;
  }

  Mat4x4f perspective(double fovy, double aspect, double znear, double zfar) const {
    assert(std::abs(aspect - std::numeric_limits<double>::epsilon()) > 0);
    const auto tanHalfFovy = std::tan(fovy / 2.0f);
    Mat4x4f result;
    result.all(0);
    result(0, 0) = 1.0f / (aspect * tanHalfFovy);
    result(1, 1) = 1.0f / (tanHalfFovy);
    result(2, 2) = zfar / (zfar - znear);
    result(3, 2) = 1.0f;
    result(2, 3) = -(zfar * znear) / (zfar - znear);
    return result;
  }

  Mat4x4f getCameraProjMatrix() const {
    _Mat<float, 4, 4> view;
    lookAt(pos[0], pos[1], pos[2], dir[0], dir[1], dir[2], u[0], u[1], u[2], view.value);
    const auto proj = perspective(fov, aspect, zNear, zFar);
    return proj * view;
  }

  Vec2 project(const Vec3& pos) const {
    const Vec4 pos4(pos[0], pos[1], pos[2], 1.0);
    const auto m  = getCameraProjMatrix().Trans();
    const Vec4f p = m * pos4;
    return Vec2(p[0], p[1]) / p[3];
  }

  Vec3 deproject(const Vec2& pos) const {
    const Vec4 pos4(pos[0], pos[1], 0, 1.0);
    const auto m      = getCameraProjMatrix();
    const auto deproj = m.inv() * pos4;
    return Vec3(deproj[0], deproj[1], deproj[2]);
  }
};

} // namespace mu::db
