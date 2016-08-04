// autograph
#include "device.hpp"
#include "draw.hpp"
#include "entity.hpp"
#include "load_texture.hpp"
#include "project_root.hpp"
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

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <experimental/filesystem>
#include <optional.hpp>

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

// lazy resources:
// lazy_resource<T, loader>(Args...)

// loader that just calls the constructor
template <typename T> struct lazy_ctor {
public:
  template <typename... Args> T operator()(Args &&... args) {
    return T{std::forward<Args>(args)...};
  }

private:
};

struct texture_from_file_t {
  ag::texture
  operator()(const std::experimental::filesystem::path &p,
             ag::image_format tex_format = ag::image_format::rgba8_unorm) {
    return load_texture(p, tex_format);
  }
};

template <typename T, typename Loader = lazy_ctor<T>, typename... Args>
class lazy_resource {
public:
  lazy_resource(Args &&... args) : args_{args...} {}

  T &get() {
    if (!res_)
      do_load();
    return *res_;
  }

private:
  void do_load() {
    using namespace boost;
    res_ = hana::unpack(args_, Loader{});
  }

  std::experimental::optional<T> res_;
  std::tuple<Args...> args_;
};

template <typename T, typename Loader = lazy_ctor<T>, typename... Args>
lazy_resource<T, Loader, Args...> make_lazy_resource(Args &&... args) {
  return lazy_resource<T, Loader, Args...>(std::forward<Args>(args)...);
}

auto sprite = make_lazy_resource<ag::texture, texture_from_file_t>(
    project_root() / "img/tonberry.jpg");
int sprite_nvg_handle;

struct rect_2d {
  glm::vec2 pos;
  glm::vec2 size;
  glm::vec2 top_left() const { return pos; }
  glm::vec2 top_right() const { return pos + glm::vec2{size.x, 0.0f}; }
  glm::vec2 bottom_left() const { return pos + glm::vec2{0.0f, size.y}; }
  glm::vec2 bottom_right() const { return pos + glm::vec2{size.x, size.y}; }
};

struct rect_transform {
  // rect_transform() = default;
  // rect_transform(rect_transform&&) { fmt::print("Move invoked\n"); };
  // rect_transform(rect_transform const &) { fmt::print("Copy invoked"); }

  glm::vec2 offset_a; // rect corners relative to anchors
  glm::vec2 offset_b;

  glm::vec2 anchor_a; // upper-left anchor rect position (normalized)
  glm::vec2 anchor_b; // lower-right anchor rect position (normalized)

  glm::vec2 pivot{0.5f, 0.5f};          // relative to
  glm::vec3 rotation{0.0f, 0.0f, 0.0f}; // around pivot
  glm::vec3 scale{1.0f, 1.0f, 1.0f};    // around pivot

  rect_2d calc_rect(rect_2d parent) const {
    // anchor positions (relative)
    float anchor_top = parent.size.y * anchor_a.y;
    float anchor_bottom = parent.size.y * anchor_b.y;
    float anchor_left = parent.size.x * anchor_a.x;
    float anchor_right = parent.size.x * anchor_b.x;

    // rect corners
    float rect_top = anchor_top + offset_a.y;
    float rect_bottom = anchor_bottom + offset_b.y; // it can be negative
    float rect_left = anchor_left + offset_a.x;
    float rect_right = anchor_right + offset_b.x;

    cached_rect_.pos.x = std::round(rect_left) + 0.5f;
    cached_rect_.pos.y = std::round(rect_top) + 0.5f;
    cached_rect_.size.x = std::round(rect_right - rect_left);
    cached_rect_.size.y = std::round(rect_bottom - rect_top);
    return cached_rect_;
  }

