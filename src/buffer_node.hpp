#pragma once
#include "buffer.hpp"
#include "gl_buffer.hpp"
#include <cstring>

/////////////////////////////////////////////////
class buffer_node : public node {
public:
  buffer_node(std::size_t size) : node{node_kind::buffer} {
    dest_ = buffer_impl::create(size);
  }

  ~buffer_node() {}

  static bool classof(const node &n) { return n.kind() == node_kind::buffer; }

  void traverse(node_traversal_func fn) override {}

protected:
  buffer_impl::ptr dest_;
};

template <typename T> struct value_node : public buffer_node {
public:
  value_node(T initial_value = T{})
      : buffer_node{sizeof(T)}, value_{std::move(initial_value)} {}

  void set(T value) {
    value_ = std::move(value);
    set_dirty();
  }

  void execute(graph_context &ctx) override {
    memcpy(dest_->get_host_buf(), &value_, sizeof(T));
  }

private:
  T value_;
};
