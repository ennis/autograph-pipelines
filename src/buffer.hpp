#pragma once
#include "value.hpp"
#include "node.hpp"
#include "scheduler.hpp"
#include "gl_buffer.hpp"

struct buffer_impl : public value_impl {
	buffer_impl() : value_impl{ value_kind::buffer }, stype{ storage_type::host } {}

  static bool classof(const value_impl &v) {
    return v.kind() == value_kind::buffer;
  }

  std::size_t size; // -1 if pipeline-dynamic

  /////////////////////////////////
  storage_type stype;
  union {
	  gl_buffer_slice device_buf;
	  uint8_t* host_buf;
  } storage;
};


struct buffer {
  std::shared_ptr<buffer_impl> impl_;
};

