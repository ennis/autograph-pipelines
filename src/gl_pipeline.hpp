#pragma once
#include <experimental/filesystem>
#include <string>
#include <vector>

#include "utils.hpp"
#include <gl_core_4_5.hpp>

struct shader_source {
  enum class location { file, embedded };
  location loc;
  std::string source_or_file_path;
};

enum class pipeline_stages {
  vertex = 1 << 0,
  geometry = 1 << 1,
  pixel = 1 << 2,
  domain = 1 << 3,
  hull = 1 << 4,
  compute = 1 << 5
};

ENUM_BIT_FLAGS_OPERATORS(pipeline_stages)

struct gl_vertex_attribute {
  unsigned slot;
  GLenum type;
  unsigned size;
  unsigned stride;
  bool normalized;
};

struct gl_blend_state {
  constexpr gl_blend_state() = default;
  constexpr gl_blend_state(bool enabled_, GLenum modeRGB_, GLenum modeAlpha_,
                           GLenum funcSrcRGB_, GLenum funcDstRGB_,
                           GLenum funcSrcAlpha_, GLenum funcDstAlpha_)
      : enabled{enabled_}, modeRGB{modeRGB_}, modeAlpha{modeAlpha_},
        funcSrcRGB{funcSrcRGB_}, funcDstRGB{funcDstRGB_},
        funcSrcAlpha{funcSrcAlpha_}, funcDstAlpha{funcDstAlpha_} {}

  bool enabled = true;
  GLenum modeRGB = gl::FUNC_ADD;
  GLenum modeAlpha = gl::FUNC_ADD;
  GLenum funcSrcRGB = gl::SRC_ALPHA;
  GLenum funcDstRGB = gl::ONE_MINUS_SRC_ALPHA;
  GLenum funcSrcAlpha = gl::ONE;
  GLenum funcDstAlpha = gl::ZERO;

  constexpr size_t hash() const {
    // TODO
    return 0;
  }
};

struct gl_depth_stencil_state {
  constexpr gl_depth_stencil_state() = default;
  constexpr gl_depth_stencil_state(bool depthTestEnable_,
                                   bool depthWriteEnable_, bool stencilEnable_,
                                   GLenum stencilFace_, GLenum stencilFunc_,
                                   GLint stencilRef_, GLuint stencilMask_,
                                   GLenum stencilOpSfail_,
                                   GLenum stencilOpDPFail_,
                                   GLenum stencilOpDPPass_)
      : depthTestEnable{depthTestEnable_}, depthWriteEnable{depthWriteEnable_},
        stencilEnable{stencilEnable_}, stencilFace{stencilFace_},
        stencilFunc{stencilFunc_}, stencilRef{stencilRef_},
        stencilMask{stencilMask_}, stencilOpSfail{stencilOpSfail_},
        stencilOpDPFail{stencilOpDPFail_}, stencilOpDPPass{stencilOpDPPass_} {}

  bool depthTestEnable = false;
  bool depthWriteEnable = false;
  bool stencilEnable = false;
  GLenum stencilFace = gl::FRONT_AND_BACK;
  GLenum stencilFunc = 0;
  GLint stencilRef = 0;
  GLuint stencilMask = 0xFFFFFFFF;
  GLenum stencilOpSfail = 0;
  GLenum stencilOpDPFail = 0;
  GLenum stencilOpDPPass = 0;
};

struct gl_rasterizer_state {
  constexpr gl_rasterizer_state() = default;
  constexpr gl_rasterizer_state(GLenum fillMode_) : fillMode{fillMode_} {}
  GLenum fillMode = gl::FILL;
};

struct gl_viewport {
  float x, y, w, h;
};

enum class gl_draw_state_mask {
  viewports = (1 << 0),
  draw_buffers = (1 << 1),
  scissor_rect = (1 << 2),
  blend_states = (1 << 3),
  rasterizer_state = (1 << 4),
  depth_stencil_state = (1 << 5)
};

struct gl_draw_state {
  // bit mask of valid states
  gl_draw_state_mask mask;
  GLenum prim_mode;
  std::vector<gl_blend_state> blend_states;
  std::vector<gl_viewport> viewports;
  gl_rasterizer_state rasterizer_state;
  gl_depth_stencil_state depth_stencil_state;
};

ENUM_BIT_FLAGS_OPERATORS(gl_draw_state_mask)

void bind_draw_state(const gl_draw_state &state);

struct pp_define {
  const char *define;
  const char *value;
};

// hard program limits
constexpr unsigned gl_max_vertex_buffers = 16;
constexpr unsigned gl_max_vertex_attributes = 16;

// indirectly visible to the user, shared
struct gl_graphics_pipeline {
  shader_source src;

  // enabled stages
  pipeline_stages stages;

  // preprocessor defines
  std::vector<pp_define> defines;

  // input layout descriptor
  std::vector<gl_vertex_attribute> attribs;

  // non-shader draw state
  gl_draw_state draw_state;

  // also: input layout, descriptor sets, etc.
  // cache: compiled pipeline(s)

  mutable GLuint gl_vao = 0;
  mutable GLuint gl_program = 0;

  static gl_graphics_pipeline compile_from_file(
      std::experimental::filesystem::path path_, std::initializer_list<pp_define> defines_,
      pipeline_stages stages_,
      std::initializer_list<gl_vertex_attribute> vertex_attributes_,
      const gl_depth_stencil_state &depth_stencil_state_ = {},
      const gl_rasterizer_state &rasterizer_state_ = {},
      std::initializer_list<gl_blend_state> blend_ = {});

  void do_load();
  void bind();
};

// indirectly visible to the user, shared
struct gl_compute_pipeline {
  shader_source src;

  // preprocessor defines
  std::vector<pp_define> defines;

  static gl_compute_pipeline
  compile_from_file(std::experimental::filesystem::path path_,
                    std::initializer_list<pp_define> defines_ = {});

  static gl_compute_pipeline compile_from_source(const char *src);

  // also: descriptor sets, etc.
  // cache: compiled pipeline(s)
  mutable GLuint gl_program = 0;

  void do_load();
  void bind();
};
