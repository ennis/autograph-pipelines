#pragma once
#include "gl_handle.hpp"

namespace ag 
{
	struct vertex_array_deleter 
	{
  		void operator()(GLuint obj) { glDeleteVertexArrays(1, &obj); }
	};

	struct vertex_attribute {
		unsigned slot;
		GLenum type;
		unsigned size;
		unsigned stride;
		bool normalized;
	};

	class vertex_array 
	{
	public:
		vertex_array() = default;
		vertex_array(std::initializer_list<vertex_attribute> attribs);
		void initialize(std::initializer_list<vertex_attribute> attribs);

		GLuint object() const { return obj_.get(); }

	private:
		gl_handle<vertex_array_deleter> obj_;
	};
}