#pragma once
#include "GLHandle.h"

namespace ag {
namespace gl {

struct QueryDeleter {
  static constexpr GLenum objectType = GL_QUERY;
  void operator()(GLuint obj) { glDeleteQueries(1, &obj); }
};

class TimestampQuery
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
  // Get timestamp in nanoseconds, wait for the result
  int64_t getGpuTimestampNs() const {
    int64_t res;
    glGetQueryObjecti64v(obj_.get(), GL_QUERY_RESULT, &res);
    return res;
  }
  // Get timestamp in seconds
  double getGpuTimestamp() const
  {
    int64_t res;
    glGetQueryObjecti64v(obj_.get(), GL_QUERY_RESULT, &res);
    return (double)res / 1000000000.0;
  }

private:
  GLHandle<QueryDeleter> obj_;
};
}
}
