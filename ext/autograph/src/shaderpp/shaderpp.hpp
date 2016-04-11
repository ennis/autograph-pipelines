#ifndef SHADERPP_HPP
#define SHADERPP_HPP

#include <gsl.h>
#include <string>
#include "../autograph/utils.hpp"

enum class pipeline_stages {
  vertex = 1 << 0,
  geometry = 1 << 1,
  pixel = 1 << 2,
  domain = 1 << 3,
  hull = 1 << 4,
  compute = 1 << 5
};

ENUM_BIT_FLAGS_OPERATORS(pipeline_stages)

namespace shaderpp {

class ShaderSource {
public:
  ShaderSource(const char *path_);

  const std::string &getOriginalSource() const { return source; }

  std::string preprocess(pipeline_stages stage, gsl::span<const char *> defines,
                         gsl::span<const char *> include_paths);

private:
  std::string source;
  std::string path;
};
}

#endif
