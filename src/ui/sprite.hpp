#pragma once 
#include "../gl_texture.hpp"
#include <experimental/filesystem>

namespace ui 
{
	class sprite 
	{
	public:
		sprite(gl_texture& tex) : tex_{tex}
		{}

		glm::ivec2 size() const {
			// wrong!
			return tex_.size();
		}

	private:
		// texture atlas
		gl_texture& tex_;
		// texture coordinates
		float x_right_{0.0f};
		float y_top_{1.0f};
		float x_left_{1.0f};
		float y_bottom_{0.0f};
	};
}