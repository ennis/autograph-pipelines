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
#include <GLFW\glfw3.h>

#include "load_image.hpp"
#include <experimental/filesystem>

// static (or: compile-static): value known at compilation of C++ code
// pipeline-static: value known at pipeline creation
// pipeline-dynamic: value not known at pipeline creation

// TODO: mip-map generation node
//		input: image
//		param: filtering kernel, levels
//		output: mip-mapped image
//
// TODO: custom nodes
// TODO: pipelines with operator overloading
// TODO: make custom nodes from functions
//		image | blur(...) | threshold(...)
//
// gl_texture: subimage, different format

/*template <typename R>
void bind_resources_impl(shader_resources &shr, bind_resource_context &ctx,
                         R &&res0) {
  bind_resource(shr, ctx, res0);
}

template <typename R, typename... Resources>
void bind_resources_impl(shader_resources &shr, bind_resource_context &ctx,
                         R &&res0, Resources &&... res) {
  bind_resource(shr, ctx, res0);
  bind_resources_impl(shr, ctx, std::forward<Resources>(res)...);
}

template <typename... Resources>
void bind_resources(shader_resources &shr, Resources &&... res) {
  bind_resource_context ctx;
  bind_resources_impl(shr, ctx, std::forward<Resources>(res)...);
}
*/

// template <typename ... Resources>
// image

// draw( surface(stencil, normals, n_dot_v, n_dot_l, visibility), pipeline,
// draw_indexed {...}, vertex_buffer(0, vbo0), vbo1, vbo2, shadow_map )

// OpenGL scheduler/executor
/*class opengl_scheduler {
public:
  using resource_id = int;

  // opcodes used in evaluation plans
  enum class opcode {
        begin_frame,
    clear_float,
    clear_integer,
    copy_texture_host_device,
    copy_texture_device_device,
    memory_barrier,
        upload_uniform,
    dispatch_compute,
        end_frame
  };

  struct op {
    opcode opcode;
    virtual void execute() {}
  };

  struct op_clear_float : public op {
    resource_id target; // texture or framebuffer object
    float clear_color[4];
  };

  struct op_clear_integer : public op {
    resource_id target; // texture or framebuffer object
    int clear_color[4];
  };

  struct op_copy_texture_host_device : public op {
    // src, dest, extents
    resource_id src, dest;
  };

  struct op_copy_texture_device_device : public op {
  };
  struct op_memory_barrier : public op {
  };
  struct op_dispatch_compute : public op {
  };

  struct image_resource {
    GLuint tex_obj;
    GLenum gl_internal_format;
    image_format format;
    unsigned width;
    unsigned height;
  };

  struct fbo_resource {
    GLuint framebuffer_obj;
    unsigned num_attachements;
    resource_id color_attachements[8];
    resource_id depth_stencil_attachement;
  };

  struct buffer_resource {
    GLuint buf_obj;
    size_t byte_size;
  };

private:
};*/

////////////////////////////////////////////////////////
// Graph dumper
/*struct graph_dumper {
public:
  graph_dumper(std::ostream &os) : indentlv_(0), os_(os) {}

  void traverse(value_impl *v) {
    if (dumpres.find(v) == dumpres.end()) {
      vtovisit_.push_back(v);
      dumpres.insert(v);
    }
  }

  void traverse(node *n) {
    if (dumpnodes.find(n) == dumpnodes.end()) {
      ntovisit_.push_back(n);
      dumpnodes.insert(n);
    }
  }

  void do_traverse() {
    auto vv = vtovisit_;
    vtovisit_.clear();
    for (auto &&v : vv) {
      visit(v);
      do_traverse();
    }
    auto nn = ntovisit_;
    ntovisit_.clear();
    for (auto &&n : nn) {
      visit(n);
      do_traverse();
    }
  }

  void visit(value_impl *v) {
    if (auto img = dyn_cast<image_impl>(v)) {
      printi("Image {}", img->name_);
    } else if (auto buf = dyn_cast<buffer_impl>(v)) {
      printi("Buffer {}", buf->name_);
    }
    if (v->pred_)
      traverse(v->pred_);
  }

  void visit(node *n) {
    if (auto nn = dyn_cast<clear_node>(n)) {
      printi("Clear");
    } else if (auto nn = dyn_cast<constant_node>(n)) {
      printi("Constant");
    } else if (auto nn = dyn_cast<draw_node>(n)) {
      printi("Draw");
      pushi();
      printi("Attachements");
      pushi();
      int att = 0;
      for (auto &&b : nn->att.color) {
        printi("color #{} -> {}", att, (void *)b.get());
        traverse(b.get());
      }
      if (nn->att.depth) {
        printi("depth-stencil {}", (void *)nn->att.depth.get());
        traverse(nn->att.depth.get());
      }
      popi();
      printi("Shader resources");
      pushi();
      for (auto &&r : nn->res) {
        printi("binding #{} [{}] -> {}", r.slot,
               get_shader_binding_type_name(r.bind_type),
               (void *)r.resource.get());
        traverse(r.resource.get());
      }
      popi();

      popi();
    } else if (auto nn = dyn_cast<cast_node>(n)) {
      printi("Cast");
    } else if (auto nn = dyn_cast<compute_shader_node>(n)) {
      printi("ComputeShader");
      pushi();
      printi("pipeline = {}", (void *)nn->pp);
      printi("Shader resources");
      pushi();
      for (auto &&r : nn->res) {
        printi("binding #{} [{}] -> {}", r.slot,
               get_shader_binding_type_name(r.bind_type),
               (void *)r.resource.get());
        traverse(r.resource.get());
      }
      popi();
      popi();
    }
  }

  template <typename... Args> void printi(const char *fmt, Args &&... args) {
    for (int i = 0; i < indentlv_ * 2; ++i)
      os_.put(' ');
    fmt::print(os_, fmt, std::forward<Args>(args)...);
    fmt::print(os_, "\n");
  }

  void pushi() { indentlv_++; }
  void popi() { indentlv_--; }

private:
  int indentlv_;
  std::ostream &os_;
  std::unordered_set<value_impl *> dumpres;
  std::unordered_set<node *> dumpnodes;
  std::vector<value_impl *> vtovisit_;
  std::vector<node *> ntovisit_;
};*/

