#pragma once
#include <autograph/Config.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ag 
{
//////////////////////////////////////////////
// re-export glm types under our namespace for convenience
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::mat2;
using glm::mat3;
using glm::mat4;

using glm::mat4x4;
using glm::mat3x3;
using glm::mat2x2;
using glm::mat3x4;

using glm::quat;

template <typename T> constexpr T kPi = T(3.14159265358979323846264338327950288);
template <typename T> constexpr T kTwoPi = T(6.28318530717958647692528676655900576);
template <typename T> constexpr T kPiOverTwo = T(1.57079632679489661923132169163975144);
}