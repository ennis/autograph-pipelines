#include <string>

#ifdef __REFLECTION_PARSER__
#define REFLECT [[cxxr::reflect]]
#define NOREFLECT [[cxxr::noreflect]]
#define META(V) [[cxxr::meta(V)]]
#else
#define REFLECT
#define NOREFLECT
#define META(V)
#endif

struct DisplayName {
  const char *name;
};

struct Documentation {};

struct REFLECT
META(DisplayName{"Reflected structure example"})
META(Documentation{"This is documentation"})
ReflectedStruct 
{
	META(DisplayName{ "The first value" })
	int value1;

	META(DisplayName{ "The second value" })
	double value2;

	META(DisplayName{ "The third value" })
	std::string value3;
};