#include "simulator.hpp"
#include "mylogger.hpp"
#include <movutl/core/spline.hpp>

#define ROBOT_Z_OFFSET 0.01
#define POINT_CLOUD_SIZE (640 * 480)


using namespace mu::core;

namespace Simulator {

enum class ObjectType {
  // enum ObjectType{
  Lagori   = 0,
  Sphere   = 1,
  Cylinder = 2,
  Box      = 3,
  Unknown  = 10000
};

typedef struct {
  dBodyID body;
  dGeomID geom;
  dJointID joint;
  float color[3];
  ObjectType type;

  float r, h;
} MyObject;

typedef struct {
  std::vector<dGeomID> fields;
  std::vector<MyObject> objects; // lagori

  std::vector<std::vector<double> > field_cols;
  std::vector<std::vector<double> > field_sizes;
} FieldObject;


typedef struct {
  dBodyID body;
  dGeomID geom;
  dJointID joint;
  double color[3];
  double size[3];
  double pos[3];
  double phr[3];

  double camera_pos[3];
  double camera_phr[3];

  Spline::Spline route;
  double moving_time;
  std::vector<double> route_theta;

  cv::Mat rgb_image, depth_image;
  float point_clouds[POINT_CLOUD_SIZE];

  std::vector<std::pair<std::string, cv::Rect> > bounding_boxes;
} Robot;

typedef struct {
  double scale;
  double timestep;
  double nowTime;
  double CFN, ERP;
  double viewport_pos[3];
  double viewport_rot[3];

  dSurfaceParameters surface;
  int camera_width, camera_height;
  bool pause_every_frame;
  bool now_pause;
  bool draw_images;
} Environment;

dWorldID world;
dSpaceID space;
dGeomID ground;
dJointGroupID contactgroup;
dsFunctions fn;
FieldObject field_objects;
Robot robot;
Environment env;

std::vector<std::pair<double, CallbackType> > control_callbacks;
std::vector<CallbackType> every_frame_callbacks;

int argc;
char** argv;
int last_command;

// // objects
// MyObject wheel[WHEEL_NUM], base, ball;


// read toml file and create field
bool loadEnvFile(std::string fname) {
  LOGI << "Making Environment .....";
  auto config = cpptoml::parse_file(fname);

  auto all_config = config->get_table("all");
  auto scale      = all_config->get_as<double>("scale").value_or(1.0f);
  env.scale       = scale;
  env.timestep    = all_config->get_as<double>("timestep").value_or(0.01f);
  env.nowTime     = 0.0f;

  env.CFN = all_config->get_as<double>("CFN").value_or(1e-3);
  env.ERP = all_config->get_as<double>("ERP").value_or(0.8f);

  env.surface.mu       = all_config->get_as<double>("surface_mu").value_or(1.0f);
  env.surface.mu2      = all_config->get_as<double>("surface_mu2").value_or(1.0f);
  env.surface.slip1    = all_config->get_as<double>("surface_slip1").value_or(0.0f);
  env.surface.slip1    = all_config->get_as<double>("surface_slip2").value_or(0.0f);
  env.surface.soft_erp = all_config->get_as<double>("surface_soft_erp").value_or(0.0f);
  env.surface.soft_cfm = all_config->get_as<double>("surface_soft_cfm").value_or(0.0f);

  DISP(env.CFN);
  DISP(env.ERP);
  dWorldSetCFM(world, env.CFN);
  dWorldSetERP(world, env.ERP);

  auto viewport = all_config->get_array_of<double>("viewport");
  for(int i = 0; i < 3; i++) {
    env.viewport_pos[i] = (*viewport)[i] * scale / 1000.0f;
    env.viewport_rot[i] = (*viewport)[i + 3];
  }

  auto field_config = config->get_table("field");
  auto nfield_objs  = field_config->get_as<int>("n").value_or(-1);

  std::cout << (int)nfield_objs << std::endl;
  if(nfield_objs < 0) {
    LOGE << "Number of Field Rectangles must be > 0 : n = " + std::to_string(nfield_objs);
    return false;
  }

  field_objects.fields.resize(nfield_objs);
  field_objects.field_cols.resize(nfield_objs);
  field_objects.field_sizes.resize(nfield_objs);

  auto field_pos  = field_config->get_array_of<cpptoml::array>("pos");
  auto field_size = field_config->get_array_of<cpptoml::array>("size");
  auto field_col  = field_config->get_array_of<cpptoml::array>("colors");

  auto objects_config = config->get_table("objects");
  auto objects_params = objects_config->get_array_of<cpptoml::array>("objects");


  for(int i = 0; i < nfield_objs; i++) {
    auto pos  = (*field_pos)[i]->get_array_of<int64_t>();
    auto size = (*field_size)[i]->get_array_of<int64_t>();
    auto col  = (*field_col)[i]->get_array_of<int64_t>();

    double pos_float[] = {
      double((*pos)[0]) / 1000.0f * scale,
      double((*pos)[1]) / 1000.0f * scale,
      double((*pos)[2]) / 1000.0f * scale,
    };

    double size_float[] = {
      double((*size)[0]) / 1000.0f * scale,
      double((*size)[1]) / 1000.0f * scale,
      double((*size)[2]) / 1000.0f * scale,
    };

    double col_float[] = {
      double((*col)[0]) / 255.0f,
      double((*col)[1]) / 255.0f,
      double((*col)[2]) / 255.0f,
    };

    field_objects.fields[i] = dCreateBox(space, size_float[0], size_float[1], size_float[2]);
    dGeomSetPosition(field_objects.fields[i], pos_float[0], pos_float[1], pos_float[2]);

    field_objects.field_cols[i]  = {col_float[0], col_float[1], col_float[2]};
    field_objects.field_sizes[i] = {size_float[0], size_float[1], size_float[2]};

    // std::cout << field_objects.field_sizes[i][0] << ", " <<
    //              field_objects.field_sizes[i][1] << ", " <<
    //              field_objects.field_sizes[i][2] << ", " <<
    //              field_objects.field_cols[i][0] << ", " <<
    //              field_objects.field_cols[i][1] << ", " <<
    //              field_objects.field_cols[i][2] << ", " << std::endl;
  }

  LOGI << "Making Field Object .....";
  for(const auto& oo : *objects_params) {
    auto o   = oo->get_array_of<double>();
    int type = (*o)[0];

    double pos_float[] = {
      double((*o)[1]) / 1000.0f * scale,
      double((*o)[2]) / 1000.0f * scale,
      double((*o)[3]) / 1000.0f * scale,
    };

    float col_float[] = {
      float((*o)[4]) / 255.0f,
      float((*o)[5]) / 255.0f,
      float((*o)[6]) / 255.0f,
    };
    float float_mass = float((*o)[7]) / 1000.0f * scale;

    MyObject obj;
    for(int i = 0; i < 3; i++) {
      obj.color[i] = col_float[i];
    }

    switch(static_cast<ObjectType>(type)) {
      case ObjectType::Lagori: {
        dMass mass;
        double r = double((*o)[8]) / 1000.0f * scale;
        double h = double((*o)[9]) / 1000.0f * scale;
        LOGI << "Craate Lagori Object : (r, h) = " + std::to_string(r) + ", " + std::to_string(h);

        obj.body = dBodyCreate(world);
        dMassSetZero(&mass);
        dMassSetCylinderTotal(&mass, float_mass, 3, r, h);
        // dMassAdjust (&mass, float_mass);
        dBodySetMass(obj.body, &mass);
        obj.geom = dCreateCylinder(space, r, h);
        dGeomSetBody(obj.geom, obj.body);
        obj.type = ObjectType::Lagori;
        obj.r    = r;
        obj.h    = h;
        dBodySetPosition(obj.body, pos_float[0], pos_float[1], pos_float[2]);
        field_objects.objects.push_back(obj);
      } break;

      case ObjectType::Sphere: {
        dMass mass;
        double r = double((*o)[8]) / 1000.0f * scale;
        obj.body = dBodyCreate(world);
        dMassSetZero(&mass);
        dMassSetSphereTotal(&mass, float_mass, r);
        dBodySetMass(obj.body, &mass);
        obj.geom = dCreateSphere(space, r);
        dGeomSetBody(obj.geom, obj.body);
        dBodySetPosition(obj.body, pos_float[0], pos_float[1], pos_float[2]);
        obj.type = ObjectType::Sphere;
        obj.r    = r;
        field_objects.objects.push_back(obj);
      } break;

      default: LOGE << "Undefined Field Object Type : " + std::to_string(type); break;
    }
  }


  LOGI << "Making Robot .....";
  auto robot_config = config->get_table("robot");
  auto robot_size   = robot_config->get_array_of<int64_t>("size");
  auto robot_col    = robot_config->get_array_of<int64_t>("color");

  auto robot_camera_ = robot_config->get_array_of<double>("camera");
  auto robot_route_  = robot_config->get_array_of<cpptoml::array>("route");
  robot.moving_time  = robot_config->get_as<double>("move_time").value_or(10);

  env.camera_width  = robot_config->get_as<int>("camera_width").value_or(640);
  env.camera_height = robot_config->get_as<int>("camera_height").value_or(480);
  // robot.point_clouds.resize(env.camera_height * env.camera_width);
  env.pause_every_frame = all_config->get_as<bool>("pause_every_frame").value_or(false);
  env.now_pause         = false;
  env.draw_images       = all_config->get_as<bool>("draw_images").value_or(false);

  for(int i = 0; i < 3; i++) {
    robot.size[i]       = double((*robot_size)[i]) / 1000.0f * scale;
    robot.color[i]      = double((*robot_col)[i]) / 255.0f;
    robot.camera_pos[i] = double((*robot_camera_)[i]) / 1000.0f * scale;
    robot.camera_phr[i] = double((*robot_camera_)[i + 3]);
  }

  std::vector<double> rx, ry, rt;
  for(const auto& oo : *robot_route_) {
    auto o = oo->get_array_of<double>();
    rx.push_back(double((*o)[0]) / 1000.0f * scale);
    ry.push_back(double((*o)[1]) / 1000.0f * scale);
    robot.route_theta.push_back((*o)[2]);
  }
  robot.route.init(rx, ry, 15, true, false);
  LOGI << "Generating Robot Spline Route  .....";
  robot.route.generate();
  double zz  = robot.size[2] * scale / 2.0f + ROBOT_Z_OFFSET;
  robot.geom = dCreateBox(space, robot.size[0], robot.size[1], robot.size[2]);
  dGeomSetPosition(robot.geom, rx[0], ry[0], zz);

  robot.rgb_image   = cv::Mat(cv::Size(env.camera_width, env.camera_height), CV_8UC3);
  robot.depth_image = cv::Mat(cv::Size(env.camera_width, env.camera_height), CV_32F);
  LOGI << "Finished Making Environment Successfully!";
  return true;
}



void DrawWorld() {
  // Draw Ball
  dsSetTexture(DS_NONE);
  for(int i = 0; i < field_objects.fields.size(); i++) {
    auto obj      = field_objects.fields[i];
    auto col      = field_objects.field_cols[i];
    auto _size    = field_objects.field_sizes[i];
    double size[] = {_size[0], _size[1], _size[2]};
    dsSetColor(col[0], col[1], col[2]);
    dsDrawBox(dGeomGetPosition(obj), dGeomGetRotation(obj), size);
  }


  for(int i = 0; i < field_objects.objects.size(); i++) {
    auto obj        = field_objects.objects[i];
    ObjectType type = obj.type;

    dsSetColor(obj.color[0], obj.color[1], obj.color[2]);
    switch(type) {
      case ObjectType::Sphere: dsDrawSphere(dGeomGetPosition(obj.geom), dGeomGetRotation(obj.geom), dGeomSphereGetRadius(obj.geom)); break;

      case ObjectType::Lagori: dsDrawCylinderD(dGeomGetPosition(obj.geom), dGeomGetRotation(obj.geom), obj.h, obj.r);
      default: break;
    }
  }

  // Robot
  auto _size    = robot.size;
  double size[] = {_size[0], _size[1], _size[2]};
  dsSetColor(robot.color[0], robot.color[1], robot.color[2]);
  dsDrawBox(dGeomGetPosition(robot.geom), dGeomGetRotation(robot.geom), size);
}


void drawRoute() {
  auto rx       = robot.route.getOutputX();
  auto ry       = robot.route.getOutputY();
  double offset = 0.01f;

  for(int i = 0; i < rx.size() - 1; i++) {
    double pos1[3] = {rx[i], ry[i], offset};
    double pos2[3] = {rx[i + 1], ry[i + 1], offset};
    dsDrawLine(pos1, pos2);
  }
}

void drawCircle(dReal r, dReal pos_X, dReal pos_Y, int angle1, int angle2) {
  dReal pos1[3], pos2[3], z = 0.005;
  pos1[0] = r * cos(angle1 * M_PI / 180.0) + pos_X;
  pos1[1] = r * sin(angle1 * M_PI / 180.0) + pos_Y;
  pos1[2] = z;
  for(int i = angle1; i < angle2; i++) {
    pos2[0] = r * cos(i * M_PI / 180.0) + pos_X;
    pos2[1] = r * sin(i * M_PI / 180.0) + pos_Y;
    pos2[2] = z;
    dsDrawLine(pos1, pos2);
    pos1[0] = pos2[0];
    pos1[1] = pos2[1];
    pos1[2] = pos2[2];
  }
}

/*** �����̕`�� ***/
void drawLine() {
  dReal z        = 0.005;
  dReal center_r = 1.0, corner_r = 0.5;
  dReal pos[][3] = {{6.0, 4.0, z},  {6.0, -4.0, z},  {-6.0, -4.0, z}, {-6.0, 4.0, z}, {0.0, 4.0, z}, {0.0, -4.0, z}, {6.0, 1.5, z},  {5.5, 1.5, z},  {5.5, -1.5, z}, {6.0, -1.5, z},  {-6.0, 1.5, z},
                    {-5.5, 1.5, z}, {-5.5, -1.5, z}, {-6.0, -1.5, z}, {6.0, 2.0, z},  {4.5, 2.0, z}, {4.5, -2.0, z}, {6.0, -2.0, z}, {-6.0, 2.0, z}, {-4.5, 2.0, z}, {-4.5, -2.0, z}, {-6.0, -2.0, z}};
  dsSetColor(1.3, 1.3, 1.3);
  dsDrawLine(pos[0], pos[1]);
  dsDrawLine(pos[1], pos[2]);
  dsDrawLine(pos[2], pos[3]);
  dsDrawLine(pos[3], pos[0]);
  dsDrawLine(pos[4], pos[5]);
  dsDrawLine(pos[6], pos[7]);
  dsDrawLine(pos[7], pos[8]);
  dsDrawLine(pos[8], pos[9]);
  dsDrawLine(pos[10], pos[11]);
  dsDrawLine(pos[11], pos[12]);
  dsDrawLine(pos[12], pos[13]);
  dsDrawLine(pos[14], pos[15]);
  dsDrawLine(pos[15], pos[16]);
  dsDrawLine(pos[16], pos[17]);
  dsDrawLine(pos[18], pos[19]);
  dsDrawLine(pos[19], pos[20]);
  dsDrawLine(pos[20], pos[21]);

  // �Z���^�[�T�[�N���̕`��
  drawCircle(center_r, 0, 0, 0, 360);
  drawCircle(corner_r, -6, -4, 0, 90);
  drawCircle(corner_r, -6, 4, -90, 0);
  drawCircle(corner_r, 6, 4, 90, 180);
  drawCircle(corner_r, 6, -4, -180, -90);
}

void setCamPos(const double* _xyz, const double* _hpr) {
  const float xyz[3] = {
    (float)_xyz[0],
    (float)_xyz[1],
    (float)_xyz[2],
  };
  const float hpr[3] = {
    (float)_hpr[0],
    (float)_hpr[1],
    (float)_hpr[2],
  };
  dsSetViewpoint(xyz, hpr);
  dsSetSphereQuality(3);
}

// static void start(){
void start() {
  setCamPos(env.viewport_pos, env.viewport_rot);
  dsSetSphereQuality(3);
}

ObjectType getObjType(dBodyID b) {
  for(int i = 0; i < field_objects.objects.size(); i++) {
    if(field_objects.objects[i].body == b) {
      return field_objects.objects[i].type;
    }
  }
  return ObjectType::Unknown;
}


static void nearCallback(void* data, dGeomID o1, dGeomID o2) {
  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);
  // bool lagori_flag = false;
  // if(getObjType(b1) == ObjectType::Lagori && getObjType(b2) == ObjectType::Lagori){
  //     lagori_flag = true;
  // }

