#include <autograph/Config.h>

//////////////////////////////////////////////
// AG_GFX_API
#if defined(AG_GL_DLL)
#if defined(_MSC_VER)
#if defined(AG_BUILD_GL_DLL)
#define AG_GFX_API __declspec(dllexport)
#else
#define AG_GFX_API __declspec(dllimport)
#endif
#else
#define AG_GFX_API __attribute__((visibility("default")))
#endif
#else
#define AG_GFX_API
#endif
