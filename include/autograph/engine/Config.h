#include <autograph/Config.h>

//////////////////////////////////////////////
// 
#if defined(AG_ENGINE_DLL)
#if defined(_MSC_VER)
#if defined(AG_BUILD_ENGINE_DLL)
#define AG_ENGINE_API2 __declspec(dllexport)
#else
#define AG_ENGINE_API2 __declspec(dllimport)
#endif
#else
#define AG_ENGINE_API2 __attribute__((visibility("default")))
#endif
#else
#define AG_ENGINE_API2
#endif

#ifdef __REFLECTION_autograph_engine__
#define AG_ENGINE_API AG_ENGINE_API2 AG_REFLECT
#else
#define AG_ENGINE_API AG_ENGINE_API2
#endif