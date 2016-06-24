#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cppformat/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "buffer.hpp"
#include "cast_node.hpp"
#include "clear_node.hpp"
#include "compute_node.hpp"
#include "draw.hpp"
#include "draw_node.hpp"
#include "format.hpp"
#include "image.hpp"
#include "node.hpp"
#include "value.hpp"
#include "dyn_cast.hpp"
#include "dump_graph.hpp"

#include "await.hpp"
#include "gl_device.hpp"
#include "observable.hpp"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <GLFW/glfw3.h>

#include <experimental/filesystem>

#include "ui/button.hpp"
#include "ui/input.hpp"
#include "ui/native_window.hpp"
#include "ui/slider.hpp"
#include "ui/text_edit.hpp"

#define NANOVG_GL3_IMPLEMENTATION
#include "project_root.hpp"
#include <nanovg_gl.h>

#ifdef WIN32
#include <Xinput.h>
#endif

namespace fs = std::experimental::filesystem;

gl_sampler sam_linear_clamp{{gl::CLAMP_TO_EDGE, gl::CLAMP_TO_EDGE,
                             gl::CLAMP_TO_EDGE, gl::LINEAR, gl::LINEAR}};
gl_sampler sam_nearest_clamp{{gl::CLAMP_TO_EDGE, gl::CLAMP_TO_EDGE,
                              gl::CLAMP_TO_EDGE, gl::NEAREST, gl::NEAREST}};
gl_sampler sam_linear_repeat{
    {gl::REPEAT, gl::REPEAT, gl::REPEAT, gl::LINEAR, gl::LINEAR}};
gl_sampler sam_nearest_repeat{
    {gl::REPEAT, gl::REPEAT, gl::REPEAT, gl::NEAREST, gl::NEAREST}};

gl_sampler sam_linear_repeat_mipmap{{gl::REPEAT, gl::REPEAT, gl::REPEAT,
                                     gl::LINEAR_MIPMAP_NEAREST,
                                     gl::LINEAR_MIPMAP_NEAREST}};
gl_sampler sam_linear_repeat_mipmap_linear{{gl::REPEAT, gl::REPEAT, gl::REPEAT,
                                            gl::LINEAR_MIPMAP_LINEAR,
                                            gl::LINEAR_MIPMAP_LINEAR}};

constexpr gl_blend_state no_blend{
    false,   gl::FUNC_ADD, gl::FUNC_ADD, gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA,
    gl::ONE, gl::ZERO};
constexpr gl_blend_state alpha_blend{
    true,    gl::FUNC_ADD, gl::FUNC_ADD, gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA,
    gl::ONE, gl::ZERO};
constexpr gl_depth_stencil_state mesh_depth_stencil{
    true, true, false, gl::FRONT_AND_BACK, 0, 0, 0xFFFFFFFF, 0, 0, 0};
constexpr gl_depth_stencil_state no_depth{
    false, false, false, gl::FRONT_AND_BACK, 0, 0, 0xFFFFFFFF, 0, 0, 0};

auto mesh_attributes = {
    gl_vertex_attribute{0, gl::FLOAT, 3, 3 * sizeof(float), false},
    gl_vertex_attribute{0, gl::FLOAT, 3, 3 * sizeof(float), false},
    gl_vertex_attribute{0, gl::FLOAT, 3, 3 * sizeof(float), false},
    gl_vertex_attribute{0, gl::FLOAT, 2, 2 * sizeof(float), false}};

auto vertex_2d_attributes = {
    gl_vertex_attribute{0, gl::FLOAT, 2, 2 * sizeof(float), false},
    gl_vertex_attribute{0, gl::FLOAT, 2, 2 * sizeof(float), false}};

gl_graphics_pipeline pp_copy_tex = gl_graphics_pipeline::compile_from_file(
    project_root() / "glsl/copy_tex.glsl", {},
    pipeline_stages::vertex | pipeline_stages::pixel, vertex_2d_attributes,
    no_depth, gl_rasterizer_state{}, {alpha_blend});

gl_graphics_pipeline pp_copy_tex_mask = gl_graphics_pipeline::compile_from_file(
    project_root() / "glsl/copy_tex.glsl", {{"USE_MASK", ""}},
    pipeline_stages::vertex | pipeline_stages::pixel, vertex_2d_attributes,
    no_depth, gl_rasterizer_state{}, {alpha_blend});

gl_graphics_pipeline pp_render_sprites =
    gl_graphics_pipeline::compile_from_file(
        project_root() / "glsl/sprites.glsl", {},
        pipeline_stages::vertex | pipeline_stages::pixel, vertex_2d_attributes,
        no_depth, gl_rasterizer_state{}, {alpha_blend});

gl_compute_pipeline pp_blur_h = gl_compute_pipeline::compile_from_file(
    project_root() / "glsl/blur.glsl", {{"BLUR_H", ""}, {"FORMAT", "rgba8"}});
