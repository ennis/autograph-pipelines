#include <autograph/gl/Device.h>
#include <autograph/gl/GLHandle.h>
#include <autograph/support/Debug.h>
#include <vector>
#include <algorithm>

namespace ag {
namespace gl {

const char *getGLObjectTypeName(GLenum type) {
  switch (type) {
  case GL_BUFFER:
    return "GL_BUFFER"; // Buffer Object
  case GL_SHADER:
    return "GL_SHADER"; // Shader Object
  case GL_PROGRAM:
    return "GL_PROGRAM"; // Program Object
  case GL_VERTEX_ARRAY:
    return "GL_VERTEX_ARRAY"; // Vertex Array Object
  case GL_QUERY:
    return "GL_QUERY"; // Query Object
  case GL_PROGRAM_PIPELINE:
    return "GL_PROGRAM_PIPELINE"; // Program Pipeline Object
  case GL_TRANSFORM_FEEDBACK:
    return "GL_TRANSFORM_FEEDBACK"; // Transform Feedback Object
  case GL_SAMPLER:
    return "GL_SAMPLER"; // Sampler Object
  case GL_TEXTURE:
    return "GL_TEXTURE"; // Texture Object
  case GL_RENDERBUFFER:
    return "GL_RENDERBUFFER"; // Renderbuffer Object
  case GL_FRAMEBUFFER:
    return "GL_FRAMEBUFFER";
  default:
    return "Unknown";
  }
}

static std::vector<GLObjectTrackingData> GLObjects;

void trackGLObject(GLuint obj, GLenum type) {
  AG_DEBUG("Tracking GL object {}:{}", getGLObjectTypeName(type), obj);
  GLObjectTrackingData data;
  data.type = type;
  data.obj = obj;
  data.creationFrame = getFrameCount();
  GLObjects.push_back(data);
}

void releaseGLObject(GLuint obj, GLenum type) {
  GLObjects.erase(std::remove_if(GLObjects.begin(), GLObjects.end(),
                                 [=](auto &data) {
                                   return data.obj == obj && data.type == type;
                                 }),
                  GLObjects.end());
}

int getGLObjectCount()
{
	return static_cast<int>(GLObjects.size());
}

const GLObjectTrackingData* getGLObjectData(int index)
{
	return &GLObjects[index];
}

}
}