  void debug(NVGcontext *vg) {
    nvgSave(vg);

    glm::vec2 piv_trans = pivot * cached_rect_.size;
    nvgTranslate(vg, cached_rect_.pos.x, cached_rect_.pos.y);
    nvgTranslate(vg, piv_trans.x, piv_trans.y);
    nvgRotate(vg, rotation.x);
    // nvgScale(vg, scale.x, scale.y);
    nvgTranslate(vg, -piv_trans.x, -piv_trans.y);

    // Local space: draw stuff

	nvgBeginPath(vg);
	//nvgScissor(vg, 0.0f, 0.0f, cached_rect_.size.x, cached_rect_.size.y);
	nvgFillPaint(vg, nvgImagePattern(vg, 0.0f, 0.0f, cached_rect_.size.x, cached_rect_.size.y, 0.0f, sprite_nvg_handle, 1.0f));
	nvgRect(vg, 0, 0, cached_rect_.size.x, cached_rect_.size.y);
	nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, cached_rect_.size.x, cached_rect_.size.y);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(vg, 1.0f);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, piv_trans.x, piv_trans.y, 4.0f);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 0));
    nvgFillColor(vg, nvgRGB(255, 0, 0));
    nvgFill(vg);

    nvgRestore(vg);
  }

  void test_gui() {
    ImGui::SliderFloat2("Offset A", &offset_a[0], 0.0f, 6000.0f, "%.1f", 1.3f);
    ImGui::SliderFloat2("Offset B", &offset_b[0], 0.0f, 6000.0f, "%.1f", 1.3f);
    ImGui::SliderFloat2("Anchor A", &anchor_a[0], 0.0f, 1.0f);
    ImGui::SliderFloat2("Anchor B", &anchor_b[0], 0.0f, 1.0f);
    ImGui::SliderFloat2("Pivot", &pivot[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Rotation", &rotation[0], 0.0f, 2.0f * 3.141592f);
    ImGui::SliderFloat2("Scale", &scale[0], 0.01f, 60000.0f, "%.1f", 1.3f);
    ImGui::Separator();
    float calc[4];
    calc[0] = cached_rect_.pos.x;
    calc[1] = cached_rect_.pos.y;
    calc[2] = cached_rect_.size.x;
    calc[3] = cached_rect_.size.y;
    ImGui::SliderFloat4("Calculated", calc, 0.0f, 60000.0f);
  }

  // cached calculated rect before scaling & rotation
  mutable rect_2d cached_rect_;
};

// vector graphics component
struct vector_graphics {
  std::function<void(NVGcontext *, rect_transform)> on_draw;
};

GLFWwindow *window;
NVGcontext *nvg = nullptr;
constexpr auto init_width = 640;
constexpr auto init_height = 480;
int width = init_width;
int height = init_height;

class test {
public:
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

    // entity test
    gui_entity = S.create_entity();
    rect_transform rt_init;
    rt_init.anchor_a = glm::vec2{0.0f, 0.0f};
    rt_init.anchor_b = glm::vec2{1.0f, 1.0f};
    rt_init.offset_a = glm::vec2{0.0f, 0.0f};
    rt_init.offset_b = glm::vec2{0.0f, 0.0f};
    gui_entity->add_component(rect_transform{rt_init});
  }

  void render(int W, int H, int fbW, int fbH) {
    // vertex and index data (a rectangle)
    /*static const std::array<glm::vec2, 4> vertices{ {
            {-0.5f, 0.5f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f},
    } };
    static const std::array<int, 6> indices{ {0, 1, 2, 2, 1, 3} };
    // upload vertices to the ring buffer
    auto vbo = ag::upload_frame_array(vertices);
    // upload indices
    auto ibo = ag::upload_frame_array(indices);
    // setup transform
    glm::mat4 transform = glm::mat4{ 1.0f };
    // draw stuff
    // this call will unfold into the corresponding glUniform*,
    // glBindVertexBuffers, and glDraw* calls
    ag::draw(
            ag::get_default_framebuffer(), prog2d,
            ag::draw_indexed{ GL_TRIANGLES, 0, 6, 0, ibo, GL_UNSIGNED_INT },
            // Resources
            vao, ag::vertex_buffer{ 0, vbo, sizeof(glm::vec2) },
            ag::uniform_matrix4{ "transform", transform },
            // We have to specify the viewport because we are binding the
    framebuffer
            // from a GLuint, which does not set a default viewport
            ag::viewport{ 0, {0, 0, width, height} });*/

    glViewport(0, 0, fbW, fbH);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const entity *e : S.find<rect_transform>()) {
      auto c = e->get_component<rect_transform>();
      c->calc_rect(rect_2d{{0.0f, 0.0f},
                           {static_cast<float>(fbW), static_cast<float>(fbH)}});
      c->test_gui();
      c->debug(nvg);
    }
  }

private:
  scene S;
  entity::ptr gui_entity;
  rect_transform *rt;
  ag::vertex_array default_vao;
  ag::vertex_array vao;
  ag::program prog2d;
  ag::texture tex;
  ag::texture depth_tex;
  ag::framebuffer fbo;
};

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
  ogl_LoadFunctions();
  ImGui_ImplGlfwGL3_Init(window, true);

  test t;
  t.initializeGL();
  // NVG context for UI rendering
  nvg = nvgCreateGL3(NVG_ANTIALIAS);

  auto &sprite_tex = sprite.get();
  sprite_nvg_handle = nvglCreateImageFromHandleGL3(nvg, sprite_tex.object(), sprite_tex.width(), sprite_tex.height(), 0);


  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    glfwGetWindowSize(window, &width, &height);
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    // Calculate pixel ratio for hi-dpi devices.
    auto pxRatio = (float)fbWidth / (float)width;
    fmt::print("FB {}x{} window {}x{}\n", fbWidth, fbHeight, width, height);
	ImGui_ImplGlfwGL3_NewFrame();
    nvgBeginFrame(nvg, width, height, pxRatio);
    t.render(width, height, fbWidth, fbHeight);
    nvgEndFrame(nvg);
	ImGui::Render();
    ag::end_frame();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplGlfwGL3_Shutdown();
  nvgDeleteGL3(nvg);
  glfwTerminate();
  return 0;
}
