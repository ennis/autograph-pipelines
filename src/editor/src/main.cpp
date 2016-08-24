// std
#include <eggs/variant.hpp>
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <optional.hpp>
// autograph
#include "device.hpp"
#include "draw.hpp"
#include "load_texture.hpp"
#include "project_root.hpp"
#include "texture.hpp"
#include "vertex_array.hpp"
// GLFW
#include <GLFW/glfw3.h>
// nanovg + impl
#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <cppformat/format.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <nanovg_gl.h>
// core engine stuff
#include "await.hpp"
#include "coroutine_script.hpp"
#include "debug_imgui.hpp"
#include "input.hpp"
#include "meta.hpp"
#include "observable.hpp"
#include "rect_transform.hpp"
#include "scene.hpp"
#include "sprite.hpp"
// UI stuff
#include "ui/button.hpp"
#include "ui/layout.hpp"
#include "ui/text.hpp"
#include "ui/visual.hpp"

const char *VS = R"(
#version 440
layout(location=0) in vec2 position;
uniform mat4 transform;
void main() {
	vec4 P = vec4(position, 0.0, 1.0);
	gl_Position = transform * P;
}
)";

const char *FS = R"(
#version 440
layout(location=0) out vec4 color;
void main() {
	color = vec4(0.0, 1.0, 0.0, 1.0);
}
)";

auto sprite = ag::load_texture_lazy(project_root() / "img/tonberry.jpg");

// image = ID that refers to a file in the resource directory
// can ask for the associated ag::texture object
//
// resource<T>::has_changed()
//
// those queries should be fast
// engine::texture_cache::get(resource<image>&) -> ag::texture
// engine::texture_cache::get_atlas(resource<atlas>&) -> engine::texture_atlas
// engine::shader_cache::get(resource<shader>&) -> ag::program
// engine::render_pipeline_cache::get(resource<render_pipeline>&) -> ...
// engine::prefab_cache::get(...) -> entity::ptr
// engine::mesh_cache::get(...) -> engine::mesh
//
// resource ID types:
// images
//		-> concrete resources: images, textures, fonts, etc.
// shaders
//		-> concrete resources: shader instance?
// prefabs (entity containers), scenes
// render pipelines
//
// generated resources?
// e.g. texture atlas generated from many images
//
// dependency DAG between resources?
// relationship with render pipelines?
//
// Components should contain only resource IDs
// Cache pointer to loaded resources in RIDs
// RID = id (or pointer?) in global resource table
//	-> cheap to copy
//  -> creation: symbol table lookup
//  -> ID = UID+revision count
//
// image_resource
//		has_changed()
//		get_texture()
//      get_data()
// mesh_resource
//		get_mesh()
// shader_resource
//      get_program()
//
// Should scripts at runtime be able to see the file path of a resource?
// => Depends.
// use resource::get_asset_path(<texture or shader or mesh>) -> fs::path
// serialize like this
// Goal: no overhead if live-reload is not needed
//
// live-reload?
// => change the underlying texture or shader, keep the same pointer?
// => NO
// => replace the reference using reflection data

GLFWwindow *window;
constexpr auto init_width = 640;
constexpr auto init_height = 480;
int width = init_width;
int height = init_height;

void render_ui_update_transforms(entity *e, const glm::mat3 &parent_tr,
                                 glm::vec2 parent_size) {
  if (auto rect_tr = e->get_component<rect_transform>()) {
    rect_tr->update_transform(parent_tr, parent_size);
    if (auto layout_ctl = e->get_component<ui::layout_controller>())
      layout_ctl->layout_contents();
    for (auto &&child : rect_tr->children)
      render_ui_update_transforms(child.get(), rect_tr->calc_transform,
                                  rect_tr->calc_size);
  }
}

void nvg_ui_push_transform(NVGcontext *vg, const rect_transform &rt) {
  nvgSave(vg);
  const auto &m = rt.calc_transform;
  nvgTransform(vg, m[0][0], m[0][1], m[1][0], m[1][1], m[2][0], m[2][1]);
}

void nvg_ui_pop_transform(NVGcontext *vg) { nvgRestore(vg); }

void render_ui_recursive(NVGcontext *vg, entity *ent) {
  auto vis = ent->get_component<ui::visual>();
  auto rect_tr = ent->get_component<rect_transform>();
  if (!rect_tr)
    return;
  nvg_ui_push_transform(vg, *rect_tr);
  if (vis)
    vis->render(vg, rect_tr->calc_size);
  // pop the transform here, because the transform hierarchy is already
  // flattened by render_ui_update_transforms
  nvg_ui_pop_transform(vg);
  for (auto &&child : rect_tr->children) {
    render_ui_recursive(vg, child.get());
  }
}

