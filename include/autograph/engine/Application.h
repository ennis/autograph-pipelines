#pragma once
#include <autograph/Types.h>
#include <autograph/support/Span.h>

namespace ag
{
	AG_API std::string FindResourceFile(const char* id, span<const char*> allowedExtensions);
	AG_API void AddResourceDirectory(std::string fullPath);
	AG_API ag::span<const std::string> GetResourceDirectories();
}