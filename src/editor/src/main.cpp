// autograph
#include "draw.hpp"
#include "texture.hpp"
#include "vertex_array.hpp"
// GLFW
#include <GLFW/glfw3.h>
// misc
#include <cppformat/format.h>
#include <iostream>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

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

void initializeGL() {
  fmt::print(std::cerr, "initializeGL\n");
  // initialize autograph
  ag::device_config cfg;
  cfg.init_fb_width = 640;
  cfg.init_fb_height = 480;
  cfg.max_frames_in_flight = 3;
  ag::initialize(cfg);
  // VAO: 2D vertices
  default_vao.initialize({});
  vao.initialize(
      {ag::vertex_attribute{0, GL_FLOAT, 2, 2 * sizeof(float), false}});
  // shader program
  prog2d = ag::program::create(VS, FS);
}

void resizeGL(int w, int h) override {
  fmt::print(std::cerr, "resizeGL {} {}\n", w, h);
  // reset framebuffer
  // fbo = {};
  // re-create textures
  // tex = ag::texture::create_2d(w, h, ag::image_format::rgba8_unorm);
  // ag::clear(tex, glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});
  // depth_tex = ag::texture::create_2d(w, h, ag::image_format::depth32_float);
  // ag::clear_depth(depth_tex, 1.0f);
  // re-create framebuffer
  // fbo = ag::framebuffer::create({&tex}, depth_tex);
}

void paintGL() override {
  fmt::print(std::cerr, "paintGL\n");
  // vertex and index data (a rectangle)
  static const std::array<glm::vec2, 4> vertices{{
      {-0.5f, 0.5f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f},
  }};
  static const std::array<int, 6> indices{{0, 1, 2, 2, 1, 3}};
  // upload vertices to the ring buffer
  auto vbo = ag::upload_frame_array(vertices);
  // upload indices
  auto ibo = ag::upload_frame_array(indices);
  // setup transform
  glm::mat4 transform = glm::mat4{1.0f};
  // draw stuff
  // this call will unfold into the corresponding glUniform*,
  // glBindVertexBuffers, and glDraw* calls
  ag::draw(ag::get_default_framebuffer(), prog2d,
      ag::draw_indexed{GL_TRIANGLES, 0, 6, 0, ibo, GL_UNSIGNED_INT},
      // Resources
      vao, ag::vertex_buffer{0, vbo, sizeof(glm::vec2)},
      ag::uniform_matrix4{"transform", transform},
      // We have to specify the viewport because we are binding the framebuffer
      // from a GLuint, which does not set a default viewport
      ag::viewport{0, {0, 0, width(), height()}});
  // glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT);
}

ag::vertex_array default_vao;
ag::vertex_array vao;
ag::program prog2d;
ag::texture tex;
ag::texture depth_tex;
ag::framebuffer fbo;


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
  window = glfwCreateWindow(init_width, init_height, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  gl::sys::LoadFunctions();

  // NVG context for UI rendering
  auto nvg = nvgCreateGL3(NVG_ANTIALIAS);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    render();
    ag::end_frame();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  nvgDeleteGL3(nvg);
  glfwTerminate();
  return 0;
}
