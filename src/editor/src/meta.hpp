#pragma once
#include <gsl/span.h>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace meta {
struct metaobject;

struct attribute
{
	std::type_index typeindex;
	const void* data;
};

struct field {
  const char *name;
  const char *qual_name;
  const char *friendly_name;
  std::type_index typeindex;
  size_t offset;
  gsl::span<const attribute> attributes;
};

struct enum_constant {
  const char *name;
  uint64_t value;
  gsl::span<const attribute> attributes;
};

enum class type_kind { enum_, struct_, union_ };

struct enum_metaobject;
struct record_metaobject;

struct metaobject {
  const char *name;
  type_kind kind;
  gsl::span<const attribute> attributes;
  template <typename T> const T *as() const {
    if (kind == T::t_kind)
      return static_cast<const T *>(this);
    return nullptr;
  }
};

struct record_metaobject : public metaobject {
  static constexpr type_kind t_kind = type_kind::struct_;
  record_metaobject(const char *name, gsl::span<const field> public_fields_,
                    gsl::span<const std::type_index> bases_)
      : metaobject{name, t_kind}, public_fields{public_fields_}, bases{bases_} {
  }

  gsl::span<const field> public_fields;
  gsl::span<const std::type_index> bases;
};

struct enum_metaobject : public metaobject {
  static constexpr type_kind t_kind = type_kind::enum_;
  enum_metaobject(const char *name, gsl::span<const enum_constant> constants_)
      : metaobject{name, t_kind}, constants{constants_} {}

  gsl::span<const enum_constant> constants;
};

template <typename T> const metaobject *get_metaobject();
const metaobject *get_metaobject(std::type_index ti);

// enum value-to-string and string-to-value
template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
const char *get_enum_constant_name(T constant);
template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
T get_enum_constant_value(const char *name);
}