  if(b1 && b2 && dAreConnected(b1, b2)) return;

  const int N = 10;
  dContact contact[N];
  auto n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
  if(n > 0) {
    for(auto i = 0; i < n; i++) {
      contact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
      // if (dGeomGetClass(o1) == dSphereClass || dGeomGetClass(o2) == dSphereClass)
      // 	contact[i].surface.mu = 20;
      // else
      // 	contact[i].surface.mu = 0.5;
      contact[i].surface = env.surface;
      dJointID c         = dJointCreateContact(world, contactgroup, contact + i);
      dJointAttach(c, dGeomGetBody(o1), dGeomGetBody(o2));
    }
  }
}

static void command(int cmd) {
  // last_command = cmd;
  // switch (cmd)
  // {
  // case '1':
  //     float xyz1[3],hpr1[3];
  //     dsGetViewpoint(xyz1,hpr1);
  //     printf("xyz=%4.2f %4.2f %4.2f ",xyz1[0],xyz1[1],xyz1[2]);
  //     printf("hpr=%6.2f %6.2f %5.2f \n",hpr1[0],hpr1[1],hpr1[2]);
  //     break;

  // case 'b':
  //     break;
  // }
}

/*** ��  �� ***/
// void control(){
//     // dReal tmp = dJointGetAMotorAngle(wheel[0].motor, 1);
//     // dReal u   = kp * (target - tmp);
//     if(last_command == 'a'){
//         std::cout << "[command] = a \n";
//         last_command = 0;
//         dJointSetHingeParam(wheel[0].joint, dParamVel , 1.0);
//         dJointSetHingeParam(wheel[0].joint, dParamFMax, MOTOR_F_MAX);
//     }

