#include "CameraControl/Exports.h"
#include <autograph/Editor/CameraController.h>
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace ag;

//////////////////////////////////////////////
class CameraController : public ag::CameraController {
public:
  void zoomIn(float dzoom) { zoomLevel_ += dzoom; }

  void setZoom(float zoom) { zoomLevel_ = zoom; }

  //
  void rotate(float dTheta, float dPhi) {
    theta_ += dTheta;
    phi_ += dPhi;
  }

  // dx, dy in clip space
  void pan(float dx, float dy, float dz) 
  {
	// dx, dy to camera space
	float panFactor = radius_;
    const vec3 look = glm::normalize(toCartesian());
    const vec3 worldUp = vec3{0.0f, 1.0f, 0.0f};
    const vec3 right = glm::cross(look, worldUp);
    const vec3 up = glm::cross(look, right);
	
	target_ = target_ + (right * dx * panFactor) + (up * dy * panFactor);
	radius_ -= dz*radius_;
  }

  //
  void lookAt(vec3 lookAt) { target_ = lookAt; }
  void lookAt(float x, float y, float z) { target_ = vec3{x, y, z}; }
  void lookDistance(float lookDist) { radius_ = lookDist; }
  void setAspectRatio(float aspect_ratio) { aspectRatio_ = aspect_ratio; }
  void setFieldOfView(float fov) { fov_ = fov; }

  void setNearFarPlanes(float nearPlane, float farPlane) {
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;
  }

