#pragma once
#include <autograph/Config.h>
#include <autograph/support/Optional.h>
#include <string>
#include <vector>

namespace ag {
AG_API optional<std::string> openFileDialog(const char *filters,
                                            const char *defaultPath);
AG_API optional<std::vector<std::string>>
openFileDialogMultiple(const char *filters, const char *defaultPath);
AG_API optional<std::string> saveFileDialog(const char *filters,
                                            const char *defaultPath);
AG_API optional<std::string> pickFolderDialog(const char *defaultPath);
}
