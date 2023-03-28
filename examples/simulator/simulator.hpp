#pragma once

#if 0
#include <ode/ode.h>
#include <ode/odemath.h>
#include <ode/collision.h>
#include <ode/objects.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include <drawstuff/drawstuff.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <functional>
#include <vector>
#include <map>
// #include <numbers>
#include <cmath>

#include "cpptoml.hpp"
#include "spline.hpp"

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

#ifdef dDOUBLE
#define dsDrawCapsule dsDrawCapsuleD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawBox     dsDrawBoxD
#define dsDrawLine    dsDrawLineD
#define dsDrawSphere  dsDrawSphereD
#endif

#define THREE_WHEEL
#define SHOW_SIMULATOR_LOG

typedef std::function<bool (double)> CallbackType;

namespace Simulator{

struct CameraPose{
    float px, py, pz;
    float pitch, roll, yaw;
    float ppx, ppy;
    int width, height;
    float mat[9];
    CameraPose(){
        px = 0;
        py = 0;
        pz = 0;
        pitch = 0;
        roll = 0;
        yaw = 0;
        ppx = 0;
        ppy = 0;
        for(int i=0;i<9;i++){mat[i] = 0;}
    }

    CameraPose(float x_, float y_, float z_, float pitch_, float roll_, float yaw_, int w, int h){
        px = x_;
        py = y_;
        pz = z_;
        pitch = pitch_;
        roll = roll_;
        yaw = yaw_;
        width  = w;
        height = h;
    }

    void print(){
        std::cout << "---------  CameraPose  ---------" << std::endl << 
        "(px,    py,   pz)  = (" << px << ", " << py << ", "<< pz << ")" << std::endl << 
        "(pitch, roll, yaw) = (" << pitch << ", " << roll << ", "<< yaw << ")" << std::endl << 
        "(width, height)    = (" << width <<  ", "<< height << ")" << std::endl ;
    }
};

void init(std::string env_file);
void loop();

// 以下はds標準の描画処理を用いない場合のみ使用
void start();
void simLoop(int pause); // if pause == 0 : シミュレーション続行、pause>0 : シミュレーションストップ（描画は引き続きおこなう）
void quit();

void printLog();
void setMotorParams(int id, int Kp, int Ki, int Ti, int max_current);
void setMotorCurrent(int id, float current);
void freeMotor(int id);
std::uint16_t getEncTotal(int id);
std::uint16_t getLineSensorColors(int id);

bool loadEnvFile(std::string);

void addCallback(double t, CallbackType fn);
void throwObject(int index, std::vector<double> vel, std::vector<double>initial_pos);
void setRobotPos(double x, double y, double theta);

bool getPointCloudXYZ(std::vector<float> *src);
bool getPointCloudXYZRGB(std::vector<float> *src);

bool ready_to_get_new_frame();
cv::Mat getCameraImg();
cv::Mat getCameraDepthImg();
void setIfDrawImages(bool);
CameraPose getCameraPose();
// static void start();
// static void nearCallback(void *, dGeomID, dGeomID);
// static void command(int);
// void control();
// static void simLoop(int);
// void setDrawStuff();
// void makeOmni();

void control(int);
void setPauseEveryFrame(bool);
void next();
};

#endif



