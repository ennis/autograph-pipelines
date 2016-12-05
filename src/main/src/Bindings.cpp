#include "Bindings.h"

#include <autograph/support/ProjectRoot.h>
// core
#include <autograph/Camera.h>
#include <autograph/Transform.h>
#include <autograph/Types.h>
// gl
#include <autograph/gl/Buffer.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
// engine
#include "Mesh.h"
#include "Scene.h"
#include "SceneRenderer.h"

namespace ag {
sol::table openLuaBindings(sol::this_state s) {
  sol::state_view lua{s};
  sol::table module = lua.create_table();

  module["getActualPath"] = &ag::getActualPath;
  module["getProjectRootDirectory"] = &ag::getProjectRootDirectory;
  // module["drawMesh"] = &drawMesh;
  // module["get"]


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
  module.new_usertype<Camera>("Camera");


  // Transform
  module.new_usertype<Transform>("Transform", "scaling", &Transform::scaling, "position",
                                 &Transform::position, "rotation",
                                 &Transform::rotation,
                                 "getMatrix", &Transform::getMatrix,
                                 "transformPoint", &Transform::transformPoint,
                                 "transformVec", &Transform::transformVec,
                                 "transformNormal", &Transform::transformNormal);

  // Scene
  module.new_usertype<AABB>(
      "AABB", "xmin", &AABB::xmin, "xmax", &AABB::xmax, "ymin", &AABB::ymin,
      "ymax", &AABB::ymax, "zmin", &AABB::zmin, "zmax", &AABB::zmax, "width",
      &AABB::width, "height", &AABB::height, "depth", &AABB::depth);

  module.new_usertype<Mesh>("Mesh", "loadFromFile",
                            sol::factories(&Mesh::loadFromFile), "AABB",
                            sol::property(&Mesh::getAABB));

  module.new_usertype<SceneObject>(
      "SceneObject", "id", sol::property(&SceneObject::id), "mesh",
      &SceneObject::mesh, "transform", &SceneObject::transform);

  module.new_usertype<Scene>("Scene", "createSceneObject",
                             &Scene::createSceneObject);

  // base types
  // issue: vec2.new will bounce back to the c++ side to call a trivial constructor
  // solution: define the constructors on the Lua side
  // XXX: same for every vector component access
  module.new_usertype<vec2>("vec2", sol::call_constructor, sol::constructors<sol::types<float, float>>{}, "x", &vec2::x, "y", &vec2::y);
  module.new_usertype<vec3>("vec3", sol::call_constructor, sol::constructors<sol::types<float, float, float>>{}, "x", &vec3::x, "y", &vec3::y, "z", &vec3::z);
  module.new_usertype<vec4>("vec4", sol::call_constructor, sol::constructors<sol::types<float, float, float, float>>{}, "x", &vec4::x, "y", &vec4::y, "z", &vec4::z, "w", &vec4::w);
  module.new_usertype<ivec2>("ivec2", sol::call_constructor, sol::constructors<sol::types<int, int>>{}, "x", &ivec2::x, "y", &ivec2::y);
  module.new_usertype<ivec3>("ivec3", sol::call_constructor, sol::constructors<sol::types<int, int, int>>{}, "x", &ivec3::x, "y", &ivec3::y, "z", &ivec3::z);
  module.new_usertype<ivec4>("ivec4", sol::call_constructor, sol::constructors<sol::types<int, int, int, int>>{}, "x", &ivec4::x, "y", &ivec4::y, "z", &ivec4::z, "w", &ivec4::w);
  
  return module;
}
}