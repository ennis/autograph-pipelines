#include <autograph/engine/MathUtils.h>
#include <glm/gtc/random.hpp>

namespace ag {

  vec2 diskRandom()
  {
	  return glm::diskRand(1.0f);
  }

}