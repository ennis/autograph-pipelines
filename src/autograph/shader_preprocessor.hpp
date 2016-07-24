#pragma once
#include <experimental/filesystem>
#include <string>
#include <vector>

namespace ag {
namespace shaderpp {
struct multi_compile_directive {
  std::vector<std::string> kw;
};

std::vector<multi_compile_directive> get_multi_compile_directives(
    const std::experimental::filesystem::path &srcpath);
std::string preprocess(const std::experimental::filesystem::path &srcpath,
                       const char **defines, int ndefines);
}
}