gl_compute_pipeline pp_blur_v = gl_compute_pipeline::compile_from_file(
    project_root() / "glsl/blur.glsl", {{"BLUR_V", ""}, {"FORMAT", "rgba8"}});

GLFWwindow *window = nullptr;

void render() {
  using namespace glm;

  gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
  gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gl::Clear(gl::COLOR_BUFFER_BIT);

  auto dl = ImGui::GetWindowDrawList();

  ImGui::BulletText("hello");
  ImGui::BulletText("world");

  vec2 p = ImGui::GetCursorScreenPos();
  dl->AddBezierCurve(p + vec2{0.0f, 0.0f}, p + vec2{20.0f, 0.0f},
                     p + vec2{100.0f - 20.0f, 50.0f}, p + vec2{100.0f, 50.0f},
                     ImColor{0.0f, 1.0f, 0.0f}, 3.0f);

  // dl->AddRectFilled(ImVec2{ 0.0f, 0.0f }, ImVec2{ 300.0f, 300.0f },
  // ImColor::HSV(1.0f, 0.5f, 1.0f), 0.5f);
}

/*float get_value() {
  float v = 0.0f;
  auto ui = ui::popup(ui::popup_buttons::ok | ui::popup_buttons::cancel)
                .content(ui::field("Value").validate([]() -> bool {...}));
  co::await(ui.closed);
}
*/

constexpr unsigned init_width = 640;
constexpr unsigned init_height = 480;

observable<> on_render;
observable<> on_update;


// UI:
// Goal: make is easy to implement complex behaviors/compose them
// e.g.
//    - a color picker
//    - a curve editor
//    - a 3D manipulator gizmo
//    - linked sliders
//    - a dock widget w/ tabs
//    - scroll bars!
// separate rendering from behaviors
// ui::checkbox_behavior VS ui::button_behavior
// each UI element is a combination of a behavior and a visual
// visuals can be provided by the user, although there is a 'standard' visual
// library
// UI elements should hold minimal state (as w/ ImGuis)
//  -> only a pointer to raw data, owned by the application
//  -> except where it makes sense
// => look at ImGui for behavior stuff
// Popups
// - drawing backend: resolution-independent
//    - use nanovg
//
// Visuals:
//  - holds the position of the control on the screen
//  - performs hit-testing
//  - the container can affect (override) the selected visual for a button
//    (e.g. button groups: first button, middle buttons and last buttons
// have a different visual)
//  - provide metrics to calculate the active area of the widget (which is
//  assumed to have a fixed shape)
//    e.g.
//    for color picker behavior: indicate the position of the hue
// selection square + sliders
//      simple_visual (buttons, checkboxes, etc)
//      composite_visual (for button groups and other complex
// widgets)
//        => hit_test(subelement)
//      visual_element: main, color_slider_x, color_slider_y,
// color_slider_z, color_rect, opacity_slider, etc
//  - place and render widgets
//  - must be easy to implement visuals
//
// Layout:
//  - two-phase: request + assignment
//
// UI redrawn only when necessary
//  - minimal redraws
//
// UI layout:
//  - golden rule: SENSIBLE DEFAULTS
//  - default widget set can have labels
//  - grid layouts
//
// Widgets:
//  - slider (behavior)
//  - button
//  - text input (use stb_textedit)
//  - checkboxes/radio
//  - dock + tabs
//  - color picker
//  - 2D vector widget
//  - button groups (custom rendering)
//
// Visual example: button grid
//  - visual: button_grid_visual
//    -> M*N sub-visuals
//  - the number of buttons can change dynamically over time
//  - button grid subvisual is (x,y) grid position, has a reference to parent
//  button_grid_visual
//  - rendering: render() is called on the main visual, children are bypassed
//  - visual classes:
//    visual: base class
//    rect_visual: generic rectangle
//    labeled_visual (rectangle w/ label: button, checkboxes, radios,
// etc.)
//    button_grid_visual
//    color_picker_visual
//    progress_bar_visual
//    curve_editor_visual
//    histogram_visual
//    text_editor_visual
//    progress_bar_visual
//    dock_visual
//    image_view_visual
//    combo_box_visual
//    list_box_visual
//    + custom visuals
//  - visuals are composable
//
//  - ui_elements have set_visual(uptr<visual>)
//
// Visual example: dock
//  - tab_bar_visual, floating_dock_visual, tab_container_visual, tab_visual
//  - create_floating_dock_visual(), create_tab_bar_visual()
//
//  ui::dock::tab_bar_visual (rect visual, dummy)
//    -> create_tab_item_visual
//  ui::dock::tab_bar_item_visual (dummy, hit-test done by parent
// tab_bar_visual)
//  ui::dock::tab_visual
//
// Visual example: scrollable regions
// => draw with scissor mask
//  - scrollbar_visual
//  - scrollable_region_visual
//
// Draw context:
//  - scissor stack
//
// Layouts:
//  - are standard UI elements
//
// Layout process:
//  - start from root
//  - call recursively calculateRequirements on each layout
//  - element::calculate_minimum_required_size first computes the content
//  minimum required size
//  - then, sizes and positions are assigned depending on layout fill modes
//
// Behaviors:
//  - takes input, does hit testing (attached to a visual), has state, triggers
//  events
//  - button_behavior
//      clicked
//  - hover_behavior
//      hovered
//  - drag_behavior(dof)
//      begin_drag(...)
//      end_drag(...)
//  - drop_behavior
//      dropped(ui::element)
//  - slider_behavior
//      -> drag_behavior
//  - checkbox_behavior(bool&)
//  - dock_behavior
//  - scroll_panel_behavior
//    - drag_behavior
//
// Visuals, styles and renderers go hand-in-hand
//
// Styles provide: default visuals + renderers
// ui::initialize(renderer)
// ui::initialize(style)
//
// Simplification pass: remove visuals
// => keep only mesure(), render()
//    render() by default calls the registered UI widget style
//(renderer.style())
//    ui::renderer: maintains clip stack and ref to style
//    can be overriden
// default renderer: needs a nanovg context
// otherwise: dummy style
// => fuse renderer and style
//
// issue: depending on the renderer, we may have to store additional data
// into the widget (animation state, etc.)
//
// issue:
//
// to create a custom visual for a widget: derive base class, override measure()
// & render()
// render() can launch coroutines
// actually, render() could be itself a coroutine
//

