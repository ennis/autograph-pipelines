#include "Effect.h"
#include <cppformat/format.h>

namespace ag {
namespace fx {

  void Effect::setConfig(const char *name, int value)
  {
	  L[name] = value;
  }

  void Effect::setConfig(const char *name, float value)
  {
	  L[name] = value;
  }

  void Effect::setConfig(const char *name, bool value)
  {
	  L[name] = value;
  }

  void Effect::loadFromFile(const char * path)
  {
	  L.script_file(path);

	  sol::table resources = L["resources"];
	  // go through the table of owned resources
	  for (auto& p : resources) {
		  auto table = p.second.as<sol::table>();
		  auto type = table["type"].get_or<std::string>("");
		  if (type == "texture") {
			  // we have a texture
		  }
		  else if (type == "buffer") {
			  // allocate a temp buffer
		  }
		  else if (type == "sampler") {
			  // samplers
		  }
	  }

	  sol::table passes = L["passes"];

	  for (auto& p : passes) {
		  fmt::print("Pass {}\n", p.first.as<std::string>());
	  }

  }

}
}