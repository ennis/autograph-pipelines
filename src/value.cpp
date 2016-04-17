#include "value.hpp"
#include <algorithm>

size_t value_impl::global_value_uid = 0;

void value_impl::traverse(traversal_visitor &v) {
  for (auto wp : successors) {
    if (auto sp = wp.lock())
      v.visit_node(*sp.get());
  }

  // collect expired refs
  successors.erase(std::remove_if(successors.begin(), successors.end(),
                                  [](const auto &wp) { return wp.expired(); }),
                   successors.end());
}

void value_impl::add_successor(std::weak_ptr<node> wp)
{
	successors.emplace_back(std::move(wp));
}