//     if(last_command == 's'){
//         std::cout << "[command] = s \n";

//         last_command = 0;
//         dJointSetHingeParam(wheel[0].joint, dParamVel , 0.0);
//         dJointSetHingeParam(wheel[0].joint, dParamFMax, MOTOR_F_MAX);
//     }

// }



void setRobotPos(double x, double y, double theta) {
  // LOGD << "[Robot Position Update] (x, y, theta) = " + std::to_string(x)  + ", " + std::to_string(y)  + ", " + std::to_string(theta);
  double zz = robot.size[2] * env.scale / 2.0f + ROBOT_Z_OFFSET;
  dGeomSetPosition(robot.geom, x, y, zz);
  dMatrix3 Rotation;
  dRFromEulerAngles(Rotation, 0, 0, -theta * M_PI / 180.0f);
  // dRFromAxisAndAngle(Rotation, 0, 0, 1, 90);
  dGeomSetRotation(robot.geom, Rotation);

  robot.pos[0] = x;
  robot.pos[1] = y;
  robot.pos[2] = zz;

  robot.phr[0] = 0;
  robot.phr[1] = 0;
  robot.phr[2] = theta;
}

void throwObject(int index, std::vector<double> vel, std::vector<double> initial_pos) {
  assert(vel.size() == 3);
  assert(initial_pos.size() == 3);
  if(index >= field_objects.objects.size()) {
    LOGE << "Throwing Error: index >= field_objects.objects.size() :: index = " + std::to_string(index) + ", field_objects.size() = " + std::to_string(field_objects.objects.size());
    return;
  }

  double p[] = {initial_pos[0] / 1000.0f * env.scale, initial_pos[1] / 1000.0f * env.scale, initial_pos[2] / 1000.0f * env.scale};

  double v[] = {vel[0] * env.scale, vel[1] * env.scale, vel[2] * env.scale};
  auto body  = field_objects.objects[index].body;
  dBodySetPosition(body, p[0], p[1], p[2]);
  dBodySetLinearVel(body, v[0], v[1], v[2]);
  dBodySetAngularVel(body, 0, 0, 0);
}

