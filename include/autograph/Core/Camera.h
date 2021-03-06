#pragma once
#include <autograph/Core/Types.h>

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

}