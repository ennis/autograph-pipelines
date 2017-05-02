#include <autograph/Core/Transform.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ag 
{
	Transform Transform::fromMatrix(const mat4& matrix)
	{
		Transform t;
		t.position = vec3{ matrix[3] };
		t.scaling = vec3{ glm::length(matrix[0]), glm::length(matrix[1]), glm::length(matrix[2]) };
		mat4 rotMat = matrix;
		rotMat[3] = vec4{ 0.0f,0.0f,0.0f,1.0f };
		rotMat[0] /= t.scaling.x;
		rotMat[1] /= t.scaling.y;
		rotMat[2] /= t.scaling.z;
		t.rotation = glm::quat_cast(rotMat);
		return t;
	}

	 // matrice 4x4 correspondante
  mat4 Transform::getMatrix() const {
      return glm::scale(glm::translate(glm::mat4{1.0f}, position) * glm::toMat4(rotation), scaling);
  }

  // matrice de transformation des normales
  mat3 Transform::getNormalMatrix() const {
    return glm::inverseTranspose(mat3{ getMatrix()});
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
