#pragma once
#include <gsl/span.h>
#include <msgpack.hpp>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <cstddef> // somehow needed for size_t?

namespace meta {
using packer = msgpack::packer<std::ostream>;
using unpacker = msgpack::unpacker;

struct Attribute {
  std::type_index typeindex;
  const void *data;
};

struct Field {
  const char *name;
  const char *qualName;
  std::type_index typeindex;
  std::size_t offset;
  void *(*getPtr)(void *object);
  gsl::span<const Attribute> attributes;

  template <typename T> T *getAs(void *object) const {
    return reinterpret_cast<T *>(getPtr(object));
  }

  template <typename T> const T *getAttribute() const {
	  for (auto&& a : attributes) {
		  if (a.typeindex == std::type_index{ typeid(T) }) {
			  return static_cast<const T*>(a.data);
		  }
	  }
	  return nullptr;
  }
};

struct Enumerator {
  const char *name;
  uint64_t value;
  gsl::span<const Attribute> attributes;
  
  template <typename T> const T *getAttribute() const {
	  for (auto&& a : attributes) {
		  if (a.typeindex == std::type_index{ typeid(T) }) {
			  return static_cast<const T*>(a.data);
		  }
	  }
	  return nullptr;
  }
};

enum class TypeKind { Enum, Struct, Union };

struct Enum;
struct Record;

struct Type {
  const char *name;
  TypeKind kind;
  gsl::span<const Attribute> attributes;
  void (*serialize)(packer &, const void *);
  void (*deserialize)(unpacker &, void *);

  template <typename T> const T *as() const {
    if (kind == T::t_kind)
      return static_cast<const T *>(this);
    return nullptr;
  }
};

struct Record : public Type {
  static constexpr TypeKind t_kind = TypeKind::Struct;
  Record(const char *name, gsl::span<const Field> public_fields_,
           gsl::span<const std::type_index> bases_,
           gsl::span<const Attribute> attributes_,
           void (*serialize_)(packer &, const void *),
           void (*deserialize_)(unpacker &, void *))
      : Type{name, t_kind, attributes_, serialize_, deserialize_},
        publicFields{public_fields_}, bases{bases_} {}

  gsl::span<const Field> publicFields;
  gsl::span<const std::type_index> bases;
};

struct Enum : public Type {
  static constexpr TypeKind t_kind = TypeKind::Enum;
  Enum(const char *name, gsl::span<const Enumerator> enumerators_,
         uint64_t (*getValue_)(const void *e),
         void (*setValue_)(void *, uint64_t),
         gsl::span<const Attribute> attributes_,
         void (*serialize_)(packer &, const void *),
         void (*deserialize_)(unpacker &, void *))
      : Type{name, t_kind, attributes_, serialize_, deserialize_},
        enumerators{enumerators_}, getValue{getValue_},
        setValue{setValue_} {}

  gsl::span<const Enumerator> enumerators;

  int findEnumeratorIndex(const void *obj) const {
    auto v = getValue(obj);
    for (int i = 0; i < enumerators.size(); ++i) {
      if (enumerators[i].value == v)
        return i;
    }
    return -1;
  }

  const Enumerator *findEnumerator(const void *obj) const {
    int i = findEnumeratorIndex(obj);
    if (i < 0)
      return nullptr;
    return &enumerators[i];
  }

  // manipulation functions
  uint64_t (*getValue)(const void *obj);
  void (*setValue)(void *obj, uint64_t value);
};

template <typename T> const Type *typeOf();
const Type *typeOf(std::type_index ti);

template <typename T> void serialize(packer& p, const T& data)
{
	serialize(p, typeOf<T>(), &data);
}

template <typename T> void serialize_dynamic(packer& p, const T& data)
{
	// always use the dynamic type
	serialize(p, typeOf(typeid(data)), &data);
}

inline void serialize(packer& p, const Type* ty, const void* data)
{
	if (!ty) return;
	ty->serialize(p, data);
}

// enum value-to-string and string-to-value
template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
const char *getEnumeratorName(T constant);
template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
T getEnumeratorValue(const char *name);
}
