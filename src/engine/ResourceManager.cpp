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

std::string ResourceManager::findResourceFile(const char *id, ag::span<const char * const> allowedExtensions)
{
    return findResourceFileWithPrefixes(id, allowedExtensions, {});
}

std::string ResourceManager::findResourceFileWithPrefixes(
    const char *id, ag::span<const char *const> allowedExtensions,
    ag::span<const char *const> prefixes) {
  namespace fs = std::experimental::filesystem;
  std::string ret;

  // first, check if ID is a regular filesystem path
  fs::path path{ id };
  if (fs::is_regular_file(path)) {
	  return id;
  }

  // ID is not a filesystem path
  auto findWithExts = [&](fs::path &baseDir) {
    for (auto ext : allowedExtensions) {
      auto fullPath = (baseDir / id).replace_extension(ext);
      if (fs::is_regular_file(fullPath)) {
        // got our file
        AG_DEBUG("{} -> {}", id, fullPath.string());
        ret = fullPath.string();
        return true;
      }
    }
    return false;
  };

  for (auto &dir : resourceDirectories_) {
    fs::path baseDir{dir};
    if (prefixes.empty()) {
      if (findWithExts(baseDir))
        return ret;
    } else {
      for (auto prefix : prefixes) {
        auto baseDirWithPrefix = baseDir / prefix;
        if (findWithExts(baseDirWithPrefix))
          return ret;
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
  if (!prefixes.empty()) {
    AG_DEBUG("    Tried prefixes:");
    for (auto prefix : prefixes) {
      AG_DEBUG("    - {}", prefix);
    }
  }
  return {};
}
}
