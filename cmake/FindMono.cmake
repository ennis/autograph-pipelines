
find_path(MONO_INCLUDE_DIR mono/jit/jit.h
          HINTS ${MONO_ROOT}/include/mono-2.0 )

find_library(MONO_LIBRARY NAMES mono-2.0
             HINTS ${MONO_ROOT}/lib )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Mono DEFAULT_MSG MONO_LIBRARY MONO_INCLUDE_DIR)
