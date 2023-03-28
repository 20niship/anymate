#pragma once

#include <cassert>
#include <iostream>
#include <librealsense2/rs.hpp>
#include <mutex>
#include <sstream>
#include <strstream>
#include <vector>

#include <movutl/core/core.hpp>
#include <movutl/db/camera.hpp>

#define DEVICE_ASSERT(X) assert(X)


namespace mu::Device {
using RSDepthType = unsigned short;

class RotationEstimator;

enum class ImageAlignment { none, to_depth, to_color };

class RealSense {
protected:
  bool initialized;
  rs2::frameset frameset_data;
  rs2::depth_frame* depth_img_ptr;
  rs2::frame* rgb_image_ptr;
  // rs2::motion_frame accel_frame, gryo_frame; // デフォルトのコンス楽たがないらしい

  double fx, fy;
  int8_t selected_device_id;
  std::vector<rs2::device> devices;

  rs2::pipeline pipe;
  rs2::config config;
  rs2::pipeline_profile pipeline_profile;
  ImageAlignment align;
  rs2::align align_to_color, align_to_depth;
  db::CamInfo depth_cam_info, rgb_cam_info;
  // settings
  bool enable_depth, enable_rgb, enable_gyro, enable_accel, enable_pose;

public:
  // settings
  bool init(std::string config_filename);
  // void set_default_device(uint id = 0); //出力したデバイス一覧のうち、IDのデバイスを以降使用する

  // print information
  void print_devices(); // 現在のデバイス一覧とその詳細を出力する
  void print_intrinsics();
  void set_alignment(const ImageAlignment a);
  void update_intrinsics();

  inline void next_frame() {                // 次のフレームを取得するように指示を出す
    frameset_data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
    switch(align) {
      case ImageAlignment::to_color: frameset_data = align_to_color.process(frameset_data); break;
      case ImageAlignment::to_depth: frameset_data = align_to_depth.process(frameset_data); break;
      default: break;
    }
  }
  inline void* get_depth_frame() {
    auto depth_img = frameset_data.get_depth_frame();
    depth_img_ptr  = &depth_img;
    return (void*)depth_img.get_data();
  }
  inline void* get_rgb_frame() {
    auto rgb_image = frameset_data.get_color_frame();
    rgb_image_ptr  = &rgb_image;
    return (void*)rgb_image.get_data();
  }
  inline core::Vec3 get_accel() {
    const rs2::motion_frame accel_frame = frameset_data.first_or_default(RS2_STREAM_ACCEL);
    const rs2_vector accel_data         = accel_frame.get_motion_data();
    return {accel_data.x, accel_data.y, accel_data.z};
  }
  inline core::Vec3 get_gyro() {
    const rs2::motion_frame gyro_frame = frameset_data.first_or_default(RS2_STREAM_ACCEL);
    const rs2_vector gyro_data         = gyro_frame.get_motion_data();
    return {gyro_data.x, gyro_data.y, gyro_data.z};
  }

  inline rs2_pose get_pose() {
    auto f = frameset_data.first_or_default(RS2_STREAM_POSE);
    return f.as<rs2::pose_frame>().get_pose_data();
  }


  // inline std::list<rs2_pose, rs2_pose> get_motion()[
  //     // TODO
  //     auto motion = frameset_data.as<rs2::motion_frame>();
  //     // If casting succeeded and the arrived frame is from gyro stream
  //     if (motion && motion.get_profile().stream_type() == RS2_STREAM_GYRO && motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
  //     {
  //         // Get the timestamp of the current frame
  //         double ts = motion.get_timestamp();
  //         // Get gyro measures
  //         rs2_vector gyro_data = motion.get_motion_data();
  //         // Call function that computes the angle of motion based on the retrieved measures
  //         algo.process_gyro(gyro_data, ts);
  //     }
  //     // If casting succeeded and the arrived frame is from accelerometer stream
  //     if (motion && motion.get_profile().stream_type() == RS2_STREAM_ACCEL && motion.get_profile().format() == RS2_FORMAT_MOTION_XYZ32F)
  //     {
  //         // Get accelerometer measures
  //         rs2_vector accel_data = motion.get_motion_data();
  //         // Call function that computes the angle of motion based on the retrieved measures
  //         algo.process_accel(accel_data);
  //     }
  // ]

