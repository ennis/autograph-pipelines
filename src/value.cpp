#include "value.hpp"
#include <algorithm>

size_t value_impl::global_value_uid = 0;

void value_impl::traverse(value_traversal_func fn) {
  for (auto wp : successors_) {
    if (auto sp = wp.lock())
      fn(*sp);
  }

  // collect expired refs
  successors_.erase(std::remove_if(successors_.begin(), successors_.end(),
                                  [](const auto &wp) { return wp.expired(); }),
	  successors_.end());
}

void value_impl::add_successor(std::weak_ptr<value_impl> wp)
{
	successors_.emplace_back(std::move(wp));
}
