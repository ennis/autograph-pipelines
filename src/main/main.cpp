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
}


constexpr unsigned init_width = 640;
constexpr unsigned init_height = 480;

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

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    ImGui_ImplGlfwGL3_NewFrame();
    /* Render here */
    render();
    ImGui::Render();
//    on_render.signal();
    input::process_input();
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }
  //sub.unsubscribe();

  ImGui_ImplGlfwGL3_Shutdown();

  nvgDeleteGL3(nvg);
  glfwTerminate();
  return 0;
}
