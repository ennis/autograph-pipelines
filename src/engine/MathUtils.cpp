#include <autograph/engine/MathUtils.h>
#include <glm/gtc/random.hpp>

namespace ag {

vec2 diskRandom(float radius) { return glm::diskRand(radius); }

uint8_t linearToSRGBComponent(float cl) {
  float cs;
  if (std::isnan(cl)) {
    /* Map IEEE-754 Not-a-number to zero. */
    cs = 0.0f;
  } else if (cl > 1.0f) {
    cs = 1.0;
  } else if (cl < 0.0f) {
    cs = 0.0;
  } else if (cl < 0.0031308f) {
    cs = 12.92f * cl;
  } else {
    cs = 1.055f * std::pow(cl, 0.41666f) - 0.055f;
  }

  /*sRGB components are typically stored as unsigned 8-bit
  fixed-point values.  If cs is computed with the above
  pseudo-code, cs can be converted to a [0,255] integer with this
  formula:*/
  uint8_t csi = (uint8_t)std::floor(255.0f * cs + 0.5f);
  return csi;
}

u8vec4 linearToSRGB(vec4 color) {
  uint8_t a = (uint8_t)(255.0f * color.a);
  return u8vec4{linearToSRGBComponent(color.r), linearToSRGBComponent(color.g),
                linearToSRGBComponent(color.b), a};
}
}