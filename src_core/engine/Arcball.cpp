#include <autograph/engine/Arcball.h>
#include <autograph/Transform.h>
#include <autograph/Camera.h>
#include <glm/gtx/quaternion.hpp>

namespace ag {
// Apply an arcball rotation to an object
void Arcball::onMouseDown(int mouseX, int mouseY) {
  mouseDownX = mouseX;
  mouseDownY = mouseY;
}

void Arcball::onMouseDrag(Transform &transform, const Camera &cam,
                          int screenWidth, int screenHeight, int mouseX,
                          int mouseY) {
  if (mouseX != mouseDownX || mouseY != mouseDownY) {
    glm::vec3 va =
        getArcballVector(screenWidth, screenHeight, mouseDownX, mouseDownY);
    glm::vec3 vb = getArcballVector(screenWidth, screenHeight, mouseX, mouseY);
    float angle = std::acos(glm::min(1.0f, glm::dot(va, vb)));
    vec3 axis_in_camera_coord = glm::cross(va, vb);
    mat3 camera2object =
        glm::inverse(mat3(cam.viewMat) * mat3(transform.getMatrix()));

    vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
    transform.rotation = glm::rotation(va, vb);
    // glm::rot
    // modelMat = glm::rotate(modelMat, angle, axis_in_object_coord);
    mouseDownX = mouseX;
    mouseDownY = mouseY;
  }
}

vec3 Arcball::getArcballVector(int sw, int sh, int x, int y) const {
  vec3 P =
      vec3(1.0 * x / (float)sw * 2 - 1.0, 1.0 * y / (float)sh * 2 - 1.0, 0);
  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1)
    P.z = sqrt(1 - OP_squared);
  else
    P = glm::normalize(P);
  return P;
}
}