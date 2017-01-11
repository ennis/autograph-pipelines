#include <autograph/engine/MathUtils.h>
#include <glm/gtc/random.hpp>

namespace ag {

  vec2 diskRandom(float radius)
  {
	  return glm::diskRand(radius);
  }

}