void updateRobot() {
  double r = env.nowTime / robot.moving_time;

  int index        = robot.route_theta.size() * r;
  double rr        = r * robot.route_theta.size() - index;
  int before_index = std::min<int>(index, robot.route_theta.size());
  int after_index  = std::min<int>(index + 1, robot.route_theta.size());
  double theta     = robot.route_theta[before_index] * (1.0f - rr) + robot.route_theta[after_index] * rr;
  if(r < 1.0) {
    auto pos = robot.route.getPosFromRange(r);
    setRobotPos(pos.x, pos.y, theta);
  }
}



bool getPointCloudXYZ(std::vector<float>* src) { return false; }
bool getPointCloudXYZRGB(std::vector<float>* src) { return false; }


CameraPose getCameraPose() {
  float xyz[3];
  float hpr[3];
  dsGetViewpoint(xyz, hpr);
  dMatrix3 R;
  return CameraPose(xyz[0], xyz[1], xyz[2], hpr[1], hpr[2], hpr[0], env.camera_width, env.camera_height);
}


cv::Mat getCameraImg() { return robot.rgb_image; }
cv::Mat getCameraDepthImg() { return robot.depth_image; }

void setPauseEveryFrame(bool v) { env.pause_every_frame = v; }


void setMotorCurrent(int id, float current) {
  // MY_ASSERT(id >= WHEEL_NUM);
  //  if(id >= WHEEL_NUM) {std::cout << "[ERROR] in function setMotorCurrent()  --> 0 <= id < WHEEL_NUM に設定してください\n"; return;}
  //  dJointSetHingeParam(wheel[id].joint, dParamVel , current);
  //  dJointSetHingeParam(wheel[id].joint, dParamFMax, MOTOR_F_MAX);
}

