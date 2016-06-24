#pragma once
#include "node_kind.hpp"
#include "utils.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct value_impl;
struct node;
struct graph_context;

using node_traversal_func = std::function<void(value_impl &)>;

// Represents an operation
// Nodes have one or more output values, and zero or more inputs
// Nodes allocate and manage the resources for their outputs (if necessary)
// (this is done in member function allocate_resources)
// Values are computed in member function execute()
struct node : public std::enable_shared_from_this<node> {
  node(node_kind kind) : kind_(kind) { uid_ = global_node_uid++; }

  virtual ~node() {}

  // traverse successor values
  virtual void traverse(node_traversal_func fn) {
    throw std::logic_error("unimplemented");
  }

  virtual void validate_inputs(graph_context &) {
  }

  virtual void finalize(graph_context&) {
  }

  virtual void execute(graph_context &) {
  }

  node_kind kind() const { return kind_; }

  bool dirty() const { return dirty_; }
  void set_dirty() { dirty_ = true; }

  //void bind_input_image(image_impl& out_img, std::shared_ptr<image_impl> in_img);
  //void bind_input_buffer(buffer_impl& out_buf, std::shared_ptr<buffer_impl> in_buf, storage_hint storage = storage_hint::automatic);
  //void bind_output_image(image_impl& out_img, const image_desc& desc);
  //void create_output_image(image_impl& out_img, std::shared_ptr<image_impl>);

  bool dirty_ = true;
  node_kind kind_;
  size_t uid_;
  static size_t global_node_uid;
};

//
// traversal visitors:
// for_all_successors(node, action)
// for_all_successors_recursive(node, action)
// for_all_predecessors(node, action)
// for_all_predecessors_recursive(node, action)
// where action(node) -> bool (stop traversal)
//
