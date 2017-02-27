#pragma once
#include <autograph/Types.h>
#include <autograph/gl/Query.h>
#include <chrono>
#include <string>

namespace ag {
namespace Profiler {

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>;
using Duration = std::chrono::nanoseconds;

struct Scope {
	// duration of the scope in CPU time
  auto duration() const {
	  return end - start;
  }

  // duration of the scope in GPU time 
  // (the time spent executing all GPU commands issued in the scope)
  auto durationGpu() const {
    auto gpuStart = gpuTimestampStartQuery.getGpuTimestampNs();
	auto gpuEnd = gpuTimestampEndQuery.getGpuTimestampNs();
    return gpuEnd - gpuStart;
  }

  // true if gpuClientTimestampRef, gpuStart, gpuEnd contain valid data
  bool gpuProfile = false;
  int64_t gpuClientTimestampRef;
  gl::TimestampQuery gpuTimestampStartQuery;
  gl::TimestampQuery gpuTimestampEndQuery;
  std::string name;
  TimePoint start;
  TimePoint end;
  Duration gpuStart;	// duration from timestamp ref
  Duration gpuEnd;
  int id = -1;
  int parent = -1;
  int next = -1;
  int firstChild = -1;
};

class ProfileData {
public:
  uint64_t frameId;
  TimePoint frameStartTime;
  TimePoint frameEndTime;

  const Scope* root() const { return &scopes_[0]; }
  const Scope* next(const Scope* s) const { return s->next != -1 ? &scopes_[s->next] : nullptr; }
  const Scope* parent(const Scope* s) const { return s->parent != -1 ? &scopes_[s->parent] : nullptr; }
  const Scope* firstChild(const Scope* s) const { return s->firstChild != -1 ? &scopes_[s->firstChild] : nullptr; }

  friend void endFrame();
  ProfileData clone();

private:
  std::vector<Scope> scopes_;
};

void beginFrame();
// does nothing if not profiling
void endFrame();
void enterScope(const char *scopeName, bool gpu = false);
void event(const char *id);
void exitScope();
void showGui();
// nullptr if no frame has been profiled
const ProfileData *getData();

struct ProfileGuard {
  ProfileGuard(const char *name, bool gpu) { enterScope(name, gpu); }
  ~ProfileGuard() { exitScope(); }
};
}

#define AG_PROFILE_FUNCTION ag::Profiler::ProfileGuard __pfg{__func__};
#define AG_PROFILE_SCOPE(name) ag::Profiler::ProfileGuard __pfg{name, false};
#define AG_GPU_PROFILE_SCOPE(name) ag::Profiler::ProfileGuard __pfg{name, true};
}