  //
  Camera getCamera() const {
    Camera cam;
    cam.viewMat = getLookAt();
    cam.invViewMat = glm::inverse(cam.viewMat);
    cam.projMat = glm::scale(vec3{zoomLevel_, zoomLevel_, 1.0f}) *
                  glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_,
                                   farPlane_);
    cam.wEye = vec3(glm::inverse(cam.viewMat) * vec4{0.0f, 0.0f, 0.0f, 1.0f});
    return cam;
  }

  mat4 getLookAt() const {
    return glm::lookAt(target_ + toCartesian(), target_, CamUp) *
           glm::toMat4(arcballRotation * tmpArcballRotation);
  }

  vec3 toCartesian() const {
    float x = radius_ * sinf(phi_) * sinf(theta_);
    float y = radius_ * cosf(phi_);
    float z = radius_ * sinf(phi_) * cosf(theta_);
    return vec3{x, y, z};
  }

  void centerOnObject(const AABB &objectBounds) {
    auto size = std::max(
        {objectBounds.width(), objectBounds.height(), objectBounds.depth()});
    auto cx = (objectBounds.xmax + objectBounds.xmin) / 2.f;
    auto cy = (objectBounds.ymax + objectBounds.ymin) / 2.f;
    auto cz = (objectBounds.zmax + objectBounds.zmin) / 2.f;
    const float fov = 45.0f;
    float camDist = (0.5f * size) / std::tan(0.5f * glm::radians(fov));
    lookAt(cx, cy, cz);
    lookDistance(camDist);
    setNearFarPlanes(0.1f * camDist, 10.0f * camDist);
    setFieldOfView(fov);
	arcballRotation = quat{};
    AG_DEBUG("near {} far {}", 0.5f * camDist, 2.0f * camDist);
  }

  void rotateArcball(const mat4 &objectToWorld, int screenWidth,
                     int screenHeight, int mouseX, int mouseY) {
    auto viewMat = getLookAt();
    if (mouseX != mouseDownX || mouseY != mouseDownY) {
      glm::vec3 va =
          getArcballVector(screenWidth, screenHeight, mouseDownX, mouseDownY);
      glm::vec3 vb =
          getArcballVector(screenWidth, screenHeight, mouseX, mouseY);
      /*float angle = std::acos(glm::min(1.0f, glm::dot(va, vb)));
      vec3 axis_in_camera_coord = glm::cross(va, vb);
      mat3 camera2world =
          glm::inverse(mat3(viewMat));
      vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;*/
      tmpArcballRotation = glm::rotation(va, vb);
	}
	else {
		// Commit rotation
		arcballRotation *= tmpArcballRotation;
		tmpArcballRotation = quat{};
	}
  }

  vec3 getArcballVector(int sw, int sh, int x, int y) const {
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

  enum class CameraMode { Idle, Panning, Rotating };

  virtual bool onCameraGUI(int mouseX, int mouseY, int screenW, int screenH,
                           Camera &inOutCam, Scene &scene,
                           ID selectedObject) override {
    bool handled = false;
    // CTRL and SHIFT
    bool ctrl_down = ImGui::IsKeyDown(KEY_LEFT_CONTROL) ||
                     ImGui::IsKeyDown(KEY_RIGHT_CONTROL);
    bool shift_down = ImGui::IsKeyDown(KEY_LEFT_SHIFT) ||
                      ImGui::IsKeyDown(KEY_RIGHT_SHIFT);

    setAspectRatio((float)screenW / (float)screenH);

	auto sceneObjectComponents =
		scene.getComponentManager<SceneObjectComponents>();
	auto selectedObj = sceneObjectComponents->get(selectedObject);

    // Camera focus on object
    if (!ctrl_down && ImGui::IsKeyDown(KEY_Z)) {
      AG_DEBUG("Camera focus on {}", selectedObject);
      if (selectedObj)
        focusOnObject(scene, *selectedObj);
      handled = true;
    }

    // Must hold CTRL for camera
    else if (!ctrl_down) {
      handled = false;
    } else {
      // Camera state machine
      if (ImGui::IsMouseDown(0) && mode != CameraMode::Rotating) {
        lastMouseX = mouseDownX = mouseX;
        lastMouseY = mouseDownY = mouseY;
        mode = CameraMode::Rotating;
      } else if (ImGui::IsMouseDown(2) && mode != CameraMode::Panning) {
        lastMouseX = mouseDownX = mouseX;
        lastMouseY = mouseDownY = mouseY;
        mode = CameraMode::Panning;
      } else if (!ImGui::IsMouseDown(0) && !ImGui::IsMouseDown(2)) {
        mode = CameraMode::Idle;
      }

      // Delta w.r.t. last frame
      auto mouseDeltaX = (float)(mouseX - lastMouseX);
      auto mouseDeltaY = (float)(mouseY - lastMouseY);
      // Delta w.r.t. last click
      auto mouseDragVecX = (float)(mouseX - mouseDownX);
      auto mouseDragVecY = (float)(mouseY - mouseDownY);

      const auto panFactor = 1.0f / screenW;
      const auto zoomFactor = 0.01f;
      const auto rotateFactor = 0.001f;

      // Rotating & panning
      if (mode == CameraMode::Rotating) {
        if (shift_down) {
          // Shift down => arcball rotation around currently selected object
			if (selectedObj)
			{
				AG_DEBUG("Camera rotating {},{} (arcball around object)", mouseDeltaX,
					mouseDeltaY);
				rotateArcball(selectedObj->worldTransform, screenW, screenH, mouseX, mouseY);
			}
        } else {
          AG_DEBUG("Camera rotating {},{} (camera orientation)", mouseDeltaX,
                   mouseDeltaY);
          rotate(-mouseDeltaX * rotateFactor, -mouseDeltaY * rotateFactor );
        }
      } else if (mode == CameraMode::Panning) {
        AG_DEBUG("Camera panning {},{}", mouseDeltaX, mouseDeltaY);
        pan(mouseDeltaX * panFactor, -mouseDeltaY * panFactor * aspectRatio_, 0.0f);
      }

      // Scrolling
      float scroll = ImGui::GetIO().MouseWheel;
      if (scroll != 0.0f) {
        AG_DEBUG("Camera scrolling {}", scroll);
        pan(0.0f, 0.0f, scroll * zoomFactor);
      }

      ImGui::ResetMouseDragDelta(0);
      ImGui::ResetMouseDragDelta(2);
      lastMouseX = mouseX;
      lastMouseY = mouseY;
      handled = true;
    }

    inOutCam = getCamera();
    return handled;
  }

  virtual void focusOnObject(Scene &scene,
                             const SceneObject &sceneObject) override {
    centerOnObject(sceneObject.getApproximateWorldBounds());
  }

private:
  float fov_{45.0f};
  float aspectRatio_{1.0f}; // should be screenWidth / screenHeight
  float nearPlane_{0.001f};
  float farPlane_{10.0f};
  float zoomLevel_{1.0f};
  float radius_{1.0f};
  float theta_{0.0f};
  float phi_{kPiOverTwo<float>};
  vec3 target_{0.0f, 0.0f, 0.0f};
  quat tmpArcballRotation = quat{};
  quat arcballRotation = quat{};
  int mouseDownX = 0;
  int mouseDownY = 0;
  int lastMouseX = 0;
  int lastMouseY = 0;
  CameraMode mode;
};

//////////////////////////////////////////////
CameraControl_PLUGIN_ENTRY {
  ag::registerClass<::CameraController, ag::CameraController>(
      "CameraController");
}
