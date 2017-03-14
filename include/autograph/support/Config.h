#include <autograph/Config.h>

//////////////////////////////////////////////
// AG_SUPPORT_DLL
#if defined(AG_SUPPORT_DLL)
#if defined(_MSC_VER)
#if defined(AG_BUILD_SUPPORT_DLL)
#define AG_SUPPORT_API __declspec(dllexport)
#else
#define AG_SUPPORT_API __declspec(dllimport)
#endif
#else
#define AG_SUPPORT_API __attribute__((visibility("default")))
#endif
#else
#define AG_SUPPORT_API
#endif
