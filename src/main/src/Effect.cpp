#include "Effect.h"
#include <cppformat/format.h>
#include <iostream>

namespace ag {
namespace fx {
/*struct TextureDesc
{
        ImageDimensions dimensions;
        ImageFormat format;
        int width;
        int height;
        int depth;
};

struct BufferDesc
{
        gl::BufferUsage usage;
        size_t size;
};*/

Pipeline::Pipeline() {
  L["Texture"] = L.create_table_with(
      "Texture1D", ImageDimensions::Image1D, "Texture1DArray",
      ImageDimensions::Image1DArray, "Texture2D", ImageDimensions::Image2D,
      "Texture2DArray", ImageDimensions::Image2DArray, "Texture3D",
      ImageDimensions::Image3D, "TextureCubeMap",
      ImageDimensions::ImageCubeMap);

  // expose this class to lua
  //L.new_usertype<Pipeline>("Pipeline", "new", sol::no_constructor,
  //                        "CreateTexture2D", &Pipeline::createTexture2D);
}

Pipeline::~Pipeline() {}

void Pipeline::setConfig(const char *name, int value) { L[name] = value; }

void Pipeline::setConfig(const char *name, float value) { L[name] = value; }

void Pipeline::setConfig(const char *name, bool value) { L[name] = value; }

void Pipeline::loadScript(const char *path) {

  L.script_file(path);

  // sol::table passes = L["passes"];

  // for (auto& p : passes) {
  //  fmt::print("Pass {}\n", p.first.as<std::string>());
  //}
}

gl::Texture &Pipeline::createTexture2D(const char *name, ImageFormat fmt, int w,
                                       int h, int numMips) {
  auto &a = textures[name];
  if (!a) {
    // no texture with this name yet, create one
    a = std::make_unique<gl::Texture>(
        gl::Texture::create2D(w, h, fmt, numMips));
    fmt::print(std::cerr, "createTexture2D {} {}x{} {}, {} mip level(s)", name,
               w, h, getImageFormatInfo(fmt).name, numMips);
  } else {
    // another texture exists by the name name
    fmt::print(std::cerr, "{}: a texture with the same name already exists\n");
  }
  return *a;
}

gl::Texture *Pipeline::getTexture(const char *name) {
  auto match = textures.find(name);
  if (match == textures.end()) {
    return nullptr;
  }
  return match->second.get();
}

Pass *Pipeline::getPass(const char *name) {
  auto match = passes.find(name);
  if (match == passes.end()) {
    return nullptr;
  }
  return match->second.get();
}

gl::Texture &Pipeline::createTexture3D(const char *name, ImageFormat fmt, int w,
                                       int h, int d, int numMips) {
  throw std::logic_error("Unimplemented");
}

// Lua API
AG_LUA_API ag::gl::Texture *FXCreateTexture2D(ag::fx::Pipeline *pPipeline,
                                              const char *name,
                                              ag::ImageFormat imgFmt, int width,
                                              int height, int numMips) {
  return nullptr;
}

AG_LUA_API ag::gl::Texture *FXCreateTexture3D(ag::fx::Pipeline *pPipeline,
                                              const char *name,
                                              ag::ImageFormat imgFmt, int width,
                                              int height, int depth,
                                              int numMips) {
  return nullptr;
}

AG_LUA_API ag::fx::Pass *FXCreatePass(ag::fx::Pipeline *pPipeline,
                                      const char *name,
                                      ag::fx::PassType pass_type) {
  return nullptr;
}

AG_LUA_API ag::gl::Sampler *FXCreateSampler(ag::fx::Pipeline *pPipeline,
                                            const char *name,
                                            const ag::gl::SamplerDesc *desc) {
  return nullptr;
}

AG_LUA_API void FXPassBindTexture(ag::fx::Pipeline *pPipeline,
                                  ag::fx::Pass *pass, int slot,
                                  ag::gl::Texture *tex) {}

AG_LUA_API void FXPassBindTextureImage(ag::fx::Pipeline *pPipeline,
                                       ag::fx::Pass *pass, int slot,
                                       ag::gl::Texture *tex) {}

AG_LUA_API void FXPassBindSampler(ag::fx::Pipeline *pPipeline,
                                  ag::fx::Pass *pass, int slot,
                                  ag::gl::Sampler *sampler) {}
}
}