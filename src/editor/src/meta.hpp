#pragma once
#include <gsl/span.h>
#include <msgpack.hpp>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace meta {
using packer = msgpack::packer<std::ostream>;
using unpacker = msgpack::unpacker;

struct attribute {
  std::type_index typeindex;
  const void *data;
};

struct field {
  const char *name;
  const char *qual_name;
  const char *friendly_name;
  std::type_index typeindex;
  size_t offset;
  void *(*get_ptr)(void *object);
  gsl::span<const attribute> attributes;

  template <typename T> T *get_as(void *object) const {
    return reinterpret_cast<T *>(get_ptr(object));
  }
};

struct enumerator {
  const char *name;
  const char *friendly_name;
  uint64_t value;
  gsl::span<const attribute> attributes;
};

enum class type_kind { enum_, struct_, union_ };

struct enum_t;
struct record_t;

struct type {
  const char *name;
  type_kind kind;
  gsl::span<const attribute> attributes;
  void (*serialize)(packer &, const void *);
  void (*deserialize)(unpacker &, void *);

  template <typename T> const T *as() const {
    if (kind == T::t_kind)
      return static_cast<const T *>(this);
    return nullptr;
  }
};

struct record_t : public type {
  static constexpr type_kind t_kind = type_kind::struct_;
  record_t(const char *name, gsl::span<const field> public_fields_,
           gsl::span<const std::type_index> bases_,
           gsl::span<const attribute> attributes_,
           void (*serialize_)(packer &, const void *),
           void (*deserialize_)(unpacker &, void *))
      : type{name, t_kind, attributes_, serialize_, deserialize_},
        public_fields{public_fields_}, bases{bases_} {}

  gsl::span<const field> public_fields;
  gsl::span<const std::type_index> bases;
};

struct enum_t : public type {
  static constexpr type_kind t_kind = type_kind::enum_;
  enum_t(const char *name, gsl::span<const enumerator> enumerators_,
         uint64_t (*get_value_)(const void *e),
         void (*set_value_)(void *, uint64_t),
         gsl::span<const attribute> attributes_,
         void (*serialize_)(packer &, const void *),
         void (*deserialize_)(unpacker &, void *))
      : type{name, t_kind, attributes_, serialize_, deserialize_},
        enumerators{enumerators_}, get_value{get_value_},
        set_value{set_value_} {}

  gsl::span<const enumerator> enumerators;

  int find_enumerator_index(const void *obj) const {
    auto v = get_value(obj);
    for (int i = 0; i < enumerators.size(); ++i) {
      if (enumerators[i].value == v)
        return i;
    }
    return -1;
  }

  const enumerator *find_enumerator(const void *obj) const {
    int i = find_enumerator_index(obj);
    if (i < 0)
      return nullptr;
    return &enumerators[i];
  }

  // manipulation functions
  uint64_t (*get_value)(const void *obj);
  void (*set_value)(void *obj, uint64_t value);
};

template <typename T> const type *type_of();
const type *type_of(std::type_index ti);

template <typename T> void serialize(packer& p, const T& data)
{
	serialize(p, type_of<T>(), &data);
}

template <typename T> void serialize_dynamic(packer& p, const T& data)
{
	// always use the dynamic type
	serialize(p, type_of(typeid(data)), &data);
}

inline void serialize(packer& p, const type* ty, const void* data)
{
	if (!ty) return;
	ty->serialize(p, data);
}

// enum value-to-string and string-to-value
template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
const char *get_enumerator_name(T constant);
template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
T get_enumerator_value(const char *name);
}
