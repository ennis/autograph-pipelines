#include <autograph/Config.h>
#include <autograph/support/ProjectRoot.h>
#include <cppformat/format.h>
#include <experimental/filesystem>
#include <iostream>

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
                fmt::print(std::cerr, "Project root directory not found: {}\n",
                           ag::projectName);
                throw std::runtime_error("Project root directory not found");
              }
            }
          }
        }
      }
    }
    found = true;
    path = path / ag::projectName;
    pathstr = path.string();
    fmt::print(std::clog, "Project root directory: {}\n", pathstr);
  }
  return pathstr;
}
}