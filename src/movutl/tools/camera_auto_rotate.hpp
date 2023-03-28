#pragma once 
#include <movutl/db/camera.hpp>

namespace mu::tools{

class AutoCameraRotate{
  private:
    core::CameraPosition *cam;
    core::Vec3 axis;
    float speed;
    size_t frames;

    AutoCameraRotate() {
      cam = nullptr;
      axis = {0,0,1};
      speed = 0.01;
      frames = 0;
    }

    void set_cam(const core::CameraPosition *c){cam = c;}
    void set_axis(const Vec3 &a){axis= a;}
    void set_speed(float s){speed = s; }
    void update(){
      frames++;
      cam->


    }
};

}

