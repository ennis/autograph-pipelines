#include <autograph/Transform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ag 
{
	 // matrice 4x4 correspondante
  mat4 Transform::getMatrix() const {
    return glm::translate(glm::scale(glm::toMat4(rotation), scaling), position);
  }

  // matrice de transformation des normales
  mat3 Transform::getNormalMatrix() const {
    return glm::inverseTranspose(glm::mat3{ getMatrix()});
  }


  // application à un point
  vec3 Transform::transformPoint(const vec3 &pos) const {
    return position + rotation * scaling * pos;
  }

  // application à un vecteur
  vec3 Transform::transformVec(const vec3 &vector) const {
    return rotation * scaling * vector;
  }

  // transformation d'une normale
  vec3 Transform::transformNormal(const vec3 &n) const {
    mat3 mat = getNormalMatrix();
    return mat * n;
  }

}