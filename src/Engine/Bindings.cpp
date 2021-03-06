#include "Bindings.h"

#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/ProjectRoot.h>
// core
#include <autograph/Core/Camera.h>
#include <autograph/Core/Transform.h>
#include <autograph/Core/Types.h>
// gl
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Draw.h>
#include <autograph/Gfx/Framebuffer.h>
#include <autograph/Gfx/Program.h>
#include <autograph/Gfx/Texture.h>
// engine
#include <autograph/Engine/Arcball.h>
#include <autograph/Engine/ImageUtils.h>
#include <autograph/Engine/Mesh.h>
#include <autograph/Engine/RenderTarget.h>
#include <autograph/Engine/RenderUtils.h>
#include <autograph/Engine/SceneObject.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Engine/Window.h>
#include <autograph/Engine/MathUtils.h>
// imgui
//#include "imgui.h"

namespace ag {
namespace detail {

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

  /*module.new_usertype<Texture>(
      "Texture", "create1D",
      sol::factories([](ImageFormat fmt, int w, int numMips) {
        return Texture::create1D(fmt, w, Texture::MipMaps{numMips});
      }),
      "create2D",
      sol::factories([](ImageFormat fmt, int w, int h, int numMips) {
        return Texture::create2D(fmt, w, h, Texture::MipMaps{numMips});
      }),
      "create2DMultisample",
      sol::factories([](ImageFormat fmt, int w, int h, int samples) {
        return Texture::create2DMultisample(fmt, w, h,
                                                Texture::Samples{samples});
      }),
      "width", sol::property(&Texture::width), "height",
      sol::property(&Texture::height), "format",
      sol::property(&Texture::format), "object",
      sol::property(&Texture::object), "reset", &Texture::reset);

  module.new_usertype<Buffer>("Buffer", sol::call_constructor, sol::constructors<sol::types<size_t, BufferUsage>>{});*/

  // Camera
 /* module.new_usertype<Frustum>("Frustum");
  module.new_usertype<Camera>("Camera", sol::call_constructor,
                              sol::constructors<sol::types<>>{});*/

  // Transform
  /*module.new_usertype<Transform>(
      "Transform", "setPosition",
      [](Transform &t, float x, float y, float z) {
        t.position.x = x;
        t.position.y = y;
        t.position.z = z;
      },
      "position", &Transform::position, "setScale",
      [](Transform &t, float s) { t.scaling = vec3{s}; });*/

  // Scene
  /*module.new_usertype<AABB>(
      "AABB", sol::call_constructor,
      [](float xmin, float xmax, float ymin, float ymax, float zmin,
         float zmax) { return AABB{xmin, xmax, ymin, ymax, zmin, zmax}; },
      "xmin", &AABB::xmin, "xmax", &AABB::xmax, "ymin", &AABB::ymin, "ymax",
      &AABB::ymax, "zmin", &AABB::zmin, "zmax", &AABB::zmax, "width",
      &AABB::width, "height", &AABB::height, "depth", &AABB::depth);*/

  //module.new_usertype<Mesh3D>("Mesh3D");

  /*module.new_usertype<SceneObject>(
      "SceneObject", "id", sol::property(&SceneObject::id), "mesh",
      &SceneObject::mesh, "localTransform", &SceneObject::localTransform,
      "worldTransform", &SceneObject::worldTransform, "getLocalBoundingBox",
      &SceneObject::getLocalBoundingBox, "getApproximateWorldBoundingBox",
      &SceneObject::getApproximateWorldBoundingBox);*/

/*  module.new_usertype<Scene>("Scene", sol::call_constructor,
                             sol::constructors<sol::types<>>{}, "addMesh",
                             &Scene::addMesh, "loadModel", &Scene::loadModel,
                             "update", &Scene::update);*/

  // base types
  // issue: vec2.new will bounce back to the c++ side to call a trivial
  // constructor
  // solution: define the constructors on the Lua side
  /*module.new_usertype<vec2>("vec2", sol::call_constructor,
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
      &ivec4::y, "z", &ivec4::z, "w", &ivec4::w);*/

