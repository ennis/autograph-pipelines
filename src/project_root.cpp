#include "project_root.hpp"
#include <cppformat/format.h>

namespace fs = std::experimental::filesystem;

constexpr const char proj_name[] = "autograph-pipelines";

fs::path project_root() {
  static bool found = false;
  static auto path = fs::current_path();
  if (!found) {
    if (!fs::is_directory(path / proj_name)) {
      path = path.parent_path();
      if (!fs::is_directory(path / proj_name)) {
        path = path.parent_path();
        if (!fs::is_directory(path / proj_name)) {
          path = path.parent_path();
          if (!fs::is_directory(path / proj_name)) {
            path = path.parent_path();
            if (!fs::is_directory(path / proj_name)) {
              path = path.parent_path();
              if (!fs::is_directory(path / proj_name)) {
                throw std::runtime_error(fmt::format(
                    "project root directory not found: {}", proj_name));
              }
            }
          }
        }
      }
    }
    found = true;
  }
  return std::move(path / proj_name);
}
