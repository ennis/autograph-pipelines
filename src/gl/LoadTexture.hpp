#pragma once
#include <experimental/filesystem>
#include <autograph/ImageFormat.h>
#include <autograph/gl/Texture.hpp>
//#include "LazyResource.hpp"

namespace ag {
	namespace gl {
		/*Texture loadTexture(std::experimental::filesystem::path path,
			ImageFormat tex_format = ImageFormat::RGBA8_Unorm);

		void saveTexture(std::experimental::filesystem::path path, Texture &tex);

		// delayed loader for image files
		struct texture_from_file_t {
			ag::Texture
				operator()(const std::experimental::filesystem::path &p,
					ag::ImageFormat tex_format = ag::ImageFormat::RGBA8_Unorm) {
				return loadTexture(p, tex_format);
			}
		};

		inline auto loadTextureLazy(std::experimental::filesystem::path path,
			ImageFormat tex_format = ImageFormat::RGBA8_Unorm)
		{
			return make_lazy_resource<ag::Texture, texture_from_file_t>(std::move(path), std::move(tex_format));
		}*/

	}
}