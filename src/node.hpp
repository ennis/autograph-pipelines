#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "node_kind.hpp"

struct value_impl;

struct node {
  node(node_kind kind) : kind_(kind) 
  {
	  uid_ = global_node_uid++;
  }

  virtual ~node() {}

  // get a std::vector containing the dependencies of the node
  //virtual std::vector<dependency> dependencies() const = 0;

  node_kind kind() const { return kind_; }

  node_kind kind_;
  size_t uid_;
  static size_t global_node_uid;
};
