#include <autograph/engine/CameraControl.h>
#include <autograph/support/Debug.h>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

namespace ag {
void CameraControl::zoomIn(float dzoom) { zoomLevel_ += dzoom; }

void CameraControl::setZoom(float zoom) { zoomLevel_ = zoom; }

//
void CameraControl::rotate(float dTheta, float dPhi) {
  theta_ += dTheta;
  phi_ += dPhi;
}
// dx, dy in camera space
void CameraControl::pan(float dx, float dy) {
  const vec3 look = glm::normalize(toCartesian());
  const vec3 worldUp = vec3{0.0f, 1.0f, 0.0f};
  const vec3 right = glm::cross(look, worldUp);
  const vec3 up = glm::cross(look, right);
  target_ = target_ + (right * dx) + (up * dy);
}
//
void CameraControl::lookAt(vec3 lookAt) { target_ = lookAt; }
void CameraControl::lookAt(float x, float y, float z) {
  target_ = vec3{x, y, z};
}
void CameraControl::lookDistance(float lookDist) { radius_ = lookDist; }
void CameraControl::setAspectRatio(float aspect_ratio) {
  aspectRatio_ = aspect_ratio;
}
void CameraControl::setFieldOfView(float fov) { fov_ = fov; }
void CameraControl::setNearFarPlanes(float nearPlane, float farPlane) {
  nearPlane_ = nearPlane;
  farPlane_ = farPlane;
}
//
Camera CameraControl::getCamera() const {
  Camera cam;
  cam.viewMat = getLookAt();
  cam.invViewMat = glm::inverse(cam.viewMat);
  cam.projMat =
      glm::scale(vec3{zoomLevel_, zoomLevel_, 1.0f}) *
      glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_, farPlane_);
  cam.wEye = vec3(glm::inverse(cam.viewMat) * vec4{0.0f, 0.0f, 0.0f, 1.0f});
  // AG_DEBUG("modelMat {},{},{},{}", cam.modelMatrix[0],
  // objectUniforms.modelMatrix[1], objectUniforms.modelMatrix[2],
  // objectUniforms.modelMatrix[3]);
  // AG_DEBUG("fov {} aspectRatio {} nearPlane {} farPlane {}", cam.projMat[0],
  // cam.projMat[1], cam.projMat[2], cam.projMat[3]);
  // AG_DEBUG("viewMat {},{},{},{}", cam.viewMat[0], cam.viewMat[1],
  // cam.viewMat[2], cam.viewMat[3]);
  // AG_DEBUG("projMat {},{},{},{}", cam.projMat[0], cam.projMat[1],
  // cam.projMat[2], cam.projMat[3]);
  return cam;
}

mat4 CameraControl::getLookAt() const {
  // AG_DEBUG("getLookAt: {}, {}", target_ + toCartesian(), target_);
  return glm::lookAt(target_ + toCartesian(), target_, CamUp);
}

vec3 CameraControl::toCartesian() const {
  float x = radius_ * sinf(phi_) * sinf(theta_);
  float y = radius_ * cosf(phi_);
  float z = radius_ * sinf(phi_) * cosf(theta_);
  return vec3{x, y, z};
}

void CameraControl::centerOnObject(const AABB &objectBounds) {
	auto size = std::max(
	{ objectBounds.width(), objectBounds.height(), objectBounds.depth() });
	auto cx = (objectBounds.xmax + objectBounds.xmin) / 2.f;
	auto cy = (objectBounds.ymax + objectBounds.ymin) / 2.f;
	auto cz = (objectBounds.zmax + objectBounds.zmin) / 2.f;
	const float fov = 45.0f;
	float camDist = (0.5f * size) / std::tan(0.5f * glm::radians(fov));
	lookAt(cx, cy, cz);
	lookDistance(camDist);
	setNearFarPlanes(0.1f * camDist, 10.0f * camDist);
	setFieldOfView(fov);
	AG_DEBUG("near {} far {}", 0.5f * camDist, 2.0f * camDist);
}
}