#pragma once
#include <autograph/Types.h>
#include <autograph/support/Span.h>

namespace ag
{
	AG_API std::string findResourceFile(const char* id, span<const char*> allowedExtensions);
	AG_API void addResourceDirectory(std::string fullPath);
	AG_API ag::span<const std::string> getResourceDirectories();
}