#pragma once
#include "format.hpp"
#include "texture.hpp"
#include <experimental/filesystem>

namespace ag {

texture load_texture(std::experimental::filesystem::path path,
                     image_format tex_format = image_format::rgba8_unorm);

struct image_file {
  image_file() = default;
  image_file(std::experimental::filesystem::path path);

  texture &get_texture();

  bool loaded_ = false;
  std::experimental::filesystem::path path_;
  texture tex_;
};

void save_texture(std::experimental::filesystem::path path, texture &tex);
}