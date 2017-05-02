#pragma once
#include "GLHandle.h"
#include <chrono>

namespace ag {

struct AG_GFX_API QueryDeleter {
  static constexpr gl::GLenum objectType = gl::QUERY;
  void operator()(gl::GLuint obj) { gl::DeleteQueries(1, &obj); }
};

class AG_GFX_API TimestampQuery
{
public:
  TimestampQuery() {
    gl::GLuint queryObj;
    gl::GenQueries(1, &queryObj);
    obj_ = queryObj;
  }

  gl::GLuint object() const { return obj_.get(); }

  void asyncTimestamp() { gl::QueryCounter(obj_.get(), gl::TIMESTAMP); }
  //
  bool isResultAvailable() const {
    int64_t res;
    gl::GetQueryObjecti64v(obj_.get(), gl::QUERY_RESULT_AVAILABLE, &res);
    return res != 0;
  }
  // Get timestamp in nanoseconds
  std::chrono::nanoseconds getGpuTimestampNs() const
  {
    int64_t res;
    gl::GetQueryObjecti64v(obj_.get(), gl::QUERY_RESULT, &res);
	return std::chrono::nanoseconds{ res };
  }

private:
  GLHandle<QueryDeleter> obj_;
};
}
