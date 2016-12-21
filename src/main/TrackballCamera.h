#pragma once
#include <autograph/Camera.h>
#include <glm/gtx/rotate_vector.hpp>

namespace ag {

static const vec3 CamFront{0.0f, 0.0f, 1.0f};
static const vec3 CamRight{1.0f, 0.0f, 0.0f};
static const vec3 CamUp{0.0f, 1.0f, 0.0f};

/////////////////////////////////////////////////////
class CameraControl {
public:
  void zoomIn(float dzoom)
  {
  	zoomLevel_ += dzoom;
  }

  void setZoom(float zoom)
  {
  	zoomLevel_ = zoom;
  }

  //
  void rotate(float dTheta, float dPhi) {
  	theta_ += dTheta;
  	phi_ += dPhi;
  }
  // dx, dy in camera space
  void pan(float dx, float dy)
  {
  	const vec3 look = glm::normalize(toCartesian());
    const vec3 worldUp = vec3{0.0f, 1.0f, 0.0f};
    const vec3 right = glm::cross(look, worldUp);
    const vec3 up = glm::cross(look, right);
    target_ = target_ + (right * dx) + (up * dy);
  }
  //
  void lookAt(vec3 lookAt)
  {
  	target_ = lookAt;
  }
  void lookAt(float x, float y, float z)
  {
  	target_ = vec3{x,y,z};
  }
  void setAspectRatio(float aspect_ratio) 
  {
  	aspectRatio_ = aspect_ratio;
  }
  void setFieldOfView(float fov) {
  	fov_ = fov;
  }
  void setNearFarPlanes(float nearPlane, float farPlane) {
  	nearPlane_ = nearPlane;
  	farPlane_ = farPlane;
  }
  //
  Camera getCamera() const
  {
  	Camera cam;
  	cam.viewMat = getLookAt();
  	cam.invViewMat = glm::inverse(cam.viewMat);        
  	cam.projMat = glm::scale(vec3{zoomLevel_, zoomLevel_, 1.0f}) *
                  glm::perspective(fov_, aspectRatio_, nearPlane_, farPlane_);
    cam.wEye = vec3(glm::inverse(cam.viewMat) *
                         vec4{0.0f, 0.0f, 0.0f, 1.0f});
    return cam;
  }

private:
  mat4 getLookAt() const {
    return glm::lookAt(target_ + toCartesian(), target_, CamUp);
  }

  vec3 toCartesian() const {
    float x = radius_ * sinf(phi_) * sinf(theta_);
    float y = radius_ * cosf(phi_);
    float z = radius_ * sinf(phi_) * cosf(theta_);
    return vec3{x, y, z};
}

	float fov_{45.0f};
	float aspectRatio_{1.0f};	// should be screenWidth / screenHeight
	float nearPlane_{0.001f};
	float farPlane_{10.0f};
  float zoomLevel_;
  float radius_;
  float theta_;
  float phi_;
  vec3 target_{0.0f, 0.0f, 0.0f};
};

/////////////////////////////////////////////////////
class Arcball {
public:
  // Apply an arcball rotation to an object
  void onMouseDown(int mouseX, int mouseY) 
  {
  	mouseDownX = mouseX;
  	mouseDownY = mouseY;
  }

  void onMouseDrag(mat4& modelMat, const Camera& cam, int screenWidth, int screenHeight, int mouseX, int mouseY)
  {
    if (mouseX != mouseDownX || mouseY != mouseDownY) {
      glm::vec3 va = getArcballVector(screenWidth, screenHeight, mouseDownX, mouseDownY);
      glm::vec3 vb = getArcballVector(screenWidth, screenHeight, mouseX, mouseY);
      float angle = std::acos(std::min(1.0f, glm::dot(va, vb)));
      vec3 axis_in_camera_coord = glm::cross(va, vb);
      mat3 camera2object = glm::inverse(mat3(cam.viewMat) * mat3(modelMat));
      vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
      modelMat = glm::rotate(modelMat, angle, axis_in_object_coord);
      mouseDownX = mouseX;
      mouseDownY = mouseY;
    }
  }

private:
  vec3 getArcballVector(int sw, int sh, int x, int y) {
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

  int mouseDownX;
  int mouseDownY;
};

}