#include <autograph/Types.h>
#include <autograph/engine/ResourceManager.h>
#include <autograph/support/Debug.h>
#include <experimental/filesystem>

namespace ag {
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
ResourceManager::ResourceManager() {}

void ResourceManager::addResourceDirectory(std::string dir) {
  resourceDirectories_.push_back(std::move(dir));
}

span<const std::string> ResourceManager::getResourceDirectories() {
  return ag::span<const std::string>{resourceDirectories_};
}

std::string ResourceManager::findResourceFile(
    const char *id, ag::span<const char *const> allowedExtensions) {
  namespace fs = std::experimental::filesystem;
  for (auto &dir : resourceDirectories_) {
    fs::path baseDir{dir};
    for (auto ext : allowedExtensions) {
      auto fullPath = (baseDir / id).replace_extension(ext);
      if (fs::is_regular_file(fullPath)) {
        // got our file
        AG_DEBUG("{} -> {}", id, fullPath.string());
        return fullPath.string();
      } else {
        // AG_DEBUG("{} not found", fullPath.string());
      }
    }
  }
  AG_DEBUG("findResourceFile: {} not found", id);
  AG_DEBUG("    Tried directories:");
  for (auto &dir : resourceDirectories_) {
    AG_DEBUG("    - {}", dir);
  }
  AG_DEBUG("    Tried extensions:");
  for (auto ext : allowedExtensions) {
    AG_DEBUG("    - {}", ext);
  }
  return {};
}
}
