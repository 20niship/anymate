#pragma once
#include <movutl/core/quotanion.hpp>
#include <movutl/core/vector.hpp>
#include <istream>
#include <movutl/db/block.hpp>
#include <glm/glm.hpp>

namespace mu::db{

struct KeyPosition {
  core::Vec3 pos;
  float timeStamp;
};

struct KeyRotation {
  core::Quat orientation;
  float timeStamp;
};

struct KeyScale {
  core::Vec3 scale[3];
  float timeStamp;
};

class Bone : public Block{
private:
  float get_scale_factor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    const float midWayLength = animationTime - lastTimeStamp;
    const float framesDiff   = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
  }

  template <typename T> auto mix(T a, T b, float scale) const { return a * scale + b * (a - scale); }


// TODO: Bone
#if 0 
  auto InterpolatePosition(float animationTime) {
    if(1 == positions.size()) return glm::mat4(1.0f), positions[0].pos);
    const int p0Index        = get_position_index(animationTime);
    const int p1Index        = p0Index + 1;
    const float scaleFactor  = get_scale_factor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
    const core::Vec3 finalPosition = mix(positions[p0Index].pos, positions[p1Index].pos, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
  }

  auto InterpolateRotation(float animationTime) {
    if(1 == rotations.size()) {
      auto rotation = glm::normalize(rotations[0].orientation);
      return glm::toMat4(rotation);
    }

    const int p0Index        = get_rotation_index(animationTime);
    const int p1Index        = p0Index + 1;
    const float scaleFactor  = get_scale_factor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
    core::Quat finalRotation = glm::slerp(rotatinos[p0Index].orientation, rotations[p1Index].orientation, scaleFactor);
    core::Quat finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
  }

  core::Mat4x4 InterpolateScaling(float animationTime) {
    if(1 == scales.size()) return glm::scale(glm::mat4(1.0f), scales[0].scale);
    const int p0Index          = get_scale_index(animationTime);
    const int p1Index          = p0Index + 1;
    const float scaleFactor    = get_scale_factor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
    const glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
  }

public:
  Bone(const char* name, int ID) {
    id.name        = name;
    id.instance_id = ID;
  }

  void Update(float animationTime) {
    auto translation = InterpolatePosition(animationTime);
    auto rotation    = InterpolateRotation(animationTime);
    auto scale       = InterpolateScaling(animationTime);
    local_transform  = translation * rotation * scale;
  }
  auto get_local_transform() const { return local_transform; }
  auto get_name() const { return id.name; }
  int get_bone_id() { return id.instance_id; }

  int get_position_index(float animationTime) {
    for(auto index = 0; index < positions.size() - 1; ++index) {
      if(animationTime < positions[index + 1].timeStamp) return index;
    }
    return -1;
  }

  int get_rotation_index(float animationTime) {
    for(int index = 0; index < rotations.size() - 1; ++index) {
      if(animationTime < rotations[index + 1].timeStamp) return index;
    }
    return -1;
  }

  int get_scale_index(float animationTime) {
    for(int index = 0; index < scales.size() - 1; ++index) {
      if(animationTime < scales[index + 1].timeStamp) return index;
    }
    assert(0);
  }

private:
  core::Vec<KeyPosition> positions;
  core::Vec<KeyRotation> rotations;
  core::Vec<KeyScale> scales;
  core::Mat4x4 local_transform;
#endif
};

} // namespace mu::database
