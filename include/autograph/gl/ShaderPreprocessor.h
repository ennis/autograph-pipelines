#pragma once
#include <autograph/Config.h>
#include <string>
#include <vector>

namespace ag {
namespace gl {
struct MultiCompileDirective {
  std::vector<std::string> kw;
};

AG_API std::vector<MultiCompileDirective> getMultiCompileDirectives(
    const char *srcpath);
AG_API std::string preprocessShaderFile(const char* srcpath,
                       const char **defines, int ndefines);
}
}
