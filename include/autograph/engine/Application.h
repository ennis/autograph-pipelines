#pragma once
#include <autograph/Types.h>
#include <autograph/support/Span.h>

namespace ag
{
	AG_API std::string getParentDirectory(const char* id);
	AG_API std::string findResourceFile(const char* id, span<const char*> allowedExtensions);
	AG_API std::string findResourceFileWithPrefixes(const char* id, span<const char*> allowedExtensions, span<const char*> prefixes);
	AG_API void addResourceDirectory(std::string fullPath);
	AG_API ag::span<const std::string> getResourceDirectories();
}