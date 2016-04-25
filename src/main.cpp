#include <fstream>
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

#include <autograph/gl_types.hpp>
#include <autograph/utils.hpp>

#include "draw.hpp"
#include "buffer.hpp"
#include "cast_node.hpp"
#include "clear_node.hpp"
#include "compute_node.hpp"
#include "draw_node.hpp"
#include "format.hpp"
#include "image.hpp"
#include "node.hpp"
#include "value.hpp"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <GLFW/glfw3.h>

#include "load_image.hpp"
#include <experimental/filesystem>


namespace fs = std::experimental::filesystem;
constexpr const char proj_name[] = "autograph-pipelines";

fs::path project_root() {
	static bool found = false;
	static auto path = fs::current_path();
	if (!found) {
		if (!fs::is_directory(path / proj_name)) {
			path = path.parent_path();
			if (!fs::is_directory(path / proj_name)) {
				path = path.parent_path();
				if (!fs::is_directory(path / proj_name)) {
					path = path.parent_path();
					if (!fs::is_directory(path / proj_name)) {
						path = path.parent_path();
						if (!fs::is_directory(path / proj_name)) {
							path = path.parent_path();
							if (!fs::is_directory(path / proj_name)) {
								throw std::runtime_error(fmt::format(
									"project root directory not found: {}", proj_name));
							}
						}
					}
				}
			}
		}
		found = true;
	}
	return std::move(path / proj_name);
}



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

gl_graphics_pipeline pp_render_sprites = gl_graphics_pipeline::compile_from_file(
	project_root() / "glsl/sprites.glsl", {},
	pipeline_stages::vertex | pipeline_stages::pixel, vertex_2d_attributes,
	no_depth, gl_rasterizer_state{}, { alpha_blend });

gl_compute_pipeline pp_blur_h = gl_compute_pipeline::compile_from_file(
	project_root() / "glsl/blur.glsl", { { "BLUR_H", "" },{ "FORMAT", "rgba8" } });
gl_compute_pipeline pp_blur_v = gl_compute_pipeline::compile_from_file(
	project_root() / "glsl/blur.glsl", { { "BLUR_V", "" },{ "FORMAT", "rgba8" } });

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
  auto ui = ui::popup(ui::popup_buttons::ok)
                .content(ui::field("Value").validate([]() -> bool {...}));
  co::await(ui.closed);
  return v;
}*/

struct style {
  // frame default BG color
  // frame default font color
  // frame default FG color
  // frame scrollbar color
  // frame border color
  // button color
  // button text color
};

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
  auto img1 = load_image(proot / "img/tonberry.jpg");
  auto imgBlur = img1.subimage(rect_2d{0, 0, 128, 128})
                     .filter(pp_blur_h, 16, 16, 1.0f)
                     .filter(pp_blur_v, 16, 16, 1.0f);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) 
  {
    ImGui_ImplGlfwGL3_NewFrame();
    /* Render here */
    render();
    ImGui::Image((ImTextureID)img1.impl_->storage.device_tex->obj_.get(),
                 ImVec2{(float)img1.impl_->desc_.width,
                        (float)img1.impl_->desc_.height});
    ImGui::Render();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  ImGui_ImplGlfwGL3_Shutdown();

  glfwTerminate();
  return 0;
}