  // DrawPass
  /*module.new_usertype<Shader>(
      "Shader", sol::call_constructor,
      sol::constructors<sol::types<>>{},
      "setVertexShader", &Shader::setVertexShader, "setFragmentShader",
      &Shader::setFragmentShader, "setBlendState",
      [](Shader &shader, int index, sol::table table) {
        gl::BlendState bs;
        bs.enabled = table.get_or("enabled", false);
        bs.modeRGB = table.get_or("modeRGB", gl::FUNC_ADD);
        bs.modeAlpha = table.get_or("modeAlpha", gl::FUNC_ADD);
        bs.funcSrcRGB = table.get_or("funcSrcRGB", gl::SRC_ALPHA);
        bs.funcDstRGB = table.get_or("funcDstRGB", gl::ONE_MINUS_SRC_ALPHA);
        bs.funcSrcAlpha = table.get_or("funcSrcAlpha", gl::ONE);
        bs.funcDstAlpha = table.get_or("funcDstAlpha", gl::ZERO);
		shader.setBlendState(index, bs);
      },
      "setRasterizerState",
      [](Shader &shader, sol::table table) {
        gl::RasterizerState rs;
        rs.fillMode = table.get_or("fillMode", gl::FILL);
        rs.cullMode = table.get_or("cullMode", gl::NONE);
        rs.frontFace = table.get_or("frontFace", gl::CCW);
        rs.depthBias = table.get_or("depthBias", 1.0f);
        rs.slopeScaledDepthBias = table.get_or("slopeScaledDepthBias", 1.0f);
        rs.depthClipEnable = table.get_or("depthClipEnable", false);
        rs.scissorEnable = table.get_or("scissorEnable", false);
		shader.setRasterizerState(rs);
      },
      "setDepthStencilState",
      [](Shader &shader, sol::table table) {
        gl::DepthStencilState dss;
        dss.depthTestEnable = table.get_or("depthTestEnable", false);
        dss.depthWriteEnable = table.get_or("depthWriteEnable", false);
        dss.stencilEnable = table.get_or("stencilEnable", false);
        dss.stencilFace = table.get_or("stencilFace", gl::FRONT_AND_BACK);
        dss.stencilFunc = table.get_or("stencilFunc", 0);
        dss.stencilRef = table.get_or("stencilRef", 0);
        dss.stencilMask = table.get_or("stencilMask", 0xFFFFFFFF);
        dss.stencilOpSfail = table.get_or("stencilOpSfail", 0);
        dss.stencilOpDPFail = table.get_or("stencilOpDPFail", 0);
        dss.stencilOpDPPass = table.get_or("stencilOpDPPass", 0);
		shader.setDepthStencilState(dss);
      },
      "setViewport", &Shader::setViewport);*/


  // stuff
  /*module["debugMessage"] = [](const char *str) {
    rawLogMessage(LogLevel::Debug, str);
  };
  module["warningMessage"] = [](const char *str) {
    rawLogMessage(LogLevel::Warning, str);
  };
  module["errorMessage"] = [](const char *str) {
    rawLogMessage(LogLevel::Error, str);
  };*/

  /*module.new_usertype<Window>("Window", sol::call_constructor, sol::constructors<sol::types<int, int, const char*>>{},
	  "getFramebufferSize", [](Window& w) { auto size = w.getFramebufferSize(); return std::make_tuple(size.x, size.y); },
	  "getCursorPosition", [](Window& w) { auto pos = w.getCursorPosition(); return std::make_tuple(pos.x, pos.y); },
	  "getWindowSize", [](Window& w) { auto size = w.getWindowSize(); return std::make_tuple(size.x, size.y); },
	  "getKey", &Window::getKey, 
	  "close", &Window::close
	  );*/

  // CameraControl
  /*module.new_usertype<CameraControl>(
      "CameraControl", sol::call_constructor, sol::constructors<sol::types<>>(),
      "zoomIn", &CameraControl::zoomIn, "setZoom", &CameraControl::setZoom,
      "rotate", &CameraControl::rotate, "pan", &CameraControl::pan, "lookAt",
      static_cast<void (CameraControl::*)(float, float, float)>(
          &CameraControl::lookAt),
      "setAspectRatio", &CameraControl::setAspectRatio, "setFieldOfView",
      &CameraControl::setFieldOfView, "setNearFarPlanes",
      &CameraControl::setNearFarPlanes, "getCamera", &CameraControl::getCamera,
      "lookDistance", &CameraControl::lookDistance);*/

  // RenderUtils
  /*module.new_usertype<RenderUtils>("RenderUtils", sol::call_constructor,
                                   sol::constructors<sol::types<>>());*/

  //module["loadTexture"] = &loadTexture;
  //module["loadImage"] = &loadImage;

  // Arcball
  // module.new_usertype<Arcball>("Arcball"
  //);

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
}
