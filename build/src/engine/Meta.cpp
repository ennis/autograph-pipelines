/////////////////////////////////////////////////////////////////////
// Automatically generated file: DO NOT EDIT
#include "C:\Users\Alexandre\Developpement\autograph-pipelines\include\autograph\All.h"
#include <autograph/engine/Meta.h>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace ag {
namespace meta {
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
    (std::is_array<T>::value &&
     std::is_fundamental<std::remove_all_extents_t<T>>::value);

//============================================
// #1: msgpack primitive type
//		=> overload selected by Expression SFINAE (p.pack)
template <typename T, typename = std::enable_if_t<is_msgpack_packable_v<T>>>
auto serialize_impl0(overload_choice<0>, packer &p, const T &v) -> void {
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
template <typename T> void serialize_impl(packer &p, const T &v) {
  serialize_impl0(select_overload{}, p, v);
}

/////////////////////////////////////////////////////////////////////
// RECORD REFLECTION DATA

///////////////////////////
// struct ag::Arcball
template <> struct RecordReflect<ag::Arcball> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::Arcball>::metaobj{
    "ag::Arcball",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                     // attribute table
    &RecordReflect<ag::Arcball>::serialize, // serialize
    nullptr                                 // deserialize
};

template <> const Type *typeOf<ag::Arcball>() {
  return &RecordReflect<ag::Arcball>::metaobj;
}

///////////////////////////
// struct ag::CVarBase
template <> struct RecordReflect<ag::CVarBase> {
  // attribute instances

  static const std::array<Field, 1> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_id(void *ptr) {
    return &reinterpret_cast<ag::CVarBase *>(ptr)->id;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 1> RecordReflect<ag::CVarBase>::field_data{{
    Field{"id",
          "ag::CVarBase::id",
          std::type_index{typeid(std::basic_string<char, std::char_traits<char>,
                                                   std::allocator<char>>)},
          offsetof(ag::CVarBase, id),
          &RecordReflect<ag::CVarBase>::field_thunk_id,
          {}},
}};

const Record RecordReflect<ag::CVarBase>::metaobj{
    "ag::CVarBase",
    RecordReflect<ag::CVarBase>::field_data,
    gsl::span<std::type_index>{},
    {},                                      // attribute table
    &RecordReflect<ag::CVarBase>::serialize, // serialize
    nullptr                                  // deserialize
};

template <> const Type *typeOf<ag::CVarBase>() {
  return &RecordReflect<ag::CVarBase>::metaobj;
}

///////////////////////////
// struct ag::ResourcePool
template <> struct RecordReflect<ag::ResourcePool> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::ResourcePool>::metaobj{
    "ag::ResourcePool",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                          // attribute table
    &RecordReflect<ag::ResourcePool>::serialize, // serialize
    nullptr                                      // deserialize
};

template <> const Type *typeOf<ag::ResourcePool>() {
  return &RecordReflect<ag::ResourcePool>::metaobj;
}

///////////////////////////
// struct ag::Action
template <> struct RecordReflect<ag::Action> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1> RecordReflect<ag::Action>::bases_data{{
    std::type_index{typeid(ag::Observable<void>)},
}};

const Record RecordReflect<ag::Action>::metaobj{
    "ag::Action",
    gsl::span<Field>{},
    RecordReflect<ag::Action>::bases_data,
    {},                                    // attribute table
    &RecordReflect<ag::Action>::serialize, // serialize
    nullptr                                // deserialize
};

template <> const Type *typeOf<ag::Action>() {
  return &RecordReflect<ag::Action>::metaobj;
}

///////////////////////////
// struct ag::KeyAction
template <> struct RecordReflect<ag::KeyAction> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1> RecordReflect<ag::KeyAction>::bases_data{{
    std::type_index{typeid(ag::Action)},
}};

const Record RecordReflect<ag::KeyAction>::metaobj{
    "ag::KeyAction",
    gsl::span<Field>{},
    RecordReflect<ag::KeyAction>::bases_data,
    {},                                       // attribute table
    &RecordReflect<ag::KeyAction>::serialize, // serialize
    nullptr                                   // deserialize
};

template <> const Type *typeOf<ag::KeyAction>() {
  return &RecordReflect<ag::KeyAction>::metaobj;
}

///////////////////////////
// struct ag::GamepadButtonAction
template <> struct RecordReflect<ag::GamepadButtonAction> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1>
    RecordReflect<ag::GamepadButtonAction>::bases_data{{
        std::type_index{typeid(ag::Action)},
    }};

const Record RecordReflect<ag::GamepadButtonAction>::metaobj{
    "ag::GamepadButtonAction",
    gsl::span<Field>{},
    RecordReflect<ag::GamepadButtonAction>::bases_data,
    {},                                                 // attribute table
    &RecordReflect<ag::GamepadButtonAction>::serialize, // serialize
    nullptr                                             // deserialize
};

template <> const Type *typeOf<ag::GamepadButtonAction>() {
  return &RecordReflect<ag::GamepadButtonAction>::metaobj;
}

///////////////////////////
// struct ag::gl::VertexAttribute
template <> struct RecordReflect<ag::gl::VertexAttribute> {
  // attribute instances

  static const std::array<Field, 5> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_slot(void *ptr) {
    return &reinterpret_cast<ag::gl::VertexAttribute *>(ptr)->slot;
  }
  static void *field_thunk_type(void *ptr) {
    return &reinterpret_cast<ag::gl::VertexAttribute *>(ptr)->type;
  }
  static void *field_thunk_size(void *ptr) {
    return &reinterpret_cast<ag::gl::VertexAttribute *>(ptr)->size;
  }
  static void *field_thunk_relativeOffset(void *ptr) {
    return &reinterpret_cast<ag::gl::VertexAttribute *>(ptr)->relativeOffset;
  }
  static void *field_thunk_normalized(void *ptr) {
    return &reinterpret_cast<ag::gl::VertexAttribute *>(ptr)->normalized;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 5> RecordReflect<ag::gl::VertexAttribute>::field_data{{
    Field{"slot",
          "ag::gl::VertexAttribute::slot",
          std::type_index{typeid(unsigned int)},
          offsetof(ag::gl::VertexAttribute, slot),
          &RecordReflect<ag::gl::VertexAttribute>::field_thunk_slot,
          {}},
    Field{"type",
          "ag::gl::VertexAttribute::type",
          std::type_index{typeid(unsigned int)},
          offsetof(ag::gl::VertexAttribute, type),
          &RecordReflect<ag::gl::VertexAttribute>::field_thunk_type,
          {}},
    Field{"size",
          "ag::gl::VertexAttribute::size",
          std::type_index{typeid(unsigned int)},
          offsetof(ag::gl::VertexAttribute, size),
          &RecordReflect<ag::gl::VertexAttribute>::field_thunk_size,
          {}},
    Field{"relativeOffset",
          "ag::gl::VertexAttribute::relativeOffset",
          std::type_index{typeid(unsigned int)},
          offsetof(ag::gl::VertexAttribute, relativeOffset),
          &RecordReflect<ag::gl::VertexAttribute>::field_thunk_relativeOffset,
          {}},
    Field{"normalized",
          "ag::gl::VertexAttribute::normalized",
          std::type_index{typeid(bool)},
          offsetof(ag::gl::VertexAttribute, normalized),
          &RecordReflect<ag::gl::VertexAttribute>::field_thunk_normalized,
          {}},
}};

const Record RecordReflect<ag::gl::VertexAttribute>::metaobj{
    "ag::gl::VertexAttribute",
    RecordReflect<ag::gl::VertexAttribute>::field_data,
    gsl::span<std::type_index>{},
    {},                                                 // attribute table
    &RecordReflect<ag::gl::VertexAttribute>::serialize, // serialize
    nullptr                                             // deserialize
};

template <> const Type *typeOf<ag::gl::VertexAttribute>() {
  return &RecordReflect<ag::gl::VertexAttribute>::metaobj;
}

///////////////////////////
// struct ag::Profiler::Scope
template <> struct RecordReflect<ag::Profiler::Scope> {
  // attribute instances

  static const std::array<Field, 13> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_gpuProfile(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->gpuProfile;
  }
  static void *field_thunk_gpuClientTimestampRef(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->gpuClientTimestampRef;
  }
  static void *field_thunk_gpuTimestampStartQuery(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)
                ->gpuTimestampStartQuery;
  }
  static void *field_thunk_gpuTimestampEndQuery(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->gpuTimestampEndQuery;
  }
  static void *field_thunk_name(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->name;
  }
  static void *field_thunk_start(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->start;
  }
  static void *field_thunk_end(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->end;
  }
  static void *field_thunk_gpuStart(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->gpuStart;
  }
  static void *field_thunk_gpuEnd(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->gpuEnd;
  }
  static void *field_thunk_id(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->id;
  }
  static void *field_thunk_parent(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->parent;
  }
  static void *field_thunk_next(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->next;
  }
  static void *field_thunk_firstChild(void *ptr) {
    return &reinterpret_cast<ag::Profiler::Scope *>(ptr)->firstChild;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 13> RecordReflect<ag::Profiler::Scope>::field_data{{
    Field{"gpuProfile",
          "ag::Profiler::Scope::gpuProfile",
          std::type_index{typeid(bool)},
          offsetof(ag::Profiler::Scope, gpuProfile),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_gpuProfile,
          {}},
    Field{
        "gpuClientTimestampRef",
        "ag::Profiler::Scope::gpuClientTimestampRef",
        std::type_index{typeid(long long)},
        offsetof(ag::Profiler::Scope, gpuClientTimestampRef),
        &RecordReflect<ag::Profiler::Scope>::field_thunk_gpuClientTimestampRef,
        {}},
    Field{
        "gpuTimestampStartQuery",
        "ag::Profiler::Scope::gpuTimestampStartQuery",
        std::type_index{typeid(ag::gl::TimestampQuery)},
        offsetof(ag::Profiler::Scope, gpuTimestampStartQuery),
        &RecordReflect<ag::Profiler::Scope>::field_thunk_gpuTimestampStartQuery,
        {}},
    Field{"gpuTimestampEndQuery",
          "ag::Profiler::Scope::gpuTimestampEndQuery",
          std::type_index{typeid(ag::gl::TimestampQuery)},
          offsetof(ag::Profiler::Scope, gpuTimestampEndQuery),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_gpuTimestampEndQuery,
          {}},
    Field{"name",
          "ag::Profiler::Scope::name",
          std::type_index{typeid(std::basic_string<char, std::char_traits<char>,
                                                   std::allocator<char>>)},
          offsetof(ag::Profiler::Scope, name),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_name,
          {}},
    Field{
        "start",
        "ag::Profiler::Scope::start",
        std::type_index{typeid(
            std::chrono::time_point<
                std::chrono::steady_clock,
                std::chrono::duration<long long, std::ratio<1, 1000000000>>>)},
        offsetof(ag::Profiler::Scope, start),
        &RecordReflect<ag::Profiler::Scope>::field_thunk_start,
        {}},
    Field{
        "end",
        "ag::Profiler::Scope::end",
        std::type_index{typeid(
            std::chrono::time_point<
                std::chrono::steady_clock,
                std::chrono::duration<long long, std::ratio<1, 1000000000>>>)},
        offsetof(ag::Profiler::Scope, end),
        &RecordReflect<ag::Profiler::Scope>::field_thunk_end,
        {}},
    Field{"gpuStart",
          "ag::Profiler::Scope::gpuStart",
          std::type_index{typeid(
              std::chrono::duration<long long, std::ratio<1, 1000000000>>)},
          offsetof(ag::Profiler::Scope, gpuStart),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_gpuStart,
          {}},
    Field{"gpuEnd",
          "ag::Profiler::Scope::gpuEnd",
          std::type_index{typeid(
              std::chrono::duration<long long, std::ratio<1, 1000000000>>)},
          offsetof(ag::Profiler::Scope, gpuEnd),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_gpuEnd,
          {}},
    Field{"id",
          "ag::Profiler::Scope::id",
          std::type_index{typeid(int)},
          offsetof(ag::Profiler::Scope, id),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_id,
          {}},
    Field{"parent",
          "ag::Profiler::Scope::parent",
          std::type_index{typeid(int)},
          offsetof(ag::Profiler::Scope, parent),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_parent,
          {}},
    Field{"next",
          "ag::Profiler::Scope::next",
          std::type_index{typeid(int)},
          offsetof(ag::Profiler::Scope, next),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_next,
          {}},
    Field{"firstChild",
          "ag::Profiler::Scope::firstChild",
          std::type_index{typeid(int)},
          offsetof(ag::Profiler::Scope, firstChild),
          &RecordReflect<ag::Profiler::Scope>::field_thunk_firstChild,
          {}},
}};

const Record RecordReflect<ag::Profiler::Scope>::metaobj{
    "ag::Profiler::Scope",
    RecordReflect<ag::Profiler::Scope>::field_data,
    gsl::span<std::type_index>{},
    {},                                             // attribute table
    &RecordReflect<ag::Profiler::Scope>::serialize, // serialize
    nullptr                                         // deserialize
};

template <> const Type *typeOf<ag::Profiler::Scope>() {
  return &RecordReflect<ag::Profiler::Scope>::metaobj;
}

///////////////////////////
// struct ag::Profiler::ProfileData
template <> struct RecordReflect<ag::Profiler::ProfileData> {
  // attribute instances

  static const std::array<Field, 4> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_frameId(void *ptr) {
    return &reinterpret_cast<ag::Profiler::ProfileData *>(ptr)->frameId;
  }
  static void *field_thunk_frameStartTime(void *ptr) {
    return &reinterpret_cast<ag::Profiler::ProfileData *>(ptr)->frameStartTime;
  }
  static void *field_thunk_frameEndTime(void *ptr) {
    return &reinterpret_cast<ag::Profiler::ProfileData *>(ptr)->frameEndTime;
  }
  static void *field_thunk_scopes(void *ptr) {
    return &reinterpret_cast<ag::Profiler::ProfileData *>(ptr)->scopes;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 4> RecordReflect<
    ag::Profiler::ProfileData>::field_data{{
    Field{"frameId",
          "ag::Profiler::ProfileData::frameId",
          std::type_index{typeid(unsigned long long)},
          offsetof(ag::Profiler::ProfileData, frameId),
          &RecordReflect<ag::Profiler::ProfileData>::field_thunk_frameId,
          {}},
    Field{
        "frameStartTime",
        "ag::Profiler::ProfileData::frameStartTime",
        std::type_index{typeid(
            std::chrono::time_point<
                std::chrono::steady_clock,
                std::chrono::duration<long long, std::ratio<1, 1000000000>>>)},
        offsetof(ag::Profiler::ProfileData, frameStartTime),
        &RecordReflect<ag::Profiler::ProfileData>::field_thunk_frameStartTime,
        {}},
    Field{
        "frameEndTime",
        "ag::Profiler::ProfileData::frameEndTime",
        std::type_index{typeid(
            std::chrono::time_point<
                std::chrono::steady_clock,
                std::chrono::duration<long long, std::ratio<1, 1000000000>>>)},
        offsetof(ag::Profiler::ProfileData, frameEndTime),
        &RecordReflect<ag::Profiler::ProfileData>::field_thunk_frameEndTime,
        {}},
    Field{"scopes",
          "ag::Profiler::ProfileData::scopes",
          std::type_index{
              typeid(std::vector<ag::Profiler::Scope,
                                 std::allocator<ag::Profiler::Scope>>)},
          offsetof(ag::Profiler::ProfileData, scopes),
          &RecordReflect<ag::Profiler::ProfileData>::field_thunk_scopes,
          {}},
}};

const Record RecordReflect<ag::Profiler::ProfileData>::metaobj{
    "ag::Profiler::ProfileData",
    RecordReflect<ag::Profiler::ProfileData>::field_data,
    gsl::span<std::type_index>{},
    {},                                                   // attribute table
    &RecordReflect<ag::Profiler::ProfileData>::serialize, // serialize
    nullptr                                               // deserialize
};

template <> const Type *typeOf<ag::Profiler::ProfileData>() {
  return &RecordReflect<ag::Profiler::ProfileData>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget
template <> struct RecordReflect<ag::RenderTarget> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::RenderTarget>::metaobj{
    "ag::RenderTarget",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                          // attribute table
    &RecordReflect<ag::RenderTarget>::serialize, // serialize
    nullptr                                      // deserialize
};

template <> const Type *typeOf<ag::RenderTarget>() {
  return &RecordReflect<ag::RenderTarget>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::DepthRenderbuffer
template <> struct RecordReflect<ag::RenderTarget::DepthRenderbuffer> {
  // attribute instances

  static const std::array<Field, 1> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_fmt(void *ptr) {
    return &reinterpret_cast<ag::RenderTarget::DepthRenderbuffer *>(ptr)->fmt;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 1> RecordReflect<
    ag::RenderTarget::DepthRenderbuffer>::field_data{{
    Field{"fmt",
          "ag::RenderTarget::DepthRenderbuffer::fmt",
          std::type_index{typeid(ag::ImageFormat)},
          offsetof(ag::RenderTarget::DepthRenderbuffer, fmt),
          &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::field_thunk_fmt,
          {}},
}};

const Record RecordReflect<ag::RenderTarget::DepthRenderbuffer>::metaobj{
    "ag::RenderTarget::DepthRenderbuffer",
    RecordReflect<ag::RenderTarget::DepthRenderbuffer>::field_data,
    gsl::span<std::type_index>{},
    {}, // attribute table
    &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::serialize, // serialize
    nullptr // deserialize
};

template <> const Type *typeOf<ag::RenderTarget::DepthRenderbuffer>() {
  return &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::DepthTexture
template <> struct RecordReflect<ag::RenderTarget::DepthTexture> {
  // attribute instances

  static const std::array<Field, 1> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_fmt(void *ptr) {
    return &reinterpret_cast<ag::RenderTarget::DepthTexture *>(ptr)->fmt;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 1>
    RecordReflect<ag::RenderTarget::DepthTexture>::field_data{{
        Field{"fmt",
              "ag::RenderTarget::DepthTexture::fmt",
              std::type_index{typeid(ag::ImageFormat)},
              offsetof(ag::RenderTarget::DepthTexture, fmt),
              &RecordReflect<ag::RenderTarget::DepthTexture>::field_thunk_fmt,
              {}},
    }};

const Record RecordReflect<ag::RenderTarget::DepthTexture>::metaobj{
    "ag::RenderTarget::DepthTexture",
    RecordReflect<ag::RenderTarget::DepthTexture>::field_data,
    gsl::span<std::type_index>{},
    {}, // attribute table
    &RecordReflect<ag::RenderTarget::DepthTexture>::serialize, // serialize
    nullptr                                                    // deserialize
};

template <> const Type *typeOf<ag::RenderTarget::DepthTexture>() {
  return &RecordReflect<ag::RenderTarget::DepthTexture>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::NoDepth
template <> struct RecordReflect<ag::RenderTarget::NoDepth> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::RenderTarget::NoDepth>::metaobj{
    "ag::RenderTarget::NoDepth",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                                   // attribute table
    &RecordReflect<ag::RenderTarget::NoDepth>::serialize, // serialize
    nullptr                                               // deserialize
};

template <> const Type *typeOf<ag::RenderTarget::NoDepth>() {
  return &RecordReflect<ag::RenderTarget::NoDepth>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::Samples
template <> struct RecordReflect<ag::RenderTarget::Samples> {
  // attribute instances

  static const std::array<Field, 1> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_count(void *ptr) {
    return &reinterpret_cast<ag::RenderTarget::Samples *>(ptr)->count;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 1> RecordReflect<ag::RenderTarget::Samples>::field_data{
    {
        Field{"count",
              "ag::RenderTarget::Samples::count",
              std::type_index{typeid(int)},
              offsetof(ag::RenderTarget::Samples, count),
              &RecordReflect<ag::RenderTarget::Samples>::field_thunk_count,
              {}},
    }};

const Record RecordReflect<ag::RenderTarget::Samples>::metaobj{
    "ag::RenderTarget::Samples",
    RecordReflect<ag::RenderTarget::Samples>::field_data,
    gsl::span<std::type_index>{},
    {},                                                   // attribute table
    &RecordReflect<ag::RenderTarget::Samples>::serialize, // serialize
    nullptr                                               // deserialize
};

template <> const Type *typeOf<ag::RenderTarget::Samples>() {
  return &RecordReflect<ag::RenderTarget::Samples>::metaobj;
}

///////////////////////////
// struct ag::EntityManager
template <> struct RecordReflect<ag::EntityManager> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::EntityManager>::metaobj{
    "ag::EntityManager",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                           // attribute table
    &RecordReflect<ag::EntityManager>::serialize, // serialize
    nullptr                                       // deserialize
};

template <> const Type *typeOf<ag::EntityManager>() {
  return &RecordReflect<ag::EntityManager>::metaobj;
}

///////////////////////////
// struct ag::ComponentManagerBase
template <> struct RecordReflect<ag::ComponentManagerBase> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::ComponentManagerBase>::metaobj{
    "ag::ComponentManagerBase",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                                  // attribute table
    &RecordReflect<ag::ComponentManagerBase>::serialize, // serialize
    nullptr                                              // deserialize
};

template <> const Type *typeOf<ag::ComponentManagerBase>() {
  return &RecordReflect<ag::ComponentManagerBase>::metaobj;
}

///////////////////////////
// struct ag::Scene
template <> struct RecordReflect<ag::Scene> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::Scene>::metaobj{
    "ag::Scene",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                   // attribute table
    &RecordReflect<ag::Scene>::serialize, // serialize
    nullptr                               // deserialize
};

template <> const Type *typeOf<ag::Scene>() {
  return &RecordReflect<ag::Scene>::metaobj;
}

///////////////////////////
// struct ag::RenderableComponents
template <> struct RecordReflect<ag::RenderableComponents> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1>
    RecordReflect<ag::RenderableComponents>::bases_data{{
        std::type_index{typeid(ag::ComponentManager<ag::StdMaterial>)},
    }};

const Record RecordReflect<ag::RenderableComponents>::metaobj{
    "ag::RenderableComponents",
    gsl::span<Field>{},
    RecordReflect<ag::RenderableComponents>::bases_data,
    {},                                                  // attribute table
    &RecordReflect<ag::RenderableComponents>::serialize, // serialize
    nullptr                                              // deserialize
};

template <> const Type *typeOf<ag::RenderableComponents>() {
  return &RecordReflect<ag::RenderableComponents>::metaobj;
}

///////////////////////////
// struct ag::SceneObject
template <> struct RecordReflect<ag::SceneObject> {
  // attribute instances

  static const std::array<Field, 10> field_data;
  static const Record metaobj;

  // field access thunks
  static void *field_thunk_entityID(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->entityID;
  }
  static void *field_thunk_name(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->name;
  }
  static void *field_thunk_parent(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->parent;
  }
  static void *field_thunk_mesh(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->mesh;
  }
  static void *field_thunk_localTransform(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->localTransform;
  }
  static void *field_thunk_worldTransform(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->worldTransform;
  }
  static void *field_thunk_children(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->children;
  }
  static void *field_thunk_worldBounds(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->worldBounds;
  }
  static void *field_thunk_localBounds(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->localBounds;
  }
  static void *field_thunk_hasWorldBounds(void *ptr) {
    return &reinterpret_cast<ag::SceneObject *>(ptr)->hasWorldBounds;
  }

  static void serialize(packer &p, const void *data);
};

const std::array<Field, 10> RecordReflect<ag::SceneObject>::field_data{{
    Field{"entityID",
          "ag::SceneObject::entityID",
          std::type_index{typeid(unsigned long long)},
          offsetof(ag::SceneObject, entityID),
          &RecordReflect<ag::SceneObject>::field_thunk_entityID,
          {}},
    Field{"name",
          "ag::SceneObject::name",
          std::type_index{typeid(std::basic_string<char, std::char_traits<char>,
                                                   std::allocator<char>>)},
          offsetof(ag::SceneObject, name),
          &RecordReflect<ag::SceneObject>::field_thunk_name,
          {}},
    Field{"parent",
          "ag::SceneObject::parent",
          std::type_index{typeid(ag::SceneObject *)},
          offsetof(ag::SceneObject, parent),
          &RecordReflect<ag::SceneObject>::field_thunk_parent,
          {}},
    Field{"mesh",
          "ag::SceneObject::mesh",
          std::type_index{typeid(ag::Mesh<ag::Vertex3D> *)},
          offsetof(ag::SceneObject, mesh),
          &RecordReflect<ag::SceneObject>::field_thunk_mesh,
          {}},
    Field{"localTransform",
          "ag::SceneObject::localTransform",
          std::type_index{typeid(ag::Transform)},
          offsetof(ag::SceneObject, localTransform),
          &RecordReflect<ag::SceneObject>::field_thunk_localTransform,
          {}},
    Field{"worldTransform",
          "ag::SceneObject::worldTransform",
          std::type_index{
              typeid(glm::tmat4x4<float, glm::precision::packed_highp>)},
          offsetof(ag::SceneObject, worldTransform),
          &RecordReflect<ag::SceneObject>::field_thunk_worldTransform,
          {}},
    Field{
        "children",
        "ag::SceneObject::children",
        std::type_index{typeid(
            std::vector<ag::SceneObject *, std::allocator<ag::SceneObject *>>)},
        offsetof(ag::SceneObject, children),
        &RecordReflect<ag::SceneObject>::field_thunk_children,
        {}},
    Field{"worldBounds",
          "ag::SceneObject::worldBounds",
          std::type_index{typeid(ag::AABB)},
          offsetof(ag::SceneObject, worldBounds),
          &RecordReflect<ag::SceneObject>::field_thunk_worldBounds,
          {}},
    Field{"localBounds",
          "ag::SceneObject::localBounds",
          std::type_index{typeid(ag::AABB)},
          offsetof(ag::SceneObject, localBounds),
          &RecordReflect<ag::SceneObject>::field_thunk_localBounds,
          {}},
    Field{"hasWorldBounds",
          "ag::SceneObject::hasWorldBounds",
          std::type_index{typeid(bool)},
          offsetof(ag::SceneObject, hasWorldBounds),
          &RecordReflect<ag::SceneObject>::field_thunk_hasWorldBounds,
          {}},
}};

const Record RecordReflect<ag::SceneObject>::metaobj{
    "ag::SceneObject",
    RecordReflect<ag::SceneObject>::field_data,
    gsl::span<std::type_index>{},
    {},                                         // attribute table
    &RecordReflect<ag::SceneObject>::serialize, // serialize
    nullptr                                     // deserialize
};

template <> const Type *typeOf<ag::SceneObject>() {
  return &RecordReflect<ag::SceneObject>::metaobj;
}

///////////////////////////
// struct ag::SceneObjectComponents
template <> struct RecordReflect<ag::SceneObjectComponents> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1>
    RecordReflect<ag::SceneObjectComponents>::bases_data{{
        std::type_index{typeid(ag::ComponentManager<ag::SceneObject>)},
    }};

const Record RecordReflect<ag::SceneObjectComponents>::metaobj{
    "ag::SceneObjectComponents",
    gsl::span<Field>{},
    RecordReflect<ag::SceneObjectComponents>::bases_data,
    {},                                                   // attribute table
    &RecordReflect<ag::SceneObjectComponents>::serialize, // serialize
    nullptr                                               // deserialize
};

template <> const Type *typeOf<ag::SceneObjectComponents>() {
  return &RecordReflect<ag::SceneObjectComponents>::metaobj;
}

///////////////////////////
// struct ag::Light
template <> struct RecordReflect<ag::Light> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::Light>::metaobj{
    "ag::Light",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                   // attribute table
    &RecordReflect<ag::Light>::serialize, // serialize
    nullptr                               // deserialize
};

template <> const Type *typeOf<ag::Light>() {
  return &RecordReflect<ag::Light>::metaobj;
}

///////////////////////////
// struct ag::LightComponents
template <> struct RecordReflect<ag::LightComponents> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1>
    RecordReflect<ag::LightComponents>::bases_data{{
        std::type_index{typeid(ag::ComponentManager<ag::Light>)},
    }};

const Record RecordReflect<ag::LightComponents>::metaobj{
    "ag::LightComponents",
    gsl::span<Field>{},
    RecordReflect<ag::LightComponents>::bases_data,
    {},                                             // attribute table
    &RecordReflect<ag::LightComponents>::serialize, // serialize
    nullptr                                         // deserialize
};

template <> const Type *typeOf<ag::LightComponents>() {
  return &RecordReflect<ag::LightComponents>::metaobj;
}

///////////////////////////
// struct ag::ISceneEditor
template <> struct RecordReflect<ag::ISceneEditor> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1>
    RecordReflect<ag::ISceneEditor>::bases_data{{
        std::type_index{typeid(ag::Extension)},
    }};

const Record RecordReflect<ag::ISceneEditor>::metaobj{
    "ag::ISceneEditor",
    gsl::span<Field>{},
    RecordReflect<ag::ISceneEditor>::bases_data,
    {},                                          // attribute table
    &RecordReflect<ag::ISceneEditor>::serialize, // serialize
    nullptr                                      // deserialize
};

template <> const Type *typeOf<ag::ISceneEditor>() {
  return &RecordReflect<ag::ISceneEditor>::metaobj;
}

///////////////////////////
// struct ag::ScriptContext
template <> struct RecordReflect<ag::ScriptContext> {
  // attribute instances

  static const std::array<std::type_index, 1> bases_data;
  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const std::array<std::type_index, 1>
    RecordReflect<ag::ScriptContext>::bases_data{{
        std::type_index{typeid(sol::state)},
    }};

const Record RecordReflect<ag::ScriptContext>::metaobj{
    "ag::ScriptContext",
    gsl::span<Field>{},
    RecordReflect<ag::ScriptContext>::bases_data,
    {},                                           // attribute table
    &RecordReflect<ag::ScriptContext>::serialize, // serialize
    nullptr                                       // deserialize
};

template <> const Type *typeOf<ag::ScriptContext>() {
  return &RecordReflect<ag::ScriptContext>::metaobj;
}

///////////////////////////
// struct ag::PipelineStateCache
template <> struct RecordReflect<ag::PipelineStateCache> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::PipelineStateCache>::metaobj{
    "ag::PipelineStateCache",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                                // attribute table
    &RecordReflect<ag::PipelineStateCache>::serialize, // serialize
    nullptr                                            // deserialize
};

template <> const Type *typeOf<ag::PipelineStateCache>() {
  return &RecordReflect<ag::PipelineStateCache>::metaobj;
}

///////////////////////////
// struct ag::Shader
template <> struct RecordReflect<ag::Shader> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::Shader>::metaobj{
    "ag::Shader",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                    // attribute table
    &RecordReflect<ag::Shader>::serialize, // serialize
    nullptr                                // deserialize
};

template <> const Type *typeOf<ag::Shader>() {
  return &RecordReflect<ag::Shader>::metaobj;
}

///////////////////////////
// struct ag::Window
template <> struct RecordReflect<ag::Window> {
  // attribute instances

  static const Record metaobj;

  // field access thunks

  static void serialize(packer &p, const void *data);
};

const Record RecordReflect<ag::Window>::metaobj{
    "ag::Window",
    gsl::span<Field>{},
    gsl::span<std::type_index>{},
    {},                                    // attribute table
    &RecordReflect<ag::Window>::serialize, // serialize
    nullptr                                // deserialize
};

template <> const Type *typeOf<ag::Window>() {
  return &RecordReflect<ag::Window>::metaobj;
}

/////////////////////////////////////////////////////////////////////
// ENUM REFLECTION DATA

///////////////////////////
// helpers
template <typename T> uint64_t getEnumValue(const void *data) {
  return *reinterpret_cast<const T *>(data);
}
template <typename T> void setEnumValue(void *data, uint64_t value) {
  *reinterpret_cast<T *>(data) = value;
}

template <typename T> struct EnumReflect;

/////////////////////////////////////////////////////////////////////
// SERIALIZATION

void RecordReflect<ag::Arcball>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::Arcball*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::CVarBase>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::CVarBase*>(data);
  p.pack_map(1);
  p.pack("id");
  serialize_impl(p, obj->id);
  */
}

void RecordReflect<ag::ResourcePool>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::ResourcePool*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::Action>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::Action*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::KeyAction>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::KeyAction*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::GamepadButtonAction>::serialize(packer &p,
                                                       const void *data) {
  /*auto obj = reinterpret_cast<const ag::GamepadButtonAction*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::gl::VertexAttribute>::serialize(packer &p,
                                                       const void *data) {
  /*auto obj = reinterpret_cast<const ag::gl::VertexAttribute*>(data);
  p.pack_map(5);
  p.pack("slot");
  serialize_impl(p, obj->slot);
  p.pack("type");
  serialize_impl(p, obj->type);
  p.pack("size");
  serialize_impl(p, obj->size);
  p.pack("relativeOffset");
  serialize_impl(p, obj->relativeOffset);
  p.pack("normalized");
  serialize_impl(p, obj->normalized);
  */
}

void RecordReflect<ag::Profiler::Scope>::serialize(packer &p,
                                                   const void *data) {
  /*auto obj = reinterpret_cast<const ag::Profiler::Scope*>(data);
  p.pack_map(13);
  p.pack("gpuProfile");
  serialize_impl(p, obj->gpuProfile);
  p.pack("gpuClientTimestampRef");
  serialize_impl(p, obj->gpuClientTimestampRef);
  p.pack("gpuTimestampStartQuery");
  serialize_impl(p, obj->gpuTimestampStartQuery);
  p.pack("gpuTimestampEndQuery");
  serialize_impl(p, obj->gpuTimestampEndQuery);
  p.pack("name");
  serialize_impl(p, obj->name);
  p.pack("start");
  serialize_impl(p, obj->start);
  p.pack("end");
  serialize_impl(p, obj->end);
  p.pack("gpuStart");
  serialize_impl(p, obj->gpuStart);
  p.pack("gpuEnd");
  serialize_impl(p, obj->gpuEnd);
  p.pack("id");
  serialize_impl(p, obj->id);
  p.pack("parent");
  serialize_impl(p, obj->parent);
  p.pack("next");
  serialize_impl(p, obj->next);
  p.pack("firstChild");
  serialize_impl(p, obj->firstChild);
  */
}

void RecordReflect<ag::Profiler::ProfileData>::serialize(packer &p,
                                                         const void *data) {
  /*auto obj = reinterpret_cast<const ag::Profiler::ProfileData*>(data);
  p.pack_map(4);
  p.pack("frameId");
  serialize_impl(p, obj->frameId);
  p.pack("frameStartTime");
  serialize_impl(p, obj->frameStartTime);
  p.pack("frameEndTime");
  serialize_impl(p, obj->frameEndTime);
  p.pack("scopes");
  serialize_impl(p, obj->scopes);
  */
}

void RecordReflect<ag::RenderTarget>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::RenderTarget*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::RenderTarget::DepthRenderbuffer>::serialize(
    packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const
  ag::RenderTarget::DepthRenderbuffer*>(data); p.pack_map(1); p.pack("fmt");
  serialize_impl(p, obj->fmt);
  */
}

void RecordReflect<ag::RenderTarget::DepthTexture>::serialize(
    packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::RenderTarget::DepthTexture*>(data);
  p.pack_map(1);
  p.pack("fmt");
  serialize_impl(p, obj->fmt);
  */
}

void RecordReflect<ag::RenderTarget::NoDepth>::serialize(packer &p,
                                                         const void *data) {
  /*auto obj = reinterpret_cast<const ag::RenderTarget::NoDepth*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::RenderTarget::Samples>::serialize(packer &p,
                                                         const void *data) {
  /*auto obj = reinterpret_cast<const ag::RenderTarget::Samples*>(data);
  p.pack_map(1);
  p.pack("count");
  serialize_impl(p, obj->count);
  */
}

void RecordReflect<ag::EntityManager>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::EntityManager*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::ComponentManagerBase>::serialize(packer &p,
                                                        const void *data) {
  /*auto obj = reinterpret_cast<const ag::ComponentManagerBase*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::Scene>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::Scene*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::RenderableComponents>::serialize(packer &p,
                                                        const void *data) {
  /*auto obj = reinterpret_cast<const ag::RenderableComponents*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::SceneObject>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::SceneObject*>(data);
  p.pack_map(10);
  p.pack("entityID");
  serialize_impl(p, obj->entityID);
  p.pack("name");
  serialize_impl(p, obj->name);
  p.pack("parent");
  serialize_impl(p, obj->parent);
  p.pack("mesh");
  serialize_impl(p, obj->mesh);
  p.pack("localTransform");
  serialize_impl(p, obj->localTransform);
  p.pack("worldTransform");
  serialize_impl(p, obj->worldTransform);
  p.pack("children");
  serialize_impl(p, obj->children);
  p.pack("worldBounds");
  serialize_impl(p, obj->worldBounds);
  p.pack("localBounds");
  serialize_impl(p, obj->localBounds);
  p.pack("hasWorldBounds");
  serialize_impl(p, obj->hasWorldBounds);
  */
}

void RecordReflect<ag::SceneObjectComponents>::serialize(packer &p,
                                                         const void *data) {
  /*auto obj = reinterpret_cast<const ag::SceneObjectComponents*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::Light>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::Light*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::LightComponents>::serialize(packer &p,
                                                   const void *data) {
  /*auto obj = reinterpret_cast<const ag::LightComponents*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::ISceneEditor>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::ISceneEditor*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::ScriptContext>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::ScriptContext*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::PipelineStateCache>::serialize(packer &p,
                                                      const void *data) {
  /*auto obj = reinterpret_cast<const ag::PipelineStateCache*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::Shader>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::Shader*>(data);
  p.pack_map(0);
  */
}

void RecordReflect<ag::Window>::serialize(packer &p, const void *data) {
  /*auto obj = reinterpret_cast<const ag::Window*>(data);
  p.pack_map(0);
  */
}

/////////////////////////////////////////////////////////////////////
// TYPEINDEX MAP
static std::unordered_map<std::type_index, const Type *>
    typeindex_to_metaobject;
static bool typeindex_to_metaobject_initialized = false;

const Type *typeOf(std::type_index ti) {
  if (!typeindex_to_metaobject_initialized) {
    using kv_pair = decltype(typeindex_to_metaobject)::value_type;
    typeindex_to_metaobject.insert({
        kv_pair{typeid(ag::Arcball), &RecordReflect<ag::Arcball>::metaobj},
        kv_pair{typeid(ag::CVarBase), &RecordReflect<ag::CVarBase>::metaobj},
        kv_pair{typeid(ag::ResourcePool),
                &RecordReflect<ag::ResourcePool>::metaobj},
        kv_pair{typeid(ag::Action), &RecordReflect<ag::Action>::metaobj},
        kv_pair{typeid(ag::KeyAction), &RecordReflect<ag::KeyAction>::metaobj},
        kv_pair{typeid(ag::GamepadButtonAction),
                &RecordReflect<ag::GamepadButtonAction>::metaobj},
        kv_pair{typeid(ag::gl::VertexAttribute),
                &RecordReflect<ag::gl::VertexAttribute>::metaobj},
        kv_pair{typeid(ag::Profiler::Scope),
                &RecordReflect<ag::Profiler::Scope>::metaobj},
        kv_pair{typeid(ag::Profiler::ProfileData),
                &RecordReflect<ag::Profiler::ProfileData>::metaobj},
        kv_pair{typeid(ag::RenderTarget),
                &RecordReflect<ag::RenderTarget>::metaobj},
        kv_pair{typeid(ag::RenderTarget::DepthRenderbuffer),
                &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::metaobj},
        kv_pair{typeid(ag::RenderTarget::DepthTexture),
                &RecordReflect<ag::RenderTarget::DepthTexture>::metaobj},
        kv_pair{typeid(ag::RenderTarget::NoDepth),
                &RecordReflect<ag::RenderTarget::NoDepth>::metaobj},
        kv_pair{typeid(ag::RenderTarget::Samples),
                &RecordReflect<ag::RenderTarget::Samples>::metaobj},
        kv_pair{typeid(ag::EntityManager),
                &RecordReflect<ag::EntityManager>::metaobj},
        kv_pair{typeid(ag::ComponentManagerBase),
                &RecordReflect<ag::ComponentManagerBase>::metaobj},
        kv_pair{typeid(ag::Scene), &RecordReflect<ag::Scene>::metaobj},
        kv_pair{typeid(ag::RenderableComponents),
                &RecordReflect<ag::RenderableComponents>::metaobj},
        kv_pair{typeid(ag::SceneObject),
                &RecordReflect<ag::SceneObject>::metaobj},
        kv_pair{typeid(ag::SceneObjectComponents),
                &RecordReflect<ag::SceneObjectComponents>::metaobj},
        kv_pair{typeid(ag::Light), &RecordReflect<ag::Light>::metaobj},
        kv_pair{typeid(ag::LightComponents),
                &RecordReflect<ag::LightComponents>::metaobj},
        kv_pair{typeid(ag::ISceneEditor),
                &RecordReflect<ag::ISceneEditor>::metaobj},
        kv_pair{typeid(ag::ScriptContext),
                &RecordReflect<ag::ScriptContext>::metaobj},
        kv_pair{typeid(ag::PipelineStateCache),
                &RecordReflect<ag::PipelineStateCache>::metaobj},
        kv_pair{typeid(ag::Shader), &RecordReflect<ag::Shader>::metaobj},
        kv_pair{typeid(ag::Window), &RecordReflect<ag::Window>::metaobj},
    });
    typeindex_to_metaobject_initialized = true;
  }
  auto it = typeindex_to_metaobject.find(ti);
  if (it != typeindex_to_metaobject.end()) {
    return it->second;
  }
  return nullptr;
}

/*template <> struct meta_attr<ag::Arcball> {
        // attribute instances for ag::Arcball
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::CameraControl> {
        // attribute instances for ag::CameraControl
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::CVarBase> {
        // attribute instances for ag::CVarBase
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::ResourcePool> {
        // attribute instances for ag::ResourcePool
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Action> {
        // attribute instances for ag::Action
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::KeyAction> {
        // attribute instances for ag::KeyAction
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::GamepadButtonAction> {
        // attribute instances for ag::GamepadButtonAction
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::gl::VertexAttribute> {
        // attribute instances for ag::gl::VertexAttribute
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Profiler::Scope> {
        // attribute instances for ag::Profiler::Scope
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Profiler::ProfileData> {
        // attribute instances for ag::Profiler::ProfileData
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::RenderTarget> {
        // attribute instances for ag::RenderTarget
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::RenderTarget::DepthRenderbuffer> {
        // attribute instances for ag::RenderTarget::DepthRenderbuffer
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::RenderTarget::DepthTexture> {
        // attribute instances for ag::RenderTarget::DepthTexture
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::RenderTarget::NoDepth> {
        // attribute instances for ag::RenderTarget::NoDepth
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::RenderTarget::Samples> {
        // attribute instances for ag::RenderTarget::Samples
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::EntityManager> {
        // attribute instances for ag::EntityManager
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::ComponentManagerBase> {
        // attribute instances for ag::ComponentManagerBase
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Scene> {
        // attribute instances for ag::Scene
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::RenderableComponents> {
        // attribute instances for ag::RenderableComponents
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::SceneObject> {
        // attribute instances for ag::SceneObject
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::SceneObjectComponents> {
        // attribute instances for ag::SceneObjectComponents
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Light> {
        // attribute instances for ag::Light
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::LightComponents> {
        // attribute instances for ag::LightComponents
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::ISceneEditor> {
        // attribute instances for ag::ISceneEditor
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::ScriptContext> {
        // attribute instances for ag::ScriptContext
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::PipelineStateCache> {
        // attribute instances for ag::PipelineStateCache
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Shader> {
        // attribute instances for ag::Shader
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/
/*template <> struct meta_attr<ag::Window> {
        // attribute instances for ag::Window
        // attribute table
        const std::array<meta_attr_entry, 0>& table() {
                static std::array<meta_attr_entry, 0> data_ = { {
                } };
                return data_;
        }
};*/

} // namespace meta
} // namespace ag