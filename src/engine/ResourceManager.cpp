#include <autograph/Types.h>
#include <autograph/engine/ResourceManager.h>
#include <autograph/support/Debug.h>
#include <autograph/support/string_view.h>
#include <experimental/filesystem>

namespace ag {

	namespace fs = std::experimental::filesystem;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

namespace ResourceManager {
static std::vector<std::string> resourceDirectories_;

// returns the main path part of the ID, or the empty string if it has none
std::string getPathPart(const char *id) {
  ag::string_view idstr{id};
  return idstr.substr(0, idstr.find_last_of('$')).to_string();
}

std::string getPathPart(const std::string &id) {
  return getPathPart(id.c_str());
}

// returns the subpath part of the ID, or the empty string if it has none
std::string getSubpathPart(const char *id) {
  ag::string_view idstr{id};
  auto p = idstr.find_last_of('$');
  if (p == std::string::npos) {
    return {};
  } else {
    return idstr.substr(p + 1).to_string();
  }
}

std::string getSubpathPart(const std::string &id) {
  return getSubpathPart(id.c_str());
}

std::string getParentPath(const char *id) {
  fs::path path = getPathPart(id);
  return path.parent_path().generic_string();
}

std::string getParentPath(const std::string &id) {
  return getParentPath(id.c_str());
}

void addResourceDirectory(const std::string &fullPath) {
  addResourceDirectory(fullPath.c_str());
}

void addResourceDirectory(const char *fullPath) {
  resourceDirectories_.emplace_back(fullPath);
}

int getResourceDirectoriesCount() {
  return (int)resourceDirectories_.size();
}

std::string getResourceDirectory(int index) {
  return resourceDirectories_[index];
}

std::string getFilesystemPath(const char *id) {
  return getFilesystemPath(id, {});
}

std::string getFilesystemPath(const std::string &id) {
  return getFilesystemPath(id.c_str());
}

std::string getFilesystemPath(const char *id,
                                     ag::span<const char *const> prefixes) {
  namespace fs = std::experimental::filesystem;

  std::string pathPart = getPathPart(id);
  std::string ret;

  // first, check if ID is a well-specified filesystem path
  fs::path path{pathPart};
  if (fs::is_regular_file(path)) {
    return pathPart;
  }

  for (auto &dir : resourceDirectories_) {
    fs::path baseDir{dir};
    if (prefixes.empty()) {
      auto fullPath = baseDir / pathPart;
      if (fs::is_regular_file(fullPath)) {
        // got our file
        AG_DEBUG("{} -> {}", pathPart, fullPath.string());
        ret = fullPath.string();
        return ret;
      }
    } else {
      for (auto prefix : prefixes) {
        auto fullPath = baseDir / prefix / pathPart;
        if (fs::is_regular_file(fullPath)) {
          // got our file
          AG_DEBUG("{} -> {}", pathPart, fullPath.string());
          ret = fullPath.string();
          return ret;
        }
      }
    }
  }

  AG_DEBUG("findResourceFile: {} not found", pathPart);
  AG_DEBUG("    Tried directories:");
  for (auto &dir : resourceDirectories_) {
    AG_DEBUG("    - {}", dir);
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
}
