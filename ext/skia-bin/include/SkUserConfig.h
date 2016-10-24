// SkUserConfig generated by CMake.
#ifndef SkUserConfig_DEFINED
#define SkUserConfig_DEFINED

#if 1
    #define SK_RELEASE
    #undef  SK_DEBUG
#else
    #define SK_DEBUG
    #undef  SK_RELEASE
#endif

/*  Change the ordering to work in X windows.
 */
#if 0
    #define SK_R32_SHIFT    16
    #define SK_G32_SHIFT    8
    #define SK_B32_SHIFT    0
    #define SK_A32_SHIFT    24
#endif

#define SK_MESA 0

#endif
