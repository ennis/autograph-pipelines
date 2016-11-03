#pragma once
#include <autograph/Types.h>

namespace ag {

struct Transform {
  vec3 scaling = glm::vec3(1.f);
  vec3 position = glm::vec3(0.f);
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
};

}