#pragma once

#include <gl_core_4_5.hpp>
#include <string>
#include <vector>
#include <autograph/utils.hpp>

struct shader_source {
	enum class location {
		file, embedded
	};
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

struct gl_sampler {
  GLenum minFilter = gl::NEAREST;
  GLenum magFilter = gl::NEAREST;
  GLenum addrU = gl::CLAMP_TO_EDGE;
  GLenum addrV = gl::CLAMP_TO_EDGE;
  GLenum addrW = gl::CLAMP_TO_EDGE;

  constexpr size_t hash() const {
    // TODO
	  return 0;
  }
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

struct pp_define {
  const char *define;
  const char *value;
};

// program limits
constexpr unsigned gl_max_vertex_buffers = 16;
constexpr unsigned gl_max_vertex_attributes = 16;

// indirectly visible to the user, shared
struct graphics_pipeline_program {
  shader_source src;

  // enabled stages
  pipeline_stages stages;

  // preprocessor defines
  std::vector<pp_define> defines;

  // input layout descriptor
  std::vector<gl_vertex_attribute> attribs;

  gl_rasterizer_state rs;
  gl_depth_stencil_state dss;
  // blending units
  std::vector<gl_blend_state> blend;

  // also: input layout, descriptor sets, etc.
  // cache: compiled pipeline(s)

  mutable GLuint gl_vao;
  mutable GLuint gl_program;

  static graphics_pipeline_program compile_from_file(
      const char *file_name_, std::initializer_list<pp_define> defines_,
      pipeline_stages stages_,
      std::initializer_list<gl_vertex_attribute> vertex_attributes_,
      const gl_depth_stencil_state &depth_stencil_state_ = {},
      const gl_rasterizer_state &rasterizer_state_ = {},
      std::initializer_list<gl_blend_state> blend_ = {});

  //graphics_pipeline_program compile_variant(std::initializer_list<pp_define> defines_);
};

// indirectly visible to the user, shared
struct compute_pipeline_program {
  shader_source src;

  static compute_pipeline_program
  compile_from_file(const char *file_name_,
                    std::initializer_list<pp_define> defines_);
  static compute_pipeline_program
	  compile_from_source(const char *src);

  // also: descriptor sets, etc.
  // cache: compiled pipeline(s)
  mutable GLuint gl_program;
};
