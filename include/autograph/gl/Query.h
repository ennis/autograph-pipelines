#pragma once
#include "GLHandle.h"
#include <chrono>

namespace ag {
namespace gl {

struct AG_GL_API QueryDeleter {
  static constexpr GLenum objectType = GL_QUERY;
  void operator()(GLuint obj) { glDeleteQueries(1, &obj); }
};

class AG_GL_API TimestampQuery
{
public:
  TimestampQuery() {
    GLuint queryObj;
    glGenQueries(1, &queryObj);
    obj_ = queryObj;
  }

  GLuint object() const { return obj_.get(); }

  void asyncTimestamp() { glQueryCounter(obj_.get(), GL_TIMESTAMP); }
  //
  bool isResultAvailable() const {
    int64_t res;
    glGetQueryObjecti64v(obj_.get(), GL_QUERY_RESULT_AVAILABLE, &res);
    return res != 0;
  }
  // Get timestamp in nanoseconds
  std::chrono::nanoseconds getGpuTimestampNs() const
  {
    int64_t res;
    glGetQueryObjecti64v(obj_.get(), GL_QUERY_RESULT, &res);
	return std::chrono::nanoseconds{ res };
  }

private:
  GLHandle<QueryDeleter> obj_;
};
}
}
