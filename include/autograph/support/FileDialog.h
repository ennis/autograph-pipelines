#pragma once
#include <autograph/support/Config.h>
#include <autograph/support/Optional.h>
#include <string>
#include <vector>

namespace ag {
AG_SUPPORT_API optional<std::string> openFileDialog(const char *filters,
                                            const char *defaultPath);
AG_SUPPORT_API optional<std::vector<std::string>>
openFileDialogMultiple(const char *filters, const char *defaultPath);
AG_SUPPORT_API optional<std::string> saveFileDialog(const char *filters,
                                            const char *defaultPath);
AG_SUPPORT_API optional<std::string> pickFolderDialog(const char *defaultPath);
}
