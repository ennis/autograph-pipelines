#pragma once
#include <autograph/support/Span.h>
#include <sol/state.hpp>

namespace ag {
class AG_API ResourceManager {
public:
  ResourceManager();
  void addResourceDirectory(std::string dir);
  std::string findResourceFile(const char *id,
                               ag::span<const char *const> allowedExtensions);
  std::string findResourceFileWithPrefixes(const char *id,
                               ag::span<const char *const> allowedExtensions,
                               ag::span<const char *const> prefixes);
  span<const std::string> getResourceDirectories();

private:
  std::vector<std::string> resourceDirectories_;
};
}
