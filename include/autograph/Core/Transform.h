#pragma once
#include <autograph/Core/Types.h>
#include <autograph/Core/Exports.h>

namespace ag {

struct AG_CORE_API Transform {
  vec3 scaling {1.0f, 1.0f, 1.0f};
  vec3 position {0.0f, 0.0f, 0.0f};
  quat rotation;

  //  
  mat4 getMatrix() const;
  // 
  mat3 getNormalMatrix() const;
  // application à un point
  vec3 transformPoint(const vec3 &pos) const;
  // application à un vecteur
  vec3 transformVec(const vec3 &vector) const;
  // transformation d'une normale
  vec3 transformNormal(const vec3 &n) const;

  static Transform fromMatrix(const mat4& matrix);
};

}