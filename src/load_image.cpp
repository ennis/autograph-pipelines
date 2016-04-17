#include "load_image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdexcept>
#include <cppformat/format.h>
#include "image_node.hpp"

image load_image(std::experimental::filesystem::path path)
{
	auto pathstr = path.string();
	int x, y, comp;
	auto raw_data = stbi_load(pathstr.c_str(), &x, &y, &comp, 4);
	if (!raw_data)
		throw std::runtime_error{ fmt::format("Missing or corrupt image file: {}", pathstr) };
	image_desc desc;
	desc.width = x;
	desc.height = y;
	desc.depth = 1;
	desc.format = image_format::rgba8_unorm;
	desc.num_mips = 1;
	desc.storage_hint_ = storage_hint::device;
	desc.dimensions = image_dimensions::img_2d;
	return image{ image_node::create(desc, raw_data) };
}
