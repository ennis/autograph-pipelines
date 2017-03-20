#pragma once
#include <autograph/engine/Config.h>
#include <autograph/Types.h>
#include <autograph/gl/Query.h>
#include <chrono>
#include <string>

namespace ag {
namespace Profiler {

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>;
using Duration = std::chrono::nanoseconds;

struct AG_ENGINE_API Scope
{
	Scope() = default;
	Scope(const Scope&) = delete;
	Scope& operator=(const Scope&) = delete;
	Scope(Scope&&) = default;
	Scope& operator=(Scope&&) = default;

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

class AG_ENGINE_API ProfileData {
public:
	ProfileData() = default;
	ProfileData(const ProfileData&) = delete;
	ProfileData& operator=(const ProfileData&) = delete;
	ProfileData(ProfileData&&) = default;
	ProfileData& operator=(ProfileData&&) = default;

  uint64_t frameId;
  TimePoint frameStartTime;
  TimePoint frameEndTime;
  std::vector<Scope> scopes;

  const Scope* root() const { return &scopes[0]; }
  const Scope* next(const Scope* s) const { return s->next != -1 ? &scopes[s->next] : nullptr; }
  const Scope* parent(const Scope* s) const { return s->parent != -1 ? &scopes[s->parent] : nullptr; }
  const Scope* firstChild(const Scope* s) const { return s->firstChild != -1 ? &scopes[s->firstChild] : nullptr; }
  ProfileData clone();
};

AG_ENGINE_API void beginFrame();
// does nothing if not profiling
AG_ENGINE_API void endFrame();
AG_ENGINE_API void enterScope(const char *scopeName, bool gpu = false);
AG_ENGINE_API void event(const char *id);
AG_ENGINE_API void exitScope();
AG_ENGINE_API void showGui();
// nullptr if no frame has been profiled
AG_ENGINE_API const ProfileData *getData();

struct ProfileGuard {
  ProfileGuard(const char *name, bool gpu) { enterScope(name, gpu); }
  ~ProfileGuard() { exitScope(); }
};
}

#define AG_PROFILE_FUNCTION ag::Profiler::ProfileGuard __pfg{__func__};
#define AG_PROFILE_SCOPE(name) ag::Profiler::ProfileGuard __pfg{name, false};
#define AG_GPU_PROFILE_SCOPE(name) ag::Profiler::ProfileGuard __pfg{name, true};
}
