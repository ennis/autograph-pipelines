#pragma once
#include <memory>
#include <string>

enum class storage_hint { device, host, implicit, automatic };

enum class value_flags {
  eager_eval = (1 << 0),
};

ENUM_BIT_FLAGS_OPERATORS(value_flags)

struct node;

struct value_impl {

  enum value_kind { image, buffer };

  value_impl(value_kind kind, node *pred = nullptr, size_t port_id = 0)
      : kind_{kind}, pred_{pred}, port_id_{port_id} {
    uid_ = global_value_uid++;
  }

  virtual ~value_impl() {}

  value_kind kind() const { return kind_; }

  std::string name_;
  value_kind kind_;
  value_flags flags_;
  node *pred_;
  size_t port_id_;
  size_t uid_;

  static size_t global_value_uid;
};
