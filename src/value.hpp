#pragma once
#include <memory>
#include <string>
#include <autograph/utils.hpp>
#include <vector>
#include "traversal_visitor.hpp"

enum class storage_hint { device, host, implicit, automatic };
enum class storage_type { device, host };

enum class value_flags {
  eager_eval = (1 << 0),
};

ENUM_BIT_FLAGS_OPERATORS(value_flags)

struct node;
struct value_impl_data;

struct value_impl {

  enum value_kind { image, buffer };

  value_impl(value_kind kind, node *pred = nullptr, size_t port_id = 0)
      : kind_{kind}, pred_{pred}, data_{nullptr}, port_id_{port_id} {
    uid_ = global_value_uid++;
  }

  virtual ~value_impl() {}

  value_kind kind() const { return kind_; }

  value_impl_data* data() const { return data_; }
  void set_data(value_impl_data* data) { data_ = data; }

  // traverse successors
  void traverse(traversal_visitor& v);
  void add_successor(std::weak_ptr<node> wp);

  std::string name_;
  value_kind kind_;
  value_flags flags_;
  node *pred_;
  value_impl_data* data_;
  size_t port_id_;
  size_t uid_;
  std::vector<std::weak_ptr<node>> successors;

  static size_t global_value_uid;
};
