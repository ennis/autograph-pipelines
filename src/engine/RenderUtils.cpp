#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/ShaderUtils.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/packing.hpp>

namespace ag {
RenderUtils::RenderUtils() {
  reloadShaders();
  gl::SamplerDesc desc;
  desc.addrU = GL_CLAMP_TO_EDGE;
  desc.addrV = GL_CLAMP_TO_EDGE;
  desc.addrW = GL_CLAMP_TO_EDGE;
  desc.minFilter = GL_NEAREST;
  desc.magFilter = GL_NEAREST;
  samplerNearest = gl::Sampler{desc};
}

void RenderUtils::reloadShaders() {
  drawSpriteShader = Shader{"shaders/default:drawSprite"};
  drawMeshShader = Shader{"shaders/default:drawMeshDefault"};
  drawWireMeshShader = Shader{"shaders/default:drawWireMesh"};
  drawWireMeshNoDepthShader = Shader{"shaders/default:drawWireMeshNoDepth"};
  drawWireMesh2DColorShader = Shader{ "shaders/default:drawWireMesh2DColor" };
}

// Draw mesh with default view-dependent shading
void RenderUtils::drawMesh(gl::Framebuffer &target, const Camera &cam,
                           Mesh3D &mesh, vec3 pos, float scale, vec4 color) {
  auto m = glm::translate(glm::scale(glm::mat4{1.0f}, vec3{scale}), pos);
  drawMesh(target, cam, mesh, m, color);
}

void RenderUtils::drawMesh(gl::Framebuffer &target, const Camera &cam,
                           Mesh3D &mesh, mat4 modelTransform, vec4 color) {
  CameraUniforms camUniforms{cam};
  using namespace gl;
  using namespace gl::bind;
  draw(target, mesh, drawMeshShader, uniformFrameData(0, &camUniforms),
       uniform_mat4("uModelMatrix", modelTransform),
       uniform_vec4("uColor", color));
}

// Draw mesh in wireframe
void RenderUtils::drawWireMesh(gl::Framebuffer &target, const Camera &cam,
                               Mesh3D &mesh, vec3 pos, float scale,
                               vec4 wireColor) {
  auto m = glm::translate(glm::scale(glm::mat4{1.0f}, vec3{scale}), pos);
  drawMesh(target, cam, mesh, m, wireColor);
}

void RenderUtils::drawWireMesh(gl::Framebuffer &target, const Camera &cam,
                               Mesh3D &mesh, mat4 modelTransform,
                               vec4 wireColor) {
  CameraUniforms camUniforms{cam};
  using namespace gl;
  using namespace gl::bind;
  draw(target, mesh, drawWireMeshNoDepthShader,
       uniformFrameData(0, &camUniforms),
       uniform_mat4("uModelMatrix", modelTransform),
       uniform_vec4("uWireColor", wireColor));
}

// Draw a bounding box
void RenderUtils::drawBoundingBox(gl::Framebuffer &target, const Camera &cam,
                                  const AABB &aabb, vec4 wireColor) {}

void RenderUtils::drawSprite(gl::Framebuffer &target, float targetX0,
                             float targetY0, float targetX1, float targetY1,
                             gl::Texture &src, float srcX0, float srcY0,
                             float srcX1, float srcY1) {
  drawSprite(target, targetX0, targetY0, targetX1, targetY1, src.object(),
             srcX0, srcY0, srcX1, srcY1);
}

void RenderUtils::drawSprite(gl::Framebuffer &target, float targetX0,
                             float targetY0, float targetX1, float targetY1,
                             GLuint tex, float srcX0, float srcY0, float srcX1,
                             float srcY1) {
  float w = (float)target.width();
  float h = (float)target.height();

  Vertex2DTex quad[6] = {
      {vec2{2.0f * targetX0 / w - 1.0f, 2.0f * targetY0 / h - 1.0f},
       vec2{srcX0, srcY0}},
      {vec2{2.0f * targetX1 / w - 1.0f, 2.0f * targetY0 / h - 1.0f},
       vec2{srcX1, srcY0}},
      {vec2{2.0f * targetX0 / w - 1.0f, 2.0f * targetY1 / h - 1.0f},
       vec2{srcX0, srcY1}},
      {vec2{2.0f * targetX0 / w - 1.0f, 2.0f * targetY1 / h - 1.0f},
       vec2{srcX0, srcY1}},
      {vec2{2.0f * targetX1 / w - 1.0f, 2.0f * targetY0 / h - 1.0f},
       vec2{srcX1, srcY0}},
      {vec2{2.0f * targetX1 / w - 1.0f, 2.0f * targetY1 / h - 1.0f},
       vec2{srcX1, srcY1}}};

  auto vbuf = ag::gl::uploadFrameArray(quad);

  using namespace gl;
  using namespace gl::bind;

  draw(target, drawArrays(GL_TRIANGLES, 0, 6), drawSpriteShader,
       texture(0, tex, samplerNearest.object()),
       vertexBuffer(0, vbuf, sizeof(Vertex2D)));
}

void RenderUtils::drawGrid2D(gl::Framebuffer &target,
                             vec2 center,  // screen coords
                             vec2 spacing, // screen coords
                             int primaryLinesEveryN, vec4 axisColorX,
                             vec4 axisColorY, 
							vec4 primaryLinesColor, vec4 secondaryLinesColor) {
  struct Vertex2DColor {
    vec2 pos;
    uint32_t color;
  };

  float w = (float)target.width();
  float h = (float)target.height();

  auto ndc = [&](float x, float y) {
    return vec2{2.0f * x / w - 1.0f, 2.0f * y / h - 1.0f};
  };

  //
  int numVLines = w / spacing.x + 1;
  int numHLines = h / spacing.y + 1;
  int numVertices = 2 * (2 + numVLines + numHLines);
  std::vector<Vertex2DColor> lines(numVertices);

  float xoff = fmod(center.x, spacing.x);
  float yoff = fmod(center.y, spacing.y);
  int firstHLine = -center.y / spacing.y;
  int firstVLine = -center.x / spacing.x;

  auto primaryColorPacked = glm::packUnorm4x8(primaryLinesColor);
  auto secondaryColorPacked = glm::packUnorm4x8(secondaryLinesColor);

  for (int i = 0; i < numVLines; ++i) {
    int idx = 2 * i;
    bool primary = ((firstVLine + i) % primaryLinesEveryN == 0);
	float x = i * spacing.x + xoff;
    lines[idx].pos = ndc(x, 0);
    lines[idx].color = primary ? primaryColorPacked : secondaryColorPacked;
    lines[idx + 1].pos = ndc(x, h);
    lines[idx + 1].color = primary ? primaryColorPacked : secondaryColorPacked;
  }

  for (int i = 0; i < numHLines; ++i) {
    int idx = 2 * (numVLines + i);
    bool primary = ((firstHLine+i) % primaryLinesEveryN == 0);
	float y = i * spacing.y + yoff;
    lines[idx].pos = ndc(0, y);
    lines[idx].color = primary ? primaryColorPacked : secondaryColorPacked;
    lines[idx + 1].pos = ndc(w, y);
    lines[idx + 1].color = primary ? primaryColorPacked : secondaryColorPacked;
  }

  int idx = 2 * (numVLines + numHLines);
  auto axisColorXPacked = glm::packUnorm4x8(axisColorX);
  auto axisColorYPacked = glm::packUnorm4x8(axisColorY);
  lines[idx].pos = ndc(0, center.y);
  lines[idx].color = axisColorXPacked;
  lines[idx + 1].pos = ndc(w, center.y);
  lines[idx + 1].color = axisColorXPacked;
  lines[idx + 2].pos = ndc(center.x, 0);
  lines[idx + 2].color = axisColorYPacked;
  lines[idx + 3].pos = ndc(center.x, h);
  lines[idx + 3].color = axisColorYPacked;

  auto vbuf = gl::uploadFrameArray(lines.data(), lines.size());
  draw(target, gl::drawArrays(GL_LINES, 0, numVertices), drawWireMesh2DColorShader,
	  gl::bind::vertexBuffer(0, vbuf, sizeof(Vertex2DColor)));
}

AG_API RenderUtils &getRenderUtils() {
  static RenderUtils instance;
  return instance;
}
}