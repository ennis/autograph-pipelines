#pragma once

//////////////////////////////////////////////
#define AG_DEBUG_OUTPUT
#define AG_TRACE_OUTPUT
#define AG_FRAME_TRACE_OUTPUT

//////////////////////////////////////////////
#if !defined(AG_IMPLEMENTATION)
#define AG_IMPLEMENTATION 1
#endif

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
// AG_API
#if defined(AG_DLL)
#if defined(_MSC_VER)
#if AG_IMPLEMENTATION
#define AG_API2 __declspec(dllexport)
#else
#define AG_API2 __declspec(dllimport)
#endif
#else
#define AG_API2 __attribute__((visibility("default")))
#endif
#else
#define AG_API2
#endif

#define AG_API AG_API2 AG_REFLECT
#define AG_NATIVE_API AG_API2

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
