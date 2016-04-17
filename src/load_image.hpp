#pragma once

#include <experimental/filesystem>
#include "image.hpp"

image load_image(std::experimental::filesystem::path path);