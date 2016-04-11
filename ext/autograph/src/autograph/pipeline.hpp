#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "gl_types.hpp"

namespace ag {

	struct VertexAttribute {
		unsigned slot;
		GLenum type;
		unsigned size;
		unsigned stride;
		bool normalized;
	};

	struct GraphicsPipelineDesc {
		const char* VSSource = nullptr;
		const char* GSSource = nullptr;
		const char* PSSource = nullptr;
		const char* DSSource = nullptr;
		const char* HSSource = nullptr;
		gsl::span<const VertexAttribute> vertexAttribs;
		GLRasterizerState rasterizerState;
		GLDepthStencilState depthStencilState;
		GLBlendState blendState;
	};

	struct ComputePipelineDesc {
		const char* CSSource = nullptr;
	};

  struct vao_deleter {
    void operator()(GLuint vao) {
        gl::DeleteVertexArrays(1, &vao);
    }
  };

  struct program_deleter {
    void operator()(GLuint obj) {
        gl::DeleteProgram(obj);
    }
  };

struct GraphicsPipeline 
{
  gl_handle<program_deleter> program;
  gl_handle<vao_deleter> vao;
    GLRasterizerState rasterizer_state;
    GLDepthStencilState depth_stencil_state;
    GLBlendState blend_state;

};

struct ComputePipeline 
{
  gl_handle<program_deleter> program;
};
}

#endif
