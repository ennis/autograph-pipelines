#include <algorithm>
#include <autograph/gl/Device.h>
#include <autograph/gl/GLHandle.h>
#include <autograph/support/Debug.h>
#include <vector>

namespace ag {

const char *getGLObjectTypeName(gl::GLenum type) {
  switch (type) {
  case gl::BUFFER:
    return "BUFFER"; // Buffer Object
  case gl::SHADER:
    return "SHADER"; // Shader Object
  case gl::PROGRAM:
    return "PROGRAM"; // Program Object
  case gl::VERTEX_ARRAY:
    return "VERTEX_ARRAY"; // Vertex Array Object
  case gl::QUERY:
    return "QUERY"; // Query Object
  case gl::PROGRAM_PIPELINE:
    return "PROGRAM_PIPELINE"; // Program Pipeline Object
  case gl::TRANSFORM_FEEDBACK:
    return "TRANSFORM_FEEDBACK"; // Transform Feedback Object
  case gl::SAMPLER:
    return "SAMPLER"; // Sampler Object
  case gl::TEXTURE:
    return "TEXTURE"; // Texture Object
  case gl::RENDERBUFFER:
    return "RENDERBUFFER"; // Renderbuffer Object
  case gl::FRAMEBUFFER:
    return "FRAMEBUFFER";
  default:
    return "Unknown";
  }
}

static std::vector<GLObjectTrackingData> GLObjects;

void trackGLObject(gl::GLuint obj, gl::GLenum type) {
  // AG_DEBUG("Tracking GL object {}:{}", getGLObjectTypeName(type), obj);
  GLObjectTrackingData data;
  data.type = type;
  data.obj = obj;
  data.creationFrame = getFrameCount();
  GLObjects.push_back(data);
}

void releaseGLObject(gl::GLuint obj, gl::GLenum type) {
  GLObjects.erase(std::remove_if(GLObjects.begin(), GLObjects.end(),
                                 [=](auto &data) {
                                   return data.obj == obj && data.type == type;
                                 }),
                  GLObjects.end());
}

int getGLObjectCount() { return static_cast<int>(GLObjects.size()); }

const GLObjectTrackingData *getGLObjectData(int index) {
  return &GLObjects[index];
}

} // namespace ag