void freeMotor(int id) {
  // MY_ASSERT(id >= WHEEL_NUM);
  //  if(id >= WHEEL_NUM){ std::cout << "[ERROR] in function freeMotor()  --> 0 <= id < WHEEL_NUM に設定してください\n"; return;}
  //  dJointSetHingeParam(wheel[id].joint, dParamFMax, MOTOR_F_MAX);
}

void setMotorParams(int id, int Kp, int Ki, int Ti, int max_current) {}

// エンコーダーの値を得る
std::uint16_t getEncTotal(int id) {
  // MY_ASSERT(id >= 2);
  if(id >= 2) {
    std::cout << "[ERROR] in function getEncTotal()  --> 0 <= id < 2 に設定してください\n";
    return -1;
  }

  if(id == 0) {
    // TODO:
    return 0;
  } else {
    // TODO:
    return 0;
  }
}

// ラインセンサーの各素子(16個)が読んだ色をまとめて得る
std::uint16_t getLineSensorColors(int id) {
  // TODO:
  return 0;
}


//現実の位置や速度の情報を含んだログを出力する。
void printLog() {
  // TODO
}

void addCallback(double t, CallbackType fn) {
  if(t >= 0) {
    control_callbacks.push_back({t, fn});
  } else {
    every_frame_callbacks.push_back(fn);
  }
}

