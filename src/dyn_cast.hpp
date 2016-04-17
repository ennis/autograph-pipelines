#pragma once
#include <type_traits>

// must work w/ shared_ptr, unique_ptr and raw pointers
template <typename Derived, typename Base,
          typename = std::is_base_of<Base, Derived>>
Derived *dyn_cast(Base *b) {
  if (b && Derived::classof(*b)) {
    return static_cast<Derived *>(b);
  }
  return nullptr;
}