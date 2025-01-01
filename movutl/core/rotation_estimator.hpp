#pragma once 

#include <movutl/core/core.hpp>
#include <cassert>
#include <iostream>
#include <librealsense2/rs.hpp>
#include <mutex>
#include <sstream>
#include <strstream>
#include <vector>

#ifndef PI
#define PI 3.14159265358979323846
#define PI_FL 3.141592f
#endif


namespace mu::core{

class RotationEstimator {
  // theta is the angle of camera rotation in x, y and z components
  core::Vec3f theta;
  std::mutex theta_mtx;
  /* alpha indicates the part that gyro and accelerometer take in computation of theta; higher alpha gives more weight to gyro, but too high
  values cause drift; lower alpha gives more weight to accelerometer, which is more sensitive to disturbances */
  float alpha     = 0.98f;
  bool firstGyro  = true;
  bool firstAccel = true;
  // Keeps the arrival time of previous gyro frame
  double last_ts_gyro = 0;

public:
  // Function to calculate the change in angle of motion based on data from gyro
  void process_gyro(rs2_vector gyro_data, double ts) {
    if(firstGyro) // On the first iteration, use only data from accelerometer to set the camera's initial position
    {
      firstGyro    = false;
      last_ts_gyro = ts;
      return;
    }
    // Holds the change in angle, as calculated from gyro
    core::Vec3f gyro_angle;

    // Initialize gyro_angle with data from gyro
    gyro_angle[0] = gyro_data.x; // Pitch
    gyro_angle[1] = gyro_data.y; // Yaw
    gyro_angle[2] = gyro_data.z; // Roll

    // Compute the difference between arrival times of previous and current gyro frames
    double dt_gyro = (ts - last_ts_gyro) / 1000.0;
    last_ts_gyro   = ts;

    // Change in angle equals gyro measures * time passed since last measurement
    gyro_angle = gyro_angle * static_cast<float>(dt_gyro);

    // Apply the calculated change of angle to the current angle (theta)
    std::lock_guard<std::mutex> lock(theta_mtx);
    theta += core::Vec3f(-gyro_angle[2], -gyro_angle[1], gyro_angle[0]);
  }

  void process_accel(rs2_vector accel_data) {
    // Holds the angle as calculated from accelerometer data
    core::Vec3f accel_angle;

    // Calculate rotation angle from accelerometer data
    accel_angle[2] = atan2(accel_data.y, accel_data.z);
    accel_angle[0] = atan2(accel_data.x, sqrt(accel_data.y * accel_data.y + accel_data.z * accel_data.z));

    // If it is the first iteration, set initial pose of camera according to accelerometer data (note the different handling for Y axis)
    std::lock_guard<std::mutex> lock(theta_mtx);
    if(firstAccel) {
      firstAccel = false;
      theta      = accel_angle;
      // Since we can't infer the angle around Y axis using accelerometer data, we'll use PI as a convetion for the initial pose
      theta[1] = PI_FL;
    } else {
      /*
      Apply Complementary Filter:
          - high-pass filter = theta * alpha:  allows short-duration signals to pass through while filtering out signals
            that are steady over time, is used to cancel out drift.
          - low-pass filter = accel * (1- alpha): lets through long term changes, filtering out short term fluctuations
      */
      theta[0] = theta[0] * alpha + accel_angle[0] * (1 - alpha);
      theta[2] = theta[2] * alpha + accel_angle[2] * (1 - alpha);
    }
  }

  // Returns the current rotation angle
  core::Vec3f get_theta(){
    std::lock_guard<std::mutex> lock(theta_mtx);
    return theta;
  }
};

}
