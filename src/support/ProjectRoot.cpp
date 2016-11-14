#include <autograph/Config.h>
#include <autograph/support/ProjectRoot.h>
#include <autograph/support/Debug.h>
#include <fmt/format.h>
#include <experimental/filesystem>
#include <string>

namespace ag {
AG_API const std::string &getProjectRootDirectory() {
  namespace fs = std::experimental::filesystem;
  static bool found = false;
  static std::string pathstr;
  if (!found) {
    auto path = fs::current_path();
    if (!fs::is_directory(path / ag::projectName)) {
      path = path.parent_path();
      if (!fs::is_directory(path / ag::projectName)) {
        path = path.parent_path();
        if (!fs::is_directory(path / ag::projectName)) {
          path = path.parent_path();
          if (!fs::is_directory(path / ag::projectName)) {
            path = path.parent_path();
            if (!fs::is_directory(path / ag::projectName)) {
              path = path.parent_path();
              if (!fs::is_directory(path / ag::projectName)) {
				  ag::failWith(fmt::format("Project root directory not found: {}", ag::projectName));
              }
            }
          }
        }
      }
    }
    found = true;
    path = path / ag::projectName;
    pathstr = path.string();
    AG_DEBUG("Project root directory: {}", pathstr);
  }
  return pathstr;
}

AG_API std::string getActualPath(const char* pathRelativeToProjectRoot) 
{
  namespace fs = std::experimental::filesystem;
  fs::path path {getProjectRootDirectory()};
  path = path / pathRelativeToProjectRoot;
  return path.string();
}

}