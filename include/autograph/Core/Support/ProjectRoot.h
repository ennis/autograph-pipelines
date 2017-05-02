#pragma once
#include <autograph/Core/Exports.h>
#include <string>

namespace ag 
{
AG_CORE_API const std::string& getProjectRootDirectory();
AG_CORE_API std::string getActualPath(const char* pathRelativeToProjectRoot);
}