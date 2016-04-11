#include "shaderpp.hpp"

#include <fstream>
#include <sstream>

#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <boost/wave/preprocessing_hooks.hpp>

namespace shaderpp {
	namespace {
		std::string loadSource(const char* path) {
			std::ifstream fileIn(path, std::ios::in);
			if (!fileIn.is_open()) {
				std::cerr << "Could not open file " << path << std::endl;
				throw std::runtime_error("Could not open file");
			}
			std::string str;
			str.assign((std::istreambuf_iterator<char>(fileIn)),
				std::istreambuf_iterator<char>());
			return str;
		}

		// thanks!
		// http://boost.2283326.n4.nabble.com/wave-limited-extensibility-td2655231.html
		struct glsl_directives_hooks
			: public boost::wave::context_policies::default_preprocessing_hooks {
			// undocumented!
			template <typename ContextT, typename ContainerT>
			bool found_unknown_directive(ContextT const& ctx, ContainerT const& line,
				ContainerT& pending) {
				std::copy(line.begin(), line.end(), std::back_inserter(pending));
				return true;
			}
		};
	}

ShaderSource::ShaderSource(const char* path_)
    : source(loadSource(path_)), path(std::string(path_)) {}

std::string ShaderSource::preprocess(pipeline_stages stage,
                                     gsl::span<const char*> defines,
                                     gsl::span<const char*> include_paths) {
  using lex_iterator_type =
      boost::wave::cpplexer::lex_iterator<boost::wave::cpplexer::lex_token<>>;
  using context_type = boost::wave::context<
      std::string::iterator, lex_iterator_type,
      boost::wave::iteration_context_policies::load_file_to_string,
      glsl_directives_hooks>;

  context_type ctx(source.begin(), source.end(), path.c_str());

  ctx.add_include_path(".");

  for (auto p : include_paths)
    ctx.add_include_path(p);
  for (auto d : defines)
    ctx.add_macro_definition(d);

  switch (stage) {
  case pipeline_stages::vertex:
    ctx.add_macro_definition("_VERTEX_");
    break;
  case pipeline_stages::geometry:
    ctx.add_macro_definition("_GEOMETRY_");
    break;
  case pipeline_stages::pixel:
    ctx.add_macro_definition("_PIXEL_");
    break;
  case pipeline_stages::hull:
    ctx.add_macro_definition("_HULL_");
    break;
  case pipeline_stages::domain:
    ctx.add_macro_definition("_DOMAIN_");
    break;
  case pipeline_stages::compute:
    ctx.add_macro_definition("_COMPUTE_");
    break;
  default:
	  // 
	  return std::string{};
  }

  context_type::iterator_type first = ctx.begin();
  context_type::iterator_type last = ctx.end();

  std::ostringstream os;

  while (first != last) {
    os << (*first).get_value();
    ++first;
  }

  return std::move(os.str());
}
}
