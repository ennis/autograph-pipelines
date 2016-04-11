#pragma once
#include "value.hpp"
#include "node.hpp"

struct buffer_impl : public value_impl {
  buffer_impl() : value_impl{value_kind::buffer} {}

  static bool classof(const value_impl &v) {
    return v.kind() == value_kind::buffer;
  }

  std::size_t size; // -1 if pipeline-dynamic
};

struct constant_node : public node {
  constant_node() : node{node_kind::constant} {}
  static bool classof(const node &n) { return n.kind() == node_kind::constant; }

  buffer_impl dest;
  std::unique_ptr<uint8_t[]> data;
  std::size_t size;
};

struct buffer {
  std::shared_ptr<buffer_impl> impl_;
};

template <typename T> static buffer make_constant(const T &data) {
  auto n = std::make_shared<constant_node>();
  n->data = std::make_unique<uint8_t[]>(sizeof(T));
  n->dest.pred_ = &n;
  n->dest.size = sizeof(T);
  return buffer{std::shared_ptr<buffer_impl>(n, &n->dest)};
}
