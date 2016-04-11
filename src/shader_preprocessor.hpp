#pragma once
#include <vector>
#include <string>

struct multi_compile_directive {
	std::vector<std::string> kw;
};

std::vector<multi_compile_directive> get_multi_compile_directives(const char *srcpath);
std::string preprocess(const char *srcpath, const char **defines, int ndefines);
