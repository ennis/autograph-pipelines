/////////////////////////////////////////////////////////////////////
// Automatically generated file: DO NOT EDIT
#include "C:\Users\Alexandre\Developpement\autograph-pipelines\include\autograph\All.h"
#include <exception>
#include <typeindex>
#include <cstdint>
#include <cstddef>
#include <string>
#include <autograph/engine/Meta.h>

namespace ag  
{
namespace meta 
{
// base templates
template <typename T> struct RecordReflect;

template <typename T> struct EnumReflect;

// https://rmf.io/cxx11/overload-ranking/
struct overload_otherwise {
  overload_otherwise(...) {}
};

template <unsigned I> struct overload_choice : overload_choice<I + 1> {};
// terminate recursive inheritence at a convenient point,
// large enough to cover all cases
template <> struct overload_choice<10> {};

// "for clarity"
struct select_overload : overload_choice<0> {};

// candidates for serialization

template <typename T>
constexpr bool is_msgpack_packable_v =
    std::is_fundamental<T>::value || std::is_same<T, std::string>::value || 
    (std::is_array<T>::value && std::is_fundamental<std::remove_all_extents_t<T>>::value);

//============================================
// #1: msgpack primitive type
//		=> overload selected by Expression SFINAE (p.pack)
template <typename T, typename = std::enable_if_t<is_msgpack_packable_v<T>>>
auto serialize_impl0(overload_choice<0>, packer &p, const T &v)
    -> void {
  p.pack(v);
}

//============================================
// #2: static member serialize
//		=> overload selected by Expression SFINAE (T::serialize)
template <typename T>
auto serialize_impl0(overload_choice<1>, packer &p, const T &v)
    -> decltype(v.serialize(p), void()) {
  T::serialize(p);
}

//============================================
// #3: automatic serialization (structs)
//		=> overload selected by Expression SFINAE
//(RecordReflect<T>::serialize_impl)
template <typename T>
auto serialize_impl0(overload_choice<2>, packer &p, const T &v)
    -> decltype(RecordReflect<T>::serialize(p, &v), void()) {
  RecordReflect<T>::serialize(p, &v);
}

//============================================
// #3: automatic serialization (enums)
//		=> overload selected by Expression SFINAE
//(EnumReflect<T>::serialize_impl)
template <typename T>
auto serialize_impl0(overload_choice<3>, packer &p, const T &v)
    -> decltype(EnumReflect<T>::serialize(p, &v), void()) {
  EnumReflect<T>::serialize(p, &v);
}

//============================================
// #4: default
//		=> output binary data?
template <typename T>
auto serialize_impl0(overload_otherwise, packer &p, const T &v) -> void {}

// dispatcher
template <typename T>
void serialize_impl(packer& p, const T& v)
{
	serialize_impl0(select_overload{}, p, v);
}

/////////////////////////////////////////////////////////////////////
// RECORD REFLECTION DATA



/////////////////////////////////////////////////////////////////////
// ENUM REFLECTION DATA

///////////////////////////
// helpers
template <typename T>
uint64_t getEnumValue(const void* data) { return *reinterpret_cast<const T*>(data); }
template <typename T>
void setEnumValue(void* data, uint64_t value) { *reinterpret_cast<T*>(data) = value; }

template <typename T>
struct EnumReflect;



/////////////////////////////////////////////////////////////////////
// SERIALIZATION



/////////////////////////////////////////////////////////////////////
// TYPEINDEX MAP
static std::unordered_map<std::type_index, const Type*> typeindex_to_metaobject;
static bool typeindex_to_metaobject_initialized = false;

const Type *typeOf(std::type_index ti)
{
	if (!typeindex_to_metaobject_initialized) {
		using kv_pair = decltype(typeindex_to_metaobject)::value_type;
		typeindex_to_metaobject.insert({
		});
		typeindex_to_metaobject_initialized = true;
	}
	auto it = typeindex_to_metaobject.find(ti);
	if (it != typeindex_to_metaobject.end()) {
		return it->second;
	}
	return nullptr;
}



}
}