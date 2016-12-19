#pragma once
#include <autograph/Camera.h>

namespace ag {

const vec3 CamFront{0.0f, 0.0f, 1.0f};
const vec3 CamRight{1.0f, 0.0f, 0.0f};
const vec3 CamUp{0.0f, 1.0f, 0.0f};

/////////////////////////////////////////////////////
class CameraControl {
public:
  CameraControl &zoomIn(float dzoom);
  CameraControl &setZoom(float zoom);
  //
  void rotate(float dTheta, float dPhi) {
  	rotTheta_ += dTheta;
  	rotPhi_ += dPhi;
  }
  // dx, dy in camera space
  void pan(float dx, float dy)
  {
  	const vec3 look = glm::normalize(toCartesian());
    const vec3 worldUp = vec3{0.0f, 1.0f, 0.0f};
    const vec3 right = cross(look, worldUp);
    const vec3 up = cross(look, right);
    target_ = target_ + (right * dx) + (up * dy);
  }
  //
  void lookAt(vec3 lookAt)
  {
  	target_ = lookAt;
  }
  //
  Camera getCamera() const
  {
  	Camera cam;
  	cam.viewMat = getLookAt();
  	cam.invViewMat = glm::inverse(cam.viewMat);
  }

private:
  mat4 getLookAt() {
    return glm::lookAt(target + toCartesian, target, CamUp);
  }

  vec3 toCartesian() {
    float x = radius_ * sinf(phi_) * sinf(theta_);
    float y = radius_ * cosf(phi_);
    float z = radius_ * sinf(phi_) * cosf(theta_);
    return vec3{x, y, z};
}


  float zoomLevel_;
  float radius_;
  float theta_;
  float phi_;
  vec3 target_;
};

/////////////////////////////////////////////////////
class Trackball {
public:
  // Apply a trackball rotation to an object
  void beginRotate(mat4& modelMat, int screenWidth, int screenHeight, int mouseX,
             int mouseY) {

    if (mouseX != lastMouseX || mouseY != lastMouseY) {
      glm::vec3 va = getArcballVector(lastMouseX, lastMouseY);
      glm::vec3 vb = getArcballVector(mouseX, mouseY);
      float angle = std::acos(std::min(1.0f, glm::dot(va, vb)));
      vec3 axis_in_camera_coord = glm::cross(va, vb);
      mat3 camera2object = glm::inverse(mat3(cam.viewMat) * mat3(modelMat));
      vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
      modelMat = glm::rotate(modelMat, angle, axis_in_object_coord);
      lastMouseX = mouseX;
      lastMouseY = mouseY;
    }
    cam.viewMat = cam.viewMat * modelMat;
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

  mat4 modelMat;
  int lastMouseX;
  int lastMouseY;
};

/////////////////////////////////////////////////////
struct TrackballCamera {
  TrackballCamera(const CameraSettings &init)
      : settings{init}, cam{init}, vEye(init.eye) {}

  mat4 getLookAt() {
    auto lookAt = glm::lookAt(vec3(0, 0, -1), vec3(0, 0, 0), CamUp) *
                  glm::rotate(glm::rotate((float)cameraRotX, CamRight),
                              (float)cameraRotY, CamUp);
    return lookAt;
  }

  void updatePanVectors() {
    auto invLookAt = glm::inverse(getLookAt());
    wCamRight = vec3(invLookAt * vec4(CamRight, 0.0f));
    wCamUp = vec3(invLookAt * vec4(-CamUp, 0.0f));
    wCamFront = vec3(invLookAt * vec4(CamFront, 0.0f));
  }

  void reset() { *this = TrackballCamera{settings}; }

  /**
   * Get a normalized vector from the center of the virtual ball O to a
   * point P on the virtual ball surface, such that P is aligned on
   * screen's (X,Y) coordinates.  If (X,Y) is too far away from the
   * sphere, return the nearest point on the virtual ball surface.
   */

  void onMouseMove(double raw_dx, double raw_dy, int cur_mx, int cur_my,
                   MouseMoveMode mode) {
    auto dx = settings.sensitivity * raw_dx;
    auto dy = settings.sensitivity * raw_dy;

    if (mode == MouseMoveMode::Rotate) {
      auto rot_speed = 0.1;
      auto twopi = glm::pi<double>() * 2.0;
      cameraRotX += std::fmod(rot_speed * dy, twopi);
      cameraRotY += std::fmod(rot_speed * dx, twopi);
      updatePanVectors();
    } else if (mode == MouseMoveMode::RotateScene) {

      // rotate scene around origin
      /*auto rot_speed = 0.1;
      auto twopi = glm::pi<double>() * 2.0;
      sceneRotX += std::fmod(rot_speed * dy, twopi);
      sceneRotY += std::fmod(rot_speed * dx, twopi);
      updatePanVectors();*/

      if (cur_mx != last_mx || cur_my != last_my) {
        glm::vec3 va = get_arcball_vector(last_mx, last_my);
        glm::vec3 vb = get_arcball_vector(cur_mx, cur_my);
        float angle = std::acos(std::min(1.0f, glm::dot(va, vb)));
        glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
        glm::mat3 camera2object =
            glm::inverse(mat3(cam.viewMat) * mat3(modelMat));
        glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
        modelMat = glm::rotate(modelMat, angle, axis_in_object_coord);
        last_mx = cur_mx;
        last_my = cur_my;
      }

    } else if (mode == MouseMoveMode::Pan) {
      auto pan_speed = 0.1;
      vEye += (float)(dx * pan_speed) * wCamRight +
              (float)(dy * pan_speed) * wCamUp;
    } else if (mode == MouseMoveMode::Idle) {
      last_mx = cur_mx;
      last_my = cur_my;
    }
  }

  void onScrollWheel(double delta, ScrollMode scrollMode) {
    auto scroll = delta * settings.sensitivity;
    auto scroll_speed = 10.0;
    if (scrollMode == ScrollMode::MoveForward)
      vEye += (float)(scroll * scroll_speed) * wCamFront;
    else
      zoom *= std::exp2(scroll);
  }

  Camera getCamera() const {
    float aspect_ratio = (float)screen_width / (float)screen_height;
    auto lookAt = getLookAt();
    cam.mode = Camera::Mode::Perspective;
    cam.viewMat = lookAt * glm::translate(vEye) *
                  glm::rotate(glm::rotate((float)sceneRotY, wCamUp),
                              (float)sceneRotX, wCamRight);
    cam.projMat = glm::scale(glm::vec3{(float)zoom, (float)zoom, 1.0f}) *
                  glm::perspective(settings.fieldOfView, aspect_ratio,
                                   settings.nearPlane, settings.farPlane);
    cam.wEye = glm::vec3(glm::inverse(cam.viewMat) *
                         glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    return cam;
  }

private:
  CameraSettings settings;
  Camera cam;
  bool panning = false;
  bool rotating = false;

  double cameraRotX = 0.0;
  double cameraRotY = 0.0;
  double sceneRotX = 0.0;
  double sceneRotY = 0.0;
  double zoom = 1.0;

  int last_mx;
  int last_my;
  int screen_width;
  int screen_height;
  glm::mat4 modelMat;

  glm::vec3 vEye;
  glm::vec3 wCamRight;
  glm::vec3 wCamUp;
  glm::vec3 wCamFront;
};
}