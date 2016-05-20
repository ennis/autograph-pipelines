#pragma once
#include "autograph/utils.hpp"
#include "node_kind.hpp"
#include "traversal_visitor.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <vector>

struct value_impl;
struct node;
struct allocation_context;
struct execution_context;

// Represents an operation
// Nodes have one or more output values, and zero or more inputs
// Nodes allocate and manage the resources for their outputs (if necessary)
// (this is done in member function allocate_resources)
// Values are computed in member function execute()
struct node {
  node(node_kind kind) : kind_(kind) { uid_ = global_node_uid++; }

  virtual ~node() {}

  virtual void traverse(traversal_visitor &v) {
    throw std::logic_error("unimplemented");
  }

  virtual void allocate_resources(allocation_context &) {
    throw std::logic_error("unimplemented");
  }

  virtual void execute(execution_context &) {
    throw std::logic_error("unimplemented");
  }

  node_kind kind() const { return kind_; }

  bool dirty() const { return dirty_; }
  void set_dirty() { dirty_ = true; }

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
