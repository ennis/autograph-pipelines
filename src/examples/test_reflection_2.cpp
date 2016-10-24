//#include "test_reflection_include.hpp"

[[cxxr::reflect]];	// enable reflection generation for this translation unit

struct[[cxxr::reflect]] Attr {
  const char *str;
  int i;
};

namespace[[cxxr::noreflect]] N {
	 void func();
     void func(int i);

inline bool inline_func() { return true; }

class[[cxxr::reflect]]  AnotherClass {};
}