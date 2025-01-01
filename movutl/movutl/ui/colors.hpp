
#pragma once
#include <movutl/core/vector.hpp>

namespace mu::colors {
inline const core::Vec3b white(255, 255, 225);
inline const core::Vec3b red(255, 0, 0);
inline const core::Vec3b green(0, 255, 0);
inline const core::Vec3b blue(0, 0, 255);
inline const core::Vec3b black(0, 0, 0);
inline const core::Vec3b yellow(255, 255, 0);
inline const core::Vec3b silver(100, 100, 100);
inline const core::Vec3b purple(255, 0, 255);
inline const core::Vec3b orange(255, 165, 0);
inline const core::Vec3b powderblue(176, 224, 230);
inline const core::Vec3b skyblue(135, 206, 235);
inline const core::Vec3b brown(165, 42, 42);
inline const core::Vec3b beige(238, 228, 179);
inline const core::Vec3b aqua(144, 250, 253);
inline const core::Vec3b chocolate(153, 86, 60);
inline const core::Vec3b coral(255, 127, 80);
inline const core::Vec3b blue_violet(119, 46, 221);
inline const core::Vec3b maroon(128, 0, 0);
inline const core::Vec3b darkred(139, 0, 0);
inline const core::Vec3b light_salmon(255, 160, 122);
inline const core::Vec3b salmon(250, 128, 114);


inline const std::vector<core::Vec3b> colormap = {
  blue, red, green, silver, yellow, purple, orange, powderblue, skyblue, coral, chocolate, maroon, aqua, beige, salmon,
};

} // namespace mu::colors
