#pragma once
#include <autograph/Types.h>

namespace ag {
//////////////////////////////////////////////
struct Frustum {
  float left;
  float right;
  float top;
  float bottom;
  // near clip plane position
  float nearPlane;
  // far clip plane position
  float farPlane;
};
//////////////////////////////////////////////
// camera data returned by a camera controller
struct Camera {
  // Projection parameters
  // frustum (for culling)
  Frustum frustum;
  // view matrix
  // (World -> View)
  mat4 viewMat;
  // inverse view matrix
  // (View -> World)
  mat4 invViewMat;
  // projection matrix
  // (View -> clip?)
  mat4 projMat;
  // Eye position in world space (camera center)
  vec3 wEye;
};

//////////////////////////////////////////////
struct CameraSettings {
  vec3 eye{0.0f, 0.0f, 0.0f};
  float fieldOfView{70.0f};
  float nearPlane{0.05f};
  float farPlane{4.0f};
  float sensitivity{0.1f};
  ivec2 screenSize;
};

//////////////////////////////////////////////
// Translates input to camera motion
class AG_API TrackballCameraControl {
public:
  enum class MouseMoveMode { Pan, Rotate, RotateScene, Idle };
  enum class ScrollMode { Zoom, MoveForward };
  enum class RotationMode { Scene, Camera };

  TrackballCameraControl(const CameraSettings &initialSettings = CameraSettings{});

  const Camera &getCamera();

  //void mouseMoveRaw(double raw_dx, double raw_dy, int cur_mx, int cur_my,
  //                  MouseMoveMode mode);
  void cursorMove(ivec2 cursor, RotationMode rotationMode);
  void scrollWheel(double delta, ScrollMode scrollMode);
  void setScreenSize(int width, int height) {
	  screenWidth = width;
	  screenHeight = height;
  }

private:
  void updatePanVectors();

  CameraSettings settings;
  mat4 modelMatrix{1.0f};
  mat4 viewMat;
  ivec2 lastCur;
  Camera cam;
  int screenWidth;
  int screenHeight;
};
}