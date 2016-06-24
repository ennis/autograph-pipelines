#pragma once
#include "gl_buffer.hpp"
#include "node.hpp"
#include "scheduler.hpp"
#include "value.hpp"

struct buffer_impl : public value_impl {
  using ptr = std::shared_ptr<buffer_impl>;

  // create an empty (unsized) buffer
  buffer_impl() : value_impl{ value_kind::buffer }, size_{ 0 }
  {}

  // create a specified (sized) buffer
  buffer_impl(std::size_t size) : value_impl{value_kind::buffer, value_flags::specified}, size_{size} {}

  // aliasing constuctor
  buffer_impl(ptr share) : value_impl{ value_kind::buffer, value_flags::bound }
  {}

  static bool classof(const value_impl &v) {
    return v.kind() == value_kind::buffer;
  }

  static buffer_impl::ptr create(std::size_t size) {
    return std::make_shared<buffer_impl>(size);
  }

  // TODO: return a span?
  uint8_t *get_host_buf() { 
	  assert(is_allocated());
	  return host_buf_.get();
  }

  gl_buffer &get_buffer() { 
	  assert(is_allocated());
	  return gl_buf_; 
  }

  std::size_t size_;
  gl_buffer gl_buf_;
  std::unique_ptr<uint8_t[]> host_buf_;
};

struct buffer {
  std::shared_ptr<buffer_impl> impl_;
};
