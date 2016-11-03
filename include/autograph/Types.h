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
}