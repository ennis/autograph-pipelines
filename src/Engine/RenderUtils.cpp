#include <autograph/Engine/RenderUtils.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Engine/ShaderUtils.h>
#include <autograph/Gfx/Draw.h>
#include <autograph/Gfx/UploadBuffer.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/packing.hpp>

namespace ag {
namespace RenderUtils {
struct RenderUtilsState {
  RenderUtilsState() {
    reloadShaders();
    SamplerDesc desc;
    desc.addrU = gl::REPEAT;
    desc.addrV = gl::REPEAT;
    desc.addrW = gl::REPEAT;
    desc.minFilter = gl::NEAREST;
    desc.magFilter = gl::NEAREST;
    samplerNearest = Sampler{desc};
    desc.minFilter = gl::LINEAR;
    desc.magFilter = gl::LINEAR;
    samplerLinear = Sampler{desc};
  }

  void reloadShaders() {
    drawSpriteShader = Shader{"shaders/default:drawSprite"};
    drawMeshShader = Shader{"shaders/default:drawMeshDefault"};
    drawWireMeshShader = Shader{"shaders/default:drawWireMesh"};
    drawWireMeshNoDepthShader = Shader{"shaders/default:drawWireMeshNoDepth"};
    drawWireMesh2DColorShader = Shader{"shaders/default:drawWireMesh2DColor"};
  }

  Sampler samplerNearest;
  Sampler samplerLinear;
  Shader drawSpriteShader;
  Shader drawMeshShader;
  Shader drawWireMeshShader;
  Shader drawWireMeshNoDepthShader;
  Shader drawWireMesh2DColorShader;
};

static RenderUtilsState &getRenderUtilsState() {
  static RenderUtilsState state;
  return state;
}

// Draw mesh with default view-dependent shading
void drawMesh(Framebuffer &target, const Camera &cam, Mesh3D &mesh, vec3 pos,
              float scale, vec4 color) {
  auto m = glm::translate(glm::scale(glm::mat4{1.0f}, vec3{scale}), pos);
  drawMesh(target, cam, mesh, m, color);
}

void drawMesh(Framebuffer &target, const Camera &cam, Mesh3D &mesh,
              mat4 modelTransform, vec4 color) {
  CameraUniforms camUniforms{cam};
  auto &state = getRenderUtilsState();
  draw(target, mesh, state.drawMeshShader,
       bind::uniformFrameData(0, &camUniforms),
       bind::uniform_mat4("uModelMatrix", modelTransform),
       bind::uniform_vec4("uColor", color));
}

// Draw mesh in wireframe
void drawWireMesh(Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                  vec3 pos, float scale, vec4 wireColor) {
  auto m = glm::translate(glm::scale(glm::mat4{1.0f}, vec3{scale}), pos);
  drawMesh(target, cam, mesh, m, wireColor);
}

void drawWireMesh(Framebuffer &target, const Camera &cam, Mesh3D &mesh,
                  mat4 modelTransform, vec4 wireColor) {
  CameraUniforms camUniforms{cam};
  auto &state = getRenderUtilsState();
  draw(target, mesh, state.drawWireMeshNoDepthShader,
       bind::uniformFrameData(0, &camUniforms),
       bind::uniform_mat4("uModelMatrix", modelTransform),
       bind::uniform_vec4("uWireColor", wireColor));
}

void drawLines(Framebuffer &target, const Camera &cam,
               span<const Vertex3D> lines, mat4 modelTransform, float lineWidth,
               vec4 wireColor) {
  CameraUniforms camUniforms{cam};
  auto vbuf = uploadFrameArray(lines.data(), lines.size());
  auto &state = getRenderUtilsState();
  // gl::LineWidth(lineWidth);
  draw(target, drawArrays(gl::LINES, 0, (uint32_t)lines.size()),
       state.drawWireMeshNoDepthShader, bind::uniformFrameData(0, &camUniforms),
       bind::uniform_mat4("uModelMatrix", modelTransform),
       bind::uniform_vec4("uWireColor", wireColor),
       bind::vertexBuffer(0, vbuf, sizeof(Vertex3D)));
}

// Draw a bounding box
void drawBoundingBox(Framebuffer &target, const Camera &cam, const AABB &aabb,
                     vec4 wireColor) {
  Vertex3D lines[24] = {
      {vec3{aabb.xmin, aabb.ymin, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmin, aabb.ymin, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymin, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmin, aabb.ymax, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymin, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymin, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmax, aabb.ymax, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymax, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmax, aabb.ymin, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymax, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymax, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymax, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymax, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmin, aabb.ymax, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymin, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmin, aabb.ymax, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymax, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymax, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmax, aabb.ymin, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymin, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmax, aabb.ymin, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymax, aabb.zmin}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},

      {vec3{aabb.xmin, aabb.ymin, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
      {vec3{aabb.xmax, aabb.ymin, aabb.zmax}, vec3{0.0f}, vec3{0.0f},
       vec2{0.0f}},
  };
  drawLines(target, cam, lines, mat4{1.0f}, 1.0f, wireColor);
}

void drawSprite(Framebuffer &target, float targetX0, float targetY0,
                float targetX1, float targetY1, Texture &src, float srcX0,
                float srcY0, float srcX1, float srcY1) {
  drawSprite(target, targetX0, targetY0, targetX1, targetY1, src.object(),
             srcX0, srcY0, srcX1, srcY1);
}

void drawSprite(Framebuffer &target, float targetX0, float targetY0,
                float targetX1, float targetY1, gl::GLuint tex, float srcX0,
                float srcY0, float srcX1, float srcY1) {
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

  auto vbuf = ag::uploadFrameArray(quad);

  auto &state = getRenderUtilsState();
  draw(target, drawArrays(gl::TRIANGLES, 0, 6), state.drawSpriteShader,
       bind::texture(0, tex, state.samplerNearest.object()),
       bind::vertexBuffer(0, vbuf, sizeof(Vertex2DTex)));
}

void drawTexturedQuad(Framebuffer &target, Texture &src) {
  drawSprite(target, 0.0f, 0.0f, (float)target.width(), (float)target.height(),
             src.object());
}

void drawGrid2D(Framebuffer &target,
                vec2 center,  // screen coords
                vec2 spacing, // screen coords
                int primaryLinesEveryN, vec4 axisColorX, vec4 axisColorY,
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
  int numVLines = (int)(w / spacing.x + 1);
  int numHLines = (int)(h / spacing.y + 1);
  int numVertices = 2 * (2 + numVLines + numHLines);
  std::vector<Vertex2DColor> lines(numVertices);

  float xoff = fmod(center.x, spacing.x);
  float yoff = fmod(center.y, spacing.y);
  int firstHLine = (int)(-center.y / spacing.y);
  int firstVLine = (int)(-center.x / spacing.x);

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
    bool primary = ((firstHLine + i) % primaryLinesEveryN == 0);
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

  auto &state = getRenderUtilsState();
  auto vbuf = uploadFrameArray(lines.data(), lines.size());
  draw(target, drawArrays(gl::LINES, 0, numVertices),
       state.drawWireMesh2DColorShader,
       bind::vertexBuffer(0, vbuf, sizeof(Vertex2DColor)));
}

Sampler &getLinearSampler() { return getRenderUtilsState().samplerLinear; }

Sampler &getNearestSampler() { return getRenderUtilsState().samplerNearest; }
} // namespace RenderUtils
} // namespace ag