GLFWwindow *window = nullptr;

void test() {}

struct node_graph {
  enum class port_kind { input, output };

  struct port_data {
    std::string data;
    float off;
  };

  struct node_data {
    size_t id;
    unsigned x, y, w, h;
    std::vector<port_data> inp;
    std::vector<port_data> outp;
    std::string data;
  };

  struct edge_data {
    unsigned si, sp, di, dp;
  };

  std::unordered_map<size_t, node_data> ndat;
  std::unordered_map<size_t, edge_data> edat;

  // events:
  // drag_port_begin(nid, pid) -> bool
  // drag_port(nid, pid, position) -> bool
  // drag_port_end(nid, pid) -> bool
  // move_node_begin(nid, pid) -> bool
  // move_node(nid, pid) -> bool

  // state/observables
  // hit_test_port(nid, pid) -> bool
  // hit_test_node(nid, pid) -> bool
  // cursor_pos -> (u32,u32)

  // operations:
  // add_node(u32, sub_view, ports)
  // add_link(...)

  uint32_t add_node(std::string name, uint32_t x, uint32_t y);
  uint32_t add_port(uint32_t node, std::string name, port_kind kind);
  void add_link(uint32_t si, uint32_t sp, uint32_t di, uint32_t dp);

  void on_render() {
    for (auto &&n : ndat) {
    }
  }
};

// reacts to changes in the node graph
// provide the GUI with the graph structure
// perform validation of received input
struct node_view {
  // subscriber to:
  // nodes.added_element
  // nodes.removed_element
  // nodes.updated
};

void render_node() {
  using namespace glm;
  ImGui::Begin("hi");

  // put some data here

  auto dl = ImGui::GetWindowDrawList();
  vec2 p = ImGui::GetCursorScreenPos();

  ImGui::End();
}

void render() {
  using namespace glm;

  gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
  gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gl::Clear(gl::COLOR_BUFFER_BIT);

  auto dl = ImGui::GetWindowDrawList();

  ImGui::BulletText("hello");
  ImGui::BulletText("world");
  render_node();

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

// ui::frame
// + padding
// + margin
// + hit_test
// ui::field
// ui::button
// ui::edit_box

namespace fs = std::experimental::filesystem;

fs::path find_project_root(const char *name) {
  auto path = fs::current_path();
  if (!fs::is_directory(path / name)) {
    path = path.parent_path();
    if (!fs::is_directory(path / name)) {
      path = path.parent_path();
      if (!fs::is_directory(path / name)) {
        path = path.parent_path();
        if (!fs::is_directory(path / name)) {
          path = path.parent_path();
          if (!fs::is_directory(path / name)) {
            path = path.parent_path();
            if (!fs::is_directory(path / name)) {
              throw std::runtime_error(
                  fmt::format("project root directory not found: {}", name));
            }
          }
        }
      }
    }
  }
  return std::move(path / name);
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
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  gl::sys::LoadFunctions();
  ImGui_ImplGlfwGL3_Init(window, true);

  fs::path proot = find_project_root("autograph-pipelines");
  auto img1 = load_image(proot / "img/tonberry.jpg");

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
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
