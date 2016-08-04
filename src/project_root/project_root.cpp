#include "project_root.hpp"
#include <cppformat/format.h>

std::experimental::filesystem::path project_root() 
{
	namespace fs = std::experimental::filesystem;
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
  return path / proj_name;
}