void updateCallbacks() {
  for(int i = 0; i < control_callbacks.size(); i++) {
    if(i < control_callbacks.size() && control_callbacks[i].first < env.nowTime) {

      (control_callbacks[i].second)(env.nowTime);
      control_callbacks.erase(control_callbacks.begin() + i);
    }
  }
}

// static void simLoop(int pause){
void simLoop(int pause) {
  if(!pause && !env.now_pause) {
    // TODO: loop();
    env.nowTime += env.timestep;
    dSpaceCollide(space, 0, &nearCallback); // add
    // control();

    control(last_command);
    last_command = 0;

    dWorldStep(world, env.timestep);
    dJointGroupEmpty(contactgroup); // add

    updateRobot();
    updateCallbacks();
  }
  DrawWorld();
  drawRoute();

  auto robotPos = dGeomGetPosition(robot.geom);
  auto robotRot = dGeomGetRotation(robot.geom);

  auto ttheta          = robot.phr[2] + robot.camera_phr[0];
  auto ttheta_rad      = ttheta * M_PI / 180.0f;
  double camera_pos[3] = {
    robotPos[0] + robot.camera_pos[0] * std::cos(ttheta_rad),
    robotPos[1] + robot.camera_pos[1] * std::sin(ttheta_rad),
    robotPos[2] + robot.camera_pos[2],
  };

  double camera_phr[3] = {
    robot.phr[2] + robot.camera_phr[0],
    robot.phr[0] + robot.camera_phr[1],
    robot.phr[1] + robot.camera_phr[2],
  };
  setCamPos(camera_pos, camera_phr);


  glReadPixels(0, 0, env.camera_width, env.camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, robot.rgb_image.data);
  glReadPixels(0, 0, env.camera_width, env.camera_height, GL_DEPTH_COMPONENT, GL_FLOAT, robot.depth_image.data);
  cv::flip(robot.rgb_image, robot.rgb_image, 0);
  cv::flip(robot.depth_image, robot.depth_image, 0);
  bool calcBoundingBoxes = false;
  bool draw              = false;


  // #pragma omp parrarel for
  // for(int j = 0; j<env.camera_height;j++){
  //     auto src = robot.depth_image.ptr<float>(j);
  //     for(int k =0; k<env.camera_width; k++){
  //         robot.point_clouds[k + j*env.camera_height] = src[k] * 1.0f;
  //     }
  // }

  double Mean       = 0.1;
  double StdDev     = 0.2;
  cv::Mat noise_img = cv::Mat(robot.depth_image.size(), CV_32F);
  cv::randn(noise_img, cv::Scalar::all(Mean), cv::Scalar::all(StdDev));

  robot.depth_image = robot.depth_image * 100.0f + noise_img;

  if(calcBoundingBoxes) {
    double inv_camera_phr_rad[] = {
      (-camera_phr[0] + 90) * M_PI / 180.0f, // yaw
      -camera_phr[1] * M_PI / 180.0f,        // pitch
      -camera_phr[2] * M_PI / 180.0f         /// roll
    };
    double Rotation[] = {std::cos(-inv_camera_phr_rad[0]), std::sin(-inv_camera_phr_rad[0]), 0, -std::sin(-inv_camera_phr_rad[0]), std::cos(-inv_camera_phr_rad[0]), 0, 0, 0, 1};

    auto mDot = [](double mat[], double vec3[]) { return std::vector<double>{mat[0] * vec3[0] + mat[1] * vec3[1] + mat[2] * vec3[2], mat[3] * vec3[0] + mat[4] * vec3[1] + mat[5] * vec3[2], mat[6] * vec3[0] + mat[7] * vec3[1] + mat[8] * vec3[2]}; };

    std::cout << "--------------------------------------------------------------" << std::endl;
    std::vector<std::string> anotation{"lagori500", "lagori425", "lagori350", "lagori275", "lagori200", "ball"};
    robot.bounding_boxes.clear();
    for(int i = 0; i < field_objects.objects.size(); i++) {
      auto pos_abs = dGeomGetPosition(field_objects.objects[i].geom);
      float cam_pos[3];
      float cam_phr[3];
      dsGetViewpoint(cam_pos, cam_phr);
      double pos[3] = {
        pos_abs[0] - cam_pos[0],
        pos_abs[1] - cam_pos[1],
        pos_abs[2] - cam_pos[2],
      };

      auto pos2 = mDot(Rotation, pos);
      // double pos2[3] = {
      //     pos[0] * Rotation[0] + pos[1] * Rotation[1] + pos[2] * Rotation[2],// + Rotation[3],
      //     pos[0] * Rotation[3] + pos[1] * Rotation[4] + pos[2] * Rotation[5],// + Rotation[3],
      //     pos[0] * Rotation[6] + pos[1] * Rotation[7] + pos[2] * Rotation[8],// + Rotation[3],
      // };

      double fx = 650 * env.scale;
      double fy = 600 * env.scale;
      double x  = fx * pos2[0] / pos2[1] + env.camera_width / 2;
      double y  = -fy * pos2[2] / pos2[1] + env.camera_height / 2;

      std::cout << camera_phr[0] << "\t | \t" << pos_abs[0] << "\t" << pos_abs[1] << "\t" << pos_abs[2] << "\t | \t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] << "\t | \t" << pos2[0] << "\t" << pos2[1] << "\t" << pos2[2] << "\t | \t" << x << "\t" << y << std::endl;
      if(pos2[1] > 0) {
        cv::Rect rect(x - 30, y - 25, 60, 50);
        robot.bounding_boxes.push_back({anotation[i], rect});
        cv::rectangle(robot.rgb_image, rect, cv::Scalar(0, 0, 255), 1, cv::LINE_4);
        cv::rectangle(robot.rgb_image, cv::Rect(x - 30, y - 25, 30, 10), cv::Scalar(0, 0, 255), -1);
        cv::putText(robot.rgb_image, anotation[i], cv::Point(x - 30, y - 20), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(30, 255, 30), 2);
      }
    }
  }

  if(env.draw_images) {
    cv::Mat img_depth_gray = cv::Mat(cv::Size(env.camera_width, env.camera_height), CV_8UC1);
    double mMin, mMax, mRange, offset, rate;
    cv::Point minP, maxP;
    cv::minMaxLoc(robot.depth_image, &mMin, &mMax, &minP, &maxP);
    mRange = mMax - mMin;
    rate   = 255.0f / mRange;
    offset = mMin * rate;

    std::cout << "MIN, MAX = " << mMax << ", " << mMin << std::endl;

#pragma omp parrarel for
    for(int j = 0; j < env.camera_height; j++) {
      auto dst = img_depth_gray.ptr<unsigned char>(j);
      auto src = robot.depth_image.ptr<float>(j);
      for(int k = 0; k < env.camera_width; k++) {
        dst[k] = (src[k] - mMin) * rate;
      }
    }
    cv::Mat depth_colored;
    cv::applyColorMap(img_depth_gray, depth_colored, cv::COLORMAP_JET);

    cv::imshow("camera", robot.rgb_image);
    cv::imshow("depth", depth_colored);
    cv::waitKey(1);
  }

  if(env.pause_every_frame) {
    env.now_pause = true;
  }


  for(int i = 0; i < every_frame_callbacks.size(); i++) {
    every_frame_callbacks[i](env.nowTime);
  }
}

