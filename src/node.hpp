#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <functional>
#include "node_kind.hpp"
#include "autograph/utils.hpp"
#include "traversal_visitor.hpp"

struct value_impl;
struct node;
struct allocation_context;

struct node {
  node(node_kind kind) : kind_(kind)
  {
	  uid_ = global_node_uid++;
  }

  virtual ~node() {}

  // get a std::vector containing the dependencies of the node
  //virtual std::vector<dependency> dependencies() const = 0;

  virtual void traverse(traversal_visitor& v) = 0;
  virtual void allocate_resources(allocation_context&) = 0; 

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

