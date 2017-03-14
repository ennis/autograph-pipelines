#pragma once
#include <autograph/Config.h>
#include <cstdint>
#include <deque>
#include <autograph/gl/gl_core_4_5.h>
#include <stdexcept>

namespace ag {
namespace gl {
constexpr unsigned kFenceWaitTimeout = 2000000000; // in nanoseconds

//////////////////////////////////////////////
/**
 * @brief [brief description]
 * @details [long description]
 */
class AG_GL_API Fence {
public:
  /**
   * @brief [brief description]
   * @details [long description]
   */
  Fence() = default;

  /**
   * @brief [brief description]
   * @details [long description]
   * 
   * @param init_value [description]
   */
  Fence(uint64_t init_value) : current_value(init_value) {}

  /**
   * @brief [brief description]
   * @details [long description]
   * 
   * @param value [description]
   */
  void signal(uint64_t value);

  /**
   * @brief [brief description]
   * @details [long description]
   * 
   * @param timeout [description]
   * @return [description]
   */
  GLenum advance(uint64_t timeout);

  /**
   * @brief [brief description]
   * @details [long description]
   * 
   * @param value [description]
   */
  void wait(uint64_t value);

  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  uint64_t getValue();

  //////////////////////////////////////////////
  Fence(const Fence &) = delete;
  Fence &operator=(const Fence &) = delete;

  Fence(Fence &&rhs)
      : current_value(rhs.current_value),
        sync_points(std::move(rhs.sync_points)) {
    rhs.sync_points.clear();
  }
  Fence &operator=(Fence &&rhs) {
    std::swap(sync_points, rhs.sync_points);
    std::swap(current_value, rhs.current_value);
    return *this;
  }

  ~Fence() {
    for (auto s : sync_points)
      glDeleteSync(s.sync);
  }


private:
  struct AG_REFLECT sync_point {
    GLsync sync;
    uint64_t target_value;
  };

  uint64_t current_value;
  std::deque<sync_point> sync_points;
};
}
}
