#pragma once

#ifdef __REFLECTION_PARSER__
#define REFLECT [[cxxr::reflect]]
#define NOREFLECT [[cxxr::noreflect]]
#define META(...) [[cxxr::meta(__VA_ARGS__)]]
#else 
#define REFLECT
#define NOREFLECT
#define META(...)
#endif

namespace meta
{
	struct friendly_name {
		const char* name;
	};
}

#define META_FRIENDLY_NAME(N) META(meta::friendly_name{N})
