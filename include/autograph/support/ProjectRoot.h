#pragma once
#include <autograph/support/Config.h>
#include <string>

namespace ag 
{
AG_SUPPORT_API const std::string& getProjectRootDirectory();
AG_SUPPORT_API std::string getActualPath(const char* pathRelativeToProjectRoot);
}