/*void chase()
{
        auto& ui_root = ui::root_window();

        for (;;)
        {
                {
                        ui::button button{ ui_root, "hello" };
                        await(button.pressed);
                        // Here, we are still in the middle of propagating UI
events
                        // (iterating through std::vectors of child elements)
                        // We cannot remove or add new elements without
invalidating
                        // iterators, which makes the program crash.
                        //
                        // Solution: defer the execution of UI observers until
the events
                        // have finished propagating through the UI element tree
                        //
                        // Implementation: event queues for observables /
schedulers
                        // observable.signal(scheduler, args...) (copy args)
                        // scheduler.process()
                }
                {
                        ui::button button{ ui_root, "world" };
                        await(button.pressed);
                }
        }
}*/

std::future<void> resumable_chase() {
  auto &ui_root = ui::root_window();

  for (;;) {
    {
      ui::button button{ui_root, "hello"};
      co_await button.pressed;
    }
    {
      ui::button button{ui_root, "world"};
      co_await button.pressed;
    }
  }
}

std::future<void> resumable_render() {
  std::future<void> f;
  for (int i = 0;; ++i) {
    co_await on_render;
  }
}

std::future<void> cancellable_ui(cancellation_token cancel) {
  ui::native_window dlg{glm::ivec2{400, 300}, "Dialog"};
  co_await (dlg.should_close | cancel);
  fmt::print(std::clog, "Closed dialog\n");

  // alternate version
 /* ui::native_window dialog {
	  ui::vertical_box {
		ui::vertical_box::slot{ ui::button{"Close dialog"} }
	  }
  };*/
}

int main() {
  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, gl::TRUE_);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, gl::TRUE_);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(init_width, init_height, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  gl::sys::LoadFunctions();
  gl_device_config devcfg;
  devcfg.init_fb_width = 640;
  devcfg.init_fb_height = 480;
  gl_device::init(devcfg);
  ImGui_ImplGlfwGL3_Init(window, true);

  fs::path proot = project_root();

  // NVG context for UI rendering
  auto nvg = nvgCreateGL3(NVG_ANTIALIAS);

  subscription sub;
  input::initialize(window);
  auto &root = ui::initialize(window, nvg);
  ui::button button{root, "hello"};
  std::string filename;
  ui::text_edit textedit{root, filename};
  button.pressed.subscribe(sub, []() { fmt::print(std::clog, "Boing!\n"); });

  input::gamepad_button_action act_fire{XINPUT_GAMEPAD_X, true};
  input::gamepad_button_action act_jump{XINPUT_GAMEPAD_A, true};

  auto fire_task = [&act_fire]() -> std::future<void> {
    for (;;) {
      co_await act_fire;
      fmt::print(std::clog, "Fire!\n");
    }
  };

  auto jump_task = [&act_jump](cancellation_token tk) -> std::future<void> {
    for (;;) {
      co_await act_jump;
      fmt::print(std::clog, "Jump!\n");
      tk.signal();
    }
  };

  // coroutine ui_task{chase};
  resumable_chase();
  cancellation_token tk;
  cancellable_ui(tk);

  auto f1 = fire_task();
  auto f2 = jump_task(tk);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    ImGui_ImplGlfwGL3_NewFrame();
    /* Render here */
    render();
    ImGui::Render();
    on_render.signal();
    input::process_input();
    ui::render();
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }
  sub.unsubscribe();

  ImGui_ImplGlfwGL3_Shutdown();

  nvgDeleteGL3(nvg);
  glfwTerminate();
  return 0;
}