void render_ui(NVGcontext *vg, scene &S, entity::ptr root, int W, int H,
               int fbW, int fbH) {
  // update rect_transforms
  render_ui_update_transforms(
      root.get(), glm::mat3{1.0f},
      {static_cast<float>(fbW), static_cast<float>(fbH)});

  // DEBUG: clear viewports
  glViewport(0, 0, fbW, fbH);
  glClearColor(60.f / 255.f, 60.f / 255.f, 168.f / 255.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  //
  render_ui_recursive(vg, root.get());
}

bool hit_test_ui_recursive(entity *ent, glm::vec2 p) {
  if (auto tr = ent->get_component<rect_transform>()) {
    if (!tr->point_inside(p))
      return false;
    bool handled = false;
    for (auto &&c : tr->children)
      handled |= hit_test_ui_recursive(c.get(), p);
    if (!handled)
      if (auto sel = ent->get_component<ui::selectable>())
        handled |= sel->on_pointer_down();
    return handled;
  } else
    return false;
}

bool hit_test_ui(scene &S, entity *ent, glm::vec2 p) {
  return hit_test_ui_recursive(ent, p);
}

entity::ptr create_button(scene &s, entity::ptr parent, std::string text) {
  auto ent = s.create_entity(
      "Button", rect_transform{parent},
      ui::button{ui::button_visual_pressed, ui::button_visual_released},
      ui::vector_visual{ui::button_visual_released});
  if (parent)
    parent->get_component<rect_transform>()->children.push_back(ent);
  auto text_ent = s.create_entity(
      "Text", rect_transform{parent},
      ui::text_visual{std::move(text), 40.0f, {0.0f, 0.0f, 0.0f, 1.0f}, true});
  ent->get_component<rect_transform>()->children.push_back(std::move(text_ent));
  return ent;
}

entity::ptr create_panel(scene &s, entity::ptr parent) {
  auto ent =
      s.create_entity("Panel", rect_transform{parent}, ui::layout_component{},
                      ui::vertical_layout_controller{});
  if (parent)
    parent->get_component<rect_transform>()->children.push_back(ent);
  return ent;
}

constexpr float twopi_f = 6.28318530718f;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(init_width, init_height, "Render", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /*GLFWwindow* debug_window = glfwCreateWindow(480, 640, "Debug", NULL, NULL);
  if (!debug_window) {
          glfwTerminate();
          return -1;
  }*/

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  ogl_LoadFunctions();
  ImGui_ImplGlfwGL3_Init(window, true);
  input::initialize(window);

  // NVG context for UI rendering
  NVGcontext *nvg = nvgCreateGL3(NVG_ANTIALIAS);
  // load default font
  int handle = nvgCreateFont(
      nvg, "Roboto", (project_root() / "Roboto-Regular.ttf").string().c_str());
  fmt::print("font handle {}\n", handle);
  nvgFontFace(nvg, "Roboto");

  scene s;
  auto panel = create_panel(s, nullptr);
  auto but = create_button(s, panel, "1");
  auto but2 = create_button(s, panel, "1");
  auto but3 = create_button(s, panel, "1");
  create_button(s, panel, "1");
  create_button(s, panel, "1");
  create_button(s, panel, "2");
  create_button(s, panel, "1");
  create_button(s, panel, "1");
  create_button(s, panel, "1");
  create_button(s, panel, "6");
  create_button(s, panel, "1");
  create_button(s, panel, "5");
  create_button(s, panel, "1");
  create_button(s, panel, "7");
  create_button(s, panel, "8");
  create_button(s, panel, "8");

  {
    auto tr = panel->get_component<rect_transform>();
    tr->anchor_a = {0.2f, 0.2f};
    tr->anchor_b = {0.8f, 0.8f};
    tr->offset_a = {0.0f, 0.0f};
    tr->offset_b = {0.0f, 0.0f};
    tr->pivot = {0.5f, 0.5f};
    tr->rotation.x = 0.2f;
  }

  {
    panel->remove_component<ui::layout_controller>();
    panel->add_component<ui::grid_layout_controller>(4, 4);
    panel->add_script<coroutine_script>(
        [](scene &s, entity::ptr e) {
          auto tr = e->get_component<rect_transform>();
          float rot = 0.0f;
          while (true) {
            await(s.update);
            if (e->is_deleted())
              return;
            tr->rotation.x = rot;
            rot += twopi_f / 180.0f;
            rot = std::fmod(rot, twopi_f);
          }
        },
        s, panel);
  }

  // input handler
  subscription sub;
  input::events.subscribe(sub, [&s, &panel](auto ev) {
    if (auto mb_ev = ev->template as<input::mouse_button_event>()) {
      if (mb_ev->state == input::button_state::pressed) {
        glm::vec2 cursor_pos;
        cursor_pos.x = static_cast<float>(input::cursor_pos().x);
        cursor_pos.y = static_cast<float>(input::cursor_pos().y);
        hit_test_ui(s, panel.get(), cursor_pos);
      }
    }
  });

  // serialization test
  std::ofstream ser_out{ "scene.bin", std::ios::binary };
  msgpack::packer<std::ostream> packer{ ser_out };
  s.serialize(packer);
  ser_out.close();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    //====================================================
    // DEBUG WINDOW: update
    // glfwMakeContextCurrent(debug_window);
    // trigger input polling
    input::process_input();
    // trigger scene update
    s.update();

    //====================================================
    // MAIN WINDOW: rendering
    // glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &width, &height);
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    // Calculate pixel ratio for hi-dpi devices.
    auto pxRatio = (float)fbWidth / (float)width;
    ImGui_ImplGlfwGL3_NewFrame();
    scene_debug_gui(s);
    nvgBeginFrame(nvg, width, height, pxRatio);
    render_ui(nvg, s, panel, width, height, fbWidth, fbHeight);
    nvgEndFrame(nvg);
    ImGui::Render();
    // ag::end_frame();
    glfwSwapBuffers(window);
    glfwPollEvents();

    // End frame: cleanup deleted entities
    s.collect();
  }

  ImGui_ImplGlfwGL3_Shutdown();
  nvgDeleteGL3(nvg);
  glfwTerminate();
  return 0;
}
