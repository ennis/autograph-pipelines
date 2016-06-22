#pragma once
#include <autograph/utils.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

enum class storage_hint { device, host, implicit, automatic };
enum class storage_type { device, host };

enum class value_flags {
  empty = 0,
  specified = (1 << 0),
  bound = (1 << 1),
  allocated = (1 << 2), // implies specified
  input = (1 << 3),	// this value is an input endpoint
  output = (1 << 4)	// this value is an output endpoint
};

ENUM_BIT_FLAGS_OPERATORS(value_flags)

struct value_impl;
using value_traversal_func = std::function<void(value_impl &)>;

struct node;

struct value_impl {
  using ptr = std::shared_ptr<value_impl>;

  enum value_kind { image, buffer };

  // create a new empty value
  value_impl(value_kind kind)
      : kind_{kind}, share_{nullptr}, pred_{nullptr}, port_id_{0},
        flags_{value_flags::empty} {
    uid_ = global_value_uid++;
  }

  // create a bound value
  value_impl(value_kind kind, ptr share)
      : kind_{kind}, share_{std::move(share)}, pred_{nullptr}, port_id_{0},
        flags_{value_flags::bound | value_flags::specified} {}

  // create a value in the speicified state
  value_impl(value_kind kind, value_flags flags)
      : kind_{kind}, share_{nullptr}, pred_{nullptr}, port_id_{0},
        flags_{flags} {}

  bool is_empty() { return flags_ == value_flags::empty; }
  bool is_allocated() { return !!(flags_ & value_flags::allocated); }

  virtual ~value_impl() {}
  value_kind kind() const { return kind_; }

  // traverse successors
  void traverse(value_traversal_func fn);
  void add_successor(std::weak_ptr<value_impl> wp);

  std::string name_;
  value_kind kind_;
  value_flags flags_;
  node *pred_; // node that owns the endpoint
  ptr share_;  // for resource sharing
  size_t port_id_;
  size_t uid_;
  // std::vector of dependent values
  std::vector<std::weak_ptr<value_impl>> successors_;
  static size_t global_value_uid;
};
