#include "Bindings.h"

#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
// core
#include <autograph/Camera.h>
#include <autograph/Transform.h>
#include <autograph/Types.h>
// gl
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
// engine
#include "Arcball.h"
#include "CameraControl.h"
#include "Effect.h"
#include "Mesh.h"
#include "Scene.h"
#include "SceneRenderer.h"
// imgui
#include "imgui.h"

namespace ag {

// bindings for standalone shader files
/*sol::table openLuaShaderBindings(sol::this_state s)
{
    sol::state_view lua{s};
    sol::table module = lua.create_table();
}*/

// core bindings
sol::table openLuaBindings(sol::this_state s) {
  sol::state_view lua{s};
  sol::table module = lua.create_table();

  module["getActualPath"] = &ag::getActualPath;
  module["getProjectRootDirectory"] = &ag::getProjectRootDirectory;
  // module["drawMesh"] = &drawMesh;
  // module["get"]

  module["getDefaultFramebuffer"] = &ag::gl::getDefaultFramebuffer;

  module.new_usertype<gl::Texture>(
      "Texture", "create1D", sol::factories(&gl::Texture::create1D), "create2D",
      sol::factories(&gl::Texture::create2D), "width",
      sol::property(&gl::Texture::width), "height",
      sol::property(&gl::Texture::height), "format",
      sol::property(&gl::Texture::format), "object",
      sol::property(&gl::Texture::object), "reset", &gl::Texture::reset);

  module.new_usertype<gl::Buffer>("Buffer", "create",
                                  sol::factories(&gl::Buffer::create));

  // Camera
  module.new_usertype<Frustum>("Frustum");
  module.new_usertype<Camera>("Camera", sol::call_constructor,
                              sol::constructors<sol::types<>>{});

  // Transform
  module.new_usertype<Transform>(
      "Transform", "setPosition",
      [](Transform &t, float x, float y, float z) {
        t.position.x = x;
        t.position.y = y;
        t.position.z = z;
      },
      "position", &Transform::position, "setScale",
      [](Transform &t, float s) { t.scaling = vec3{s}; });

  // Scene
  module.new_usertype<AABB>(
      "AABB", sol::call_constructor,
      [](float xmin, float xmax, float ymin, float ymax, float zmin,
         float zmax) { return AABB{xmin, xmax, ymin, ymax, zmin, zmax}; },
      "xmin", &AABB::xmin, "xmax", &AABB::xmax, "ymin", &AABB::ymin, "ymax",
      &AABB::ymax, "zmin", &AABB::zmin, "zmax", &AABB::zmax, "width",
      &AABB::width, "height", &AABB::height, "depth", &AABB::depth);

  module.new_usertype<Mesh>("Mesh", "loadFromFile",
                            sol::factories(&Mesh::loadFromFile), "AABB",
                            sol::property(&Mesh::getAABB));

  module.new_usertype<SceneObject>(
      "SceneObject", "id", sol::property(&SceneObject::id), "mesh",
      &SceneObject::mesh, 
	  "localTransform", &SceneObject::localTransform,
	  "worldTransform", &SceneObject::worldTransform,
      "getLocalBoundingBox", &SceneObject::getLocalBoundingBox,
      "getApproximateWorldBoundingBox",
      &SceneObject::getApproximateWorldBoundingBox);

  module.new_usertype<Scene>("Scene", sol::call_constructor,
                             sol::constructors<sol::types<>>{}, "addMesh",
                             &Scene::addMesh, "loadModel", &Scene::loadModel);

  // base types
  // issue: vec2.new will bounce back to the c++ side to call a trivial
  // constructor
  // solution: define the constructors on the Lua side
  module.new_usertype<vec2>("vec2", sol::call_constructor,
                            sol::constructors<sol::types<float, float>>{}, "x",
                            &vec2::x, "y", &vec2::y);
  module.new_usertype<vec3>(
      "vec3", sol::call_constructor,
      sol::constructors<sol::types<float, float, float>>{}, "x", &vec3::x, "y",
      &vec3::y, "z", &vec3::z);
  module.new_usertype<vec4>(
      "vec4", sol::call_constructor,
      sol::constructors<sol::types<float, float, float, float>>{}, "x",
      &vec4::x, "y", &vec4::y, "z", &vec4::z, "w", &vec4::w);
  module.new_usertype<ivec2>("ivec2", sol::call_constructor,
                             sol::constructors<sol::types<int, int>>{}, "x",
                             &ivec2::x, "y", &ivec2::y);
  module.new_usertype<ivec3>("ivec3", sol::call_constructor,
                             sol::constructors<sol::types<int, int, int>>{},
                             "x", &ivec3::x, "y", &ivec3::y, "z", &ivec3::z);
  module.new_usertype<ivec4>(
      "ivec4", sol::call_constructor,
      sol::constructors<sol::types<int, int, int, int>>{}, "x", &ivec4::x, "y",
      &ivec4::y, "z", &ivec4::z, "w", &ivec4::w);

  // DrawPass
  module.new_usertype<DrawPassBuilder>(
      "DrawPassBuilder", sol::call_constructor,
      sol::constructors<sol::types<>>{}, "bindTexture",
      [](DrawPassBuilder &dp, int index, gl::Texture &tex) {
        dp.bindTexture(index, tex.object());
      },
      "bindTextureImage",
      [](DrawPassBuilder &dp, int index, gl::Texture &tex) {
        dp.bindTextureImage(index, tex.object());
      },
      "bindSampler",
      [](DrawPassBuilder &dp, int index, gl::Sampler &sampler) {
        dp.bindSampler(index, sampler.object());
      },
      //"bindUniformBuffer", [](DrawPassBuilder& dp, int index, gl::Sampler&
      //sampler) { dp.bindSampler(index, sampler.object()); },
      //"bindShaderStorageBuffer", &DrawPassBuilder::bindShaderStorageBuffer,
      "setColorBuffer",
      [](DrawPassBuilder &dp, int index, gl::Texture &tex) {
        dp.bindColorBuffer(index, tex.object());
      },
      "setDepthBuffer",
      [](DrawPassBuilder &dp, gl::Texture *tex) {
        if (tex)
          dp.bindDepthBuffer(tex->object());
        else
          dp.bindDepthBuffer(0);
      },
      "setVertexShader", &DrawPassBuilder::setVertexShader, "setFragmentShader",
      &DrawPassBuilder::setFragmentShader, "setBlendState",
      [](DrawPassBuilder &dp, int index, sol::table table) {
        gl::BlendState bs;
        bs.enabled = table.get_or("enabled", false);
        bs.modeRGB = table.get_or("modeRGB", GL_FUNC_ADD);
        bs.modeAlpha = table.get_or("modeAlpha", GL_FUNC_ADD);
        bs.funcSrcRGB = table.get_or("funcSrcRGB", GL_SRC_ALPHA);
        bs.funcDstRGB = table.get_or("funcDstRGB", GL_ONE_MINUS_SRC_ALPHA);
        bs.funcSrcAlpha = table.get_or("funcSrcAlpha", GL_ONE);
        bs.funcDstAlpha = table.get_or("funcDstAlpha", GL_ZERO);
        dp.setBlendState(index, bs);
      },
      "setRasterizerState",
      [](DrawPassBuilder &dp, sol::table table) {
        gl::RasterizerState rs;
        rs.fillMode = table.get_or("fillMode", GL_FILL);
        rs.cullMode = table.get_or("cullMode", GL_NONE);
        rs.frontFace = table.get_or("frontFace", GL_CCW);
        rs.depthBias = table.get_or("depthBias", 1.0f);
        rs.slopeScaledDepthBias = table.get_or("slopeScaledDepthBias", 1.0f);
        rs.depthClipEnable = table.get_or("depthClipEnable", false);
        rs.scissorEnable = table.get_or("scissorEnable", false);
        dp.setRasterizerState(rs);
      },
      "setDepthStencilState",
      [](DrawPassBuilder &dp, sol::table table) {
        gl::DepthStencilState dss;
        dss.depthTestEnable = table.get_or("depthTestEnable", false);
        dss.depthWriteEnable = table.get_or("depthWriteEnable", false);
        dss.stencilEnable = table.get_or("stencilEnable", false);
        dss.stencilFace = table.get_or("stencilFace", GL_FRONT_AND_BACK);
        dss.stencilFunc = table.get_or("stencilFunc", 0);
        dss.stencilRef = table.get_or("stencilRef", 0);
        dss.stencilMask = table.get_or("stencilMask", 0xFFFFFFFF);
        dss.stencilOpSfail = table.get_or("stencilOpSfail", 0);
        dss.stencilOpDPFail = table.get_or("stencilOpDPFail", 0);
        dss.stencilOpDPPass = table.get_or("stencilOpDPPass", 0);
        dp.setDepthStencilState(dss);
      },
      "setViewport", &DrawPassBuilder::setViewport, "makeDrawPass",
      &DrawPassBuilder::makeDrawPass);

  module.new_usertype<DrawPass>("DrawPass");

  // stuff
  module["debugMessage"] = [](const char *str) {
    rawLogMessage(LogLevel::Debug, str);
  };
  module["warningMessage"] = [](const char *str) {
    rawLogMessage(LogLevel::Warning, str);
  };
  module["errorMessage"] = [](const char *str) {
    rawLogMessage(LogLevel::Error, str);
  };

  // ShaderManager
  module.new_usertype<ShaderManager>("ShaderManager");

  // DeferredSceneRenderer
  module.new_usertype<DeferredSceneRenderer>(
      "DeferredSceneRenderer", sol::call_constructor,
      sol::constructors<sol::types<ShaderManager &>>(), "renderScene",
      &DeferredSceneRenderer::renderScene);
  module.new_usertype<DeferredSceneRenderer::GBuffer>(
      "DeferredGBuffer", sol::call_constructor,
      sol::constructors<sol::types<int, int>>());

  module.new_usertype<WireframeOverlayRenderer>(
      "WireframeOverlayRenderer", sol::call_constructor,
      sol::constructors<sol::types<ShaderManager &>>(), "renderSceneObject",
      &WireframeOverlayRenderer::renderSceneObject);

  // CameraControl
  module.new_usertype<CameraControl>(
      "CameraControl", sol::call_constructor, sol::constructors<sol::types<>>(),
      "zoomIn", &CameraControl::zoomIn, "setZoom", &CameraControl::setZoom,
      "rotate", &CameraControl::rotate, "pan", &CameraControl::pan, "lookAt",
      static_cast<void (CameraControl::*)(float, float, float)>(
          &CameraControl::lookAt),
      "setAspectRatio", &CameraControl::setAspectRatio, "setFieldOfView",
      &CameraControl::setFieldOfView, "setNearFarPlanes",
      &CameraControl::setNearFarPlanes, "getCamera", &CameraControl::getCamera,
      "lookDistance", &CameraControl::lookDistance);

  // imgui bindings
  /* module["imgui_BeginMainMenuBar"] = &ImGui::BeginMainMenuBar;
   module["imgui_BeginMenu"] = &ImGui::BeginMenu;
   module["imgui_MenuItem"] = static_cast<bool(*)(const char*, const char*,
   bool, bool)>(&ImGui::MenuItem);
   module["imgui_EndMenu"] = &ImGui::EndMenu;
   module["imgui_EndMainMenuBar"] = &ImGui::EndMainMenuBar;
   module["imgui_BeginDock"] = [](const char* label) { return
   ImGui::BeginDock(label); };
   module["imgui_EndDock"] = &ImGui::EndDock;*/

  return module;
}
}
