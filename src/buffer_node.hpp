#pragma once
#include "buffer.hpp"
#include "gl_buffer.hpp"

/////////////////////////////////////////////////
struct buffer_node : public node {
  buffer_node()
      : node{node_kind::buffer}, stype{storage_type::host},
        storage{std::unique_ptr<uint8_t[]>{}} {}
  ~buffer_node() {
    if (stype == storage_type::host)
      storage.host_buf.~unique_ptr<uint8_t[]>();
    else
      storage.device_buf.~gl_buffer();
  }

  static bool classof(const node &n) { return n.kind() == node_kind::buffer; }


  virtual void traverse(traversal_visitor& v) override {
	  // TODO
	  v.visit_value(dest);
  }

  static auto create(const void *ptr, size_t size, storage_type stype) {
    auto n = std::make_shared<buffer_node>();
    n->dest.size = size;
    n->stype = stype;
    if (stype == storage_type::device) {
      auto bufobj =
          gl_buffer::create(size, gl_buffer_usage::default_usage, ptr);
      new (&n->storage.device_buf) gl_buffer{std::move(bufobj)};
    } else {
      auto data = std::make_unique<uint8_t[]>(size);
      std::memcpy(data.get(), ptr, size);
      n->storage.host_buf = std::move(data);
    }

    return std::shared_ptr<buffer_impl>{n, &n->dest};
  }

  ////////////////////////////////////////////////
  buffer_impl dest;
  storage_type stype;
  union U {
    std::unique_ptr<uint8_t[]> host_buf;
    gl_buffer device_buf;
    ~U() {}
  } storage;
};