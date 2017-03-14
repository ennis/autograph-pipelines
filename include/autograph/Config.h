#pragma once

//////////////////////////////////////////////
#define AG_DEBUG_OUTPUT
#define AG_TRACE_OUTPUT
#define AG_FRAME_TRACE_OUTPUT

//////////////////////////////////////////////
#ifdef __REFLECTION_PARSER__
#define AG_REFLECT [[cxxr::reflect]]
#else
#define AG_REFLECT
#endif

//////////////////////////////////////////////
#ifdef __REFLECTION_PARSER__
#define AG_META(...) [[cxxr::meta(__VA_ARGS__)]]
#else
#define AG_META(...)
#endif

//////////////////////////////////////////////
// 
#if defined(AG_CORE_DLL)
#if defined(_MSC_VER)
#if defined(AG_BUILD_CORE_DLL)
#define AG_CORE_API __declspec(dllexport)
#else
#define AG_CORE_API __declspec(dllimport)
#endif
#else
#define AG_CORE_API __attribute__((visibility("default")))
#endif
#else
#define AG_CORE_API
#endif

//////////////////////////////////////////////
#if defined(_MSC_VER)
#if AG_IMPLEMENTATION
#define AG_LUA_API extern "C" __declspec(dllexport)
#else
#define AG_LUA_API extern "C"
#endif
#else
#define AG_LUA_API extern "C" __attribute__((visibility("default")))
#endif

//////////////////////////////////////////////
namespace ag {
static constexpr const char projectName[] = "autograph-pipelines";
}

//////////////////////////////////////////////
// Track GL objects for the devtools
#define AG_TRACK_GL_OBJECTS 1
