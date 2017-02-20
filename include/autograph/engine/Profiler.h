#pragma once
#include <autograph/Types.h>
#include <chrono>
#include <string>

namespace ag {
namespace Profiler {

struct Scope {
  auto duration() const {
    std::chrono::duration<double> s = end - start;
    return s.count();
  }

  std::string name;
  std::chrono::high_resolution_clock::time_point start;
  std::chrono::high_resolution_clock::time_point end;
  const Scope *parent = nullptr;
  const Scope *next = nullptr;
  const Scope *firstChild = nullptr;
};

struct ProfilingData {
	uint64_t frameId;
	const Scope *rootScope = nullptr;
};

void beginFrame();
// does nothing if not profiling
void endFrame();
void enterScope(const char *scopeName);
void event(const char *id);
void exitScope();
void showGui();
// nullptr if no frame has been profiled
const ProfilingData *getData();

struct ProfileGuard {
  ProfileGuard(const char *name) { enterScope(name); }
  ~ProfileGuard() { exitScope(); }
};
}

#define AG_PROFILE_FUNCTION ag::Profiler::ProfileGuard __pfg{__func__};
#define AG_PROFILE_SCOPE(name) ag::Profiler::ProfileGuard __pfg{name};
}