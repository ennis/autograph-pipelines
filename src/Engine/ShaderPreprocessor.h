#pragma once
#include <autograph/Core/Support/Span.h>
#include <autograph/Core/Support/string_view.h>
#include <autograph/Gfx/ShaderStage.h>
#include <string>

namespace ag {
struct ShaderSource {
  std::string path;
  std::string source;
};

struct ShaderSources {
  ShaderSource vertexShader;
  ShaderSource fragmentShader;
  ShaderSource geometryShader;
  ShaderSource tessControlShader;
  ShaderSource tessEvalShader;
  ShaderSource computeShader;
};

void preprocessMultiShaderSources(ShaderSources &inOutShaderSources,
                                  span<const char *> macros,
                                  span<const char *> includePaths);

std::string preprocessShaderSource(string_view source, const char *path,
                                   ShaderStage stage, span<const char *> macros,
                                   span<const char *> includePaths);

} // namespace ag