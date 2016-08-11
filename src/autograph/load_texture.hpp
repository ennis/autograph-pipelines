#pragma once
#include "format.hpp"
#include "texture.hpp"
#include "lazy_resource.hpp"
#include <experimental/filesystem>

namespace ag {

texture load_texture(std::experimental::filesystem::path path,
                     image_format tex_format = image_format::rgba8_unorm);

void save_texture(std::experimental::filesystem::path path, texture &tex);

// delayed loader for image files
struct texture_from_file_t {
	ag::texture
		operator()(const std::experimental::filesystem::path &p,
			ag::image_format tex_format = ag::image_format::rgba8_unorm) {
		return load_texture(p, tex_format);
	}
};

inline auto load_texture_lazy(std::experimental::filesystem::path path,
	image_format tex_format = image_format::rgba8_unorm)
{
	return make_lazy_resource<ag::texture, texture_from_file_t>(std::move(path), std::move(tex_format));
}

}