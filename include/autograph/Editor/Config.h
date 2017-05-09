#include <autograph/Config.h>

//////////////////////////////////////////////
// 
#if defined(AG_EDITOR_DLL)
#if defined(_MSC_VER)
#if defined(AG_BUILD_EDITOR_DLL)
#define AG_EDITOR_API2 __declspec(dllexport)
#else
#define AG_EDITOR_API2 __declspec(dllimport)
#endif
#else
#define AG_EDITOR_API2 __attribute__((visibility("default")))
#endif
#else
#define AG_EDITOR_API2
#endif

#ifdef __REFLECTION_autograph_editor__
#define AG_EDITOR_API AG_EDITOR_API2 AG_REFLECT
#else
#define AG_EDITOR_API AG_EDITOR_API2
#endif