  // gettter, setter
  inline bool get_is_initialized() { return initialized; }
  inline int get_device_num() { return devices.size(); }
  // inline int  get_sensors_num     () { return sensors.size(); }
  inline bool get_depth_enabled() { return enable_depth; }
  inline bool get_rgb_enabled() { return enable_rgb; }
  inline bool get_gyro_enabled() { return enable_gyro; }
  inline bool get_accel_enabled() { return enable_accel; }
  inline bool get_pose_enabled() { return enable_pose; }
  inline uint get_depth_width() { return depth_cam_info.width; }
  inline uint get_depth_height() { return depth_cam_info.height; }
  inline uint get_rgb_width() { return rgb_cam_info.width; }
  inline uint get_rgb_height() { return rgb_cam_info.height; }
  // inline uint get_rgb_fps         () { return rgb_cam_info.fps;        }
  inline auto get_alignment() { return align; }
  db::CamInfo get_depth_cam_info() { return depth_cam_info; }
  db::CamInfo get_rgb_cam_info() { return rgb_cam_info; }
  inline double get_depth_scale() {
    // TODO
    // return pipeline_profile->get_depth_scale() * 1000.0;
    return 0;
  }

  RealSense();
  // ~RealSense();
  RealSense(const RealSense&)            = delete;
  RealSense(const RealSense&&)           = delete;
  RealSense& operator=(const RealSense&) = delete;
  RealSense& operator=(RealSense&&)      = delete;
};



// TODO: Deviceを抽象クラスで定義、カメラとかRealsenseとかはそっからの派生で
//
#if 0
 template <class DeviceElem, class SensorElem, class Pipeline>
 class Device{
 protected:
     bool initialized;
     void * depth_image;
     double fx, fy;
     uint8_t selected_device_id;
     std::vector<DeviceElem> devices;
     std::vector<SensorElem> sensors;
     Pipeline pipe;

 public:
     // settings
     virtual bool init() = 0; // initialize sensor and devices
     virtual void set_option_config_file(std::string config_file_name) = 0;
     virtual void set_option() = 0;
     virtual void set_default_device(uint id = 0); //出力したデバイス一覧のうち、IDのデバイスを以降使用する

     // print information
     inline void print_devices (){ // 現在のデバイス一覧とその詳細を出力する
         std::cout << devices.size() << " device(s)" << std::endl;
         for(auto device : devices){ print_device(device);}
     }
     virtual void print_device  (const DeviceElem &dev) = 0; // 現在のデバイス一覧とその詳細を出力する
     inline  void print_device  (int index) { DEVICE_ASSERT(index < devices.size()); print_device(devices[index]); }
     inline  void print_device  () { DEVICE_ASSERT(devices.size() > 0); print_device(0); }

     inline void print_sensors (){
         std::cout << sensors.size() << " sensors(s)" << std::endl;
         for(auto sensor : sensors){ print_sensor(sensor);}
     }
     virtual void print_config_options() = 0; //print all configuration of device

     //
     virtual void  next_frame() = 0; // 次のフレームを取得するように指示を出す
     virtual void *get_depth_frame() = 0;
     virtual void *get_rgb_frame() = 0;

     // gettter, setter
     inline bool check_is_initialized() { return initialized;    }
     inline int  get_device_num()       { return devices.size(); }
     inline int  get_sensors_num()      { return sensors.size(); }

     Device(const Device&) = delete;
     Device(const Device&&) = delete;
     Device& operator=(const Device&) = delete;
     Device& operator=(Device&&) = delete;
 };

 using RSDepthType = unsigned short;
 // class RealSense : public Device<rs2::device, rs2::sensor, rs2::pipeline>{
 class RealSense : public Device<rs2::device, rs2::sensor, rs2::pipeline>{
     bool init();
     void print_devices ();
     void print_device  (int index);
     void print_device  (const rs2::device &dev);
     void print_sensors ();
     void print_sensors (int index);
     void print_sensors (const rs2::device &dev);
     void print_config  ();
     void print_config_options();

     void  next_frame();
     void *get_depth_frame();
     void *get_rgb_frame();
 };
 class Zed : public Device<rs2::device, rs2::sensor, void *>{};
#endif

} // namespace mu::Device

