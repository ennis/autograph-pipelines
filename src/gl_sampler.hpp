#pragma once
#include <gl_core_4_5.hpp>
#include "gl_handle.hpp"

struct gl_sampler_desc {
	GLenum tex_wrap_s;
	GLenum tex_wrap_t;
	GLenum tex_wrap_r;
	GLenum min_filter;
	GLenum mag_filter;
};

struct sampler_deleter {
	void operator()(GLuint sam_obj) { gl::DeleteSamplers(1, &sam_obj); }
};

struct gl_sampler {
	gl_sampler() {}
	gl_sampler(const gl_sampler_desc &desc) : desc_{ desc } {}
	~gl_sampler() {}

	GLuint object() {
		if (!obj_)
			init();
		return obj_.get();
	}

	gl_handle<sampler_deleter> obj_;
	gl_sampler_desc desc_;

private:
	void init();
};
