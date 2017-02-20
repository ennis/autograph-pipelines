#pragma once
#include <autograph/support/Span.h>
#include <experimental/filesystem>

namespace ag {

// Uniquely identifies a resource on the filesystem, on the network, in an
// archive file, or in memory
class AG_API ResourceID {
public:
  ResourceID(const char *str) : str_{str} {}
  ResourceID(std::string str) : str_{str} {}
  ResourceID(const std::experimental::filesystem::path &p)
      : str_{p.string()}, path_{p} {}

  // returns true if the resource exists
  bool isValid();
  // returns true if the ID corresponds to a file in the filesystem
  bool isFilesystemResource();
  //
  bool isInternalResource();
  //
  auto getResourcePath() -> std::string;
  //
  auto getFilesystemPath() -> std::experimental::filesystem::path {
    return path_;
  }
  auto getFilesystemPathString() -> std::string { return path_.string(); }
  // get the internal part
  auto getPrivateID() -> std::string;
  //
  auto getCStr() const -> const char * { return str_.c_str(); }
  //
  auto getString() const -> const std::string & { return str_; }

private:
  // Could be copy-on-write
  std::string str_;
  // can be empty (if it's an URL)
  std::experimental::filesystem::path path_;
};

class AG_API ResourceManager {
public:
  ResourceManager();
  void addResourceDirectory(std::string dir);
  std::string findResourceFile(const char *id,
                               ag::span<const char *const> allowedExtensions);
  std::string
  findResourceFileWithPrefixes(const char *id,
                               ag::span<const char *const> allowedExtensions,
                               ag::span<const char *const> prefixes);
  span<const std::string> getResourceDirectories();

private:
  std::vector<std::string> resourceDirectories_;
};
}
