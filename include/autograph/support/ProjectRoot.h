#pragma once
#include <autograph/Config.h>
#include <string>

namespace ag 
{
AG_API const std::string& getProjectRootDirectory();
AG_API std::string getActualPath(const char* pathRelativeToProjectRoot);
}