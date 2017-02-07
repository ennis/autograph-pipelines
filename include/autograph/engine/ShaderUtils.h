#pragma once
// TODO maybe rename this to 'ShaderCommon.h' or something more explicit
#include <autograph/Camera.h>
#include <autograph/Rect.h>

namespace ag {
struct CameraPerObjectUniforms {
  CameraPerObjectUniforms() = default;
  CameraPerObjectUniforms(const Camera &cam,
	  const mat4 &modelMatrix_ = mat4{ 1.0f })
      : modelMatrix{modelMatrix_}, viewMatrix{cam.viewMat}, projMatrix{
                                                                cam.projMat} {
    viewProjMatrix = projMatrix * viewMatrix;
  }

  mat4 modelMatrix;
  mat4 viewMatrix;
  mat4 projMatrix;
  mat4 viewProjMatrix;
};

struct CameraUniforms {
  CameraUniforms() = default;
  CameraUniforms(const Camera &cam)
      : viewMatrix{cam.viewMat}, projMatrix{cam.projMat} {
    viewProjMatrix = projMatrix * viewMatrix;
  }
  mat4 viewMatrix;
  mat4 projMatrix;
  mat4 viewProjMatrix;
};
}