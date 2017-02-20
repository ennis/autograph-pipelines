#pragma once
#include <autograph/Camera.h>
#include <autograph/AABB.h>
#include <glm/gtx/rotate_vector.hpp>

namespace ag {

static const vec3 CamFront{0.0f, 0.0f, 1.0f};
static const vec3 CamRight{1.0f, 0.0f, 0.0f};
static const vec3 CamUp{0.0f, 1.0f, 0.0f};

/////////////////////////////////////////////////////
class CameraControl {
public:
  void zoomIn(float dzoom);
  void setZoom(float zoom);
  void rotate(float dTheta, float dPhi);
  // dx, dy in camera space
  void pan(float dx, float dy);
  void lookAt(vec3 lookAt);
  void lookAt(float x, float y, float z);
  void lookDistance(float lookDist);
  void setAspectRatio(float aspect_ratio);
  void setFieldOfView(float fov);
  void setNearFarPlanes(float nearPlane, float farPlane);
  auto getCamera() const -> Camera;
  void centerOnObject(const AABB &objectBounds);

private:
  auto getLookAt() const -> mat4;
  auto toCartesian() const -> vec3;

  float fov_{45.0f};
  float aspectRatio_{1.0f}; // should be screenWidth / screenHeight
  float nearPlane_{0.001f};
  float farPlane_{10.0f};
  float zoomLevel_{1.0f};
  float radius_{1.0f};
  float theta_{0.0f};
  float phi_{kPiOverTwo<float>};
  vec3 target_{0.0f, 0.0f, 0.0f};
};

/////////////////////////////////////////////////////
}