void setDrawStuff() {
  fn.version          = DS_VERSION;
  fn.start            = Simulator::start;
  fn.step             = Simulator::simLoop;
  fn.command          = Simulator::command;
  fn.path_to_textures = "./textures";
}


void init(std::string env_filename) {
  dInitODE();
  setDrawStuff();
  world        = dWorldCreate();
  space        = dHashSpaceCreate(0);
  contactgroup = dJointGroupCreate(0);
  ground       = dCreatePlane(space, 0, 0, 1, 0);

  dWorldSetGravity(world, 0.0, 0.0, -9.8);
  loadEnvFile(env_filename);
  dWorldSetCFM(world, env.CFN);
  dWorldSetERP(world, env.ERP);
}

bool _get_alraeady_last_frame = false;
void next() {
  if(!env.pause_every_frame) {
    LOGE << "1フレームごと書き出すにはenv.pause_every_frameをTRUEにしてください";
  }
  env.now_pause            = false;
  _get_alraeady_last_frame = false;
}

bool ready_to_get_new_frame() {
  if(!_get_alraeady_last_frame && env.now_pause) {
    _get_alraeady_last_frame = true;
    return true;
  }
  return false;
}
void setIfDrawImages(bool v) { env.draw_images = v; }

void loop() { dsSimulationLoop(argc, argv, env.camera_width, env.camera_height, &fn); }

void quit() {
  dJointGroupDestroy(contactgroup);
  dSpaceDestroy(space);
  dWorldDestroy(world);
  dCloseODE();
}

void control(int a) {}
} // namespace Simulator

