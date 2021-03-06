#pragma once
#include <autograph/Engine/ImGuiUtils.h>
#include <autograph/Engine/Meta.h>

namespace ag {
class AG_ENGINE_API CVarBase {
public:
  CVarBase(const char *id_);
  CVarBase(const CVarBase &) = delete;
  CVarBase &operator=(const CVarBase &) = delete;
  CVarBase(CVarBase &&) = delete;
  CVarBase &operator=(CVarBase &&) = delete;

  virtual void onGui() = 0;

  std::string id;
};

template <typename T> class CVar : public CVarBase {
public:
  CVar(const char *id, T init = T{}) : CVarBase{id} {}
  void onGui() override { gui::genericValue<T>(value_); }
  T value_;
};

// TODO: 
// - CVarFloat
// - CVarToggle
// - CVarEnum
// - CVarColor
// - CVarString

void showCVarGui();
}