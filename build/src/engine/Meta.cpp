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

///////////////////////////
// struct ag::ResourceManager
template <>
struct RecordReflect<ag::ResourceManager> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::ResourceManager>::metaobj { "ag::ResourceManager", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::ResourceManager>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::ResourceManager>() {
return &RecordReflect<ag::ResourceManager>::metaobj;
}

///////////////////////////
// struct ag::ResourcePool
template <>
struct RecordReflect<ag::ResourcePool> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::ResourcePool>::metaobj { "ag::ResourcePool", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::ResourcePool>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::ResourcePool>() {
return &RecordReflect<ag::ResourcePool>::metaobj;
}

///////////////////////////
// struct ag::ImageFormatInfo
template <>
struct RecordReflect<ag::ImageFormatInfo> {
	// attribute instances



	static const std::array<Field, 3> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_name(void* ptr) {
		return &reinterpret_cast<ag::ImageFormatInfo*>(ptr)->name;
	}
	static void* field_thunk_size(void* ptr) {
		return &reinterpret_cast<ag::ImageFormatInfo*>(ptr)->size;
	}
	static void* field_thunk_numChannels(void* ptr) {
		return &reinterpret_cast<ag::ImageFormatInfo*>(ptr)->numChannels;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 3> RecordReflect<ag::ImageFormatInfo>::field_data  { {
	Field { "name", "ag::ImageFormatInfo::name", std::type_index{typeid(const char *)}, offsetof(ag::ImageFormatInfo, name), &RecordReflect<ag::ImageFormatInfo>::field_thunk_name, 
	  {}  }, 
	Field { "size", "ag::ImageFormatInfo::size", std::type_index{typeid(unsigned int)}, offsetof(ag::ImageFormatInfo, size), &RecordReflect<ag::ImageFormatInfo>::field_thunk_size, 
	  {}  }, 
	Field { "numChannels", "ag::ImageFormatInfo::numChannels", std::type_index{typeid(unsigned int)}, offsetof(ag::ImageFormatInfo, numChannels), &RecordReflect<ag::ImageFormatInfo>::field_thunk_numChannels, 
	  {}  }, 
}};



const Record RecordReflect<ag::ImageFormatInfo>::metaobj { "ag::ImageFormatInfo", 
	 RecordReflect<ag::ImageFormatInfo>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::ImageFormatInfo>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::ImageFormatInfo>() {
return &RecordReflect<ag::ImageFormatInfo>::metaobj;
}

///////////////////////////
// struct ag::gl::Texture
template <>
struct RecordReflect<ag::gl::Texture> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Texture>::metaobj { "ag::gl::Texture", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Texture>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Texture>() {
return &RecordReflect<ag::gl::Texture>::metaobj;
}

///////////////////////////
// struct ag::gl::Texture::MipMaps
template <>
struct RecordReflect<ag::gl::Texture::MipMaps> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_count(void* ptr) {
		return &reinterpret_cast<ag::gl::Texture::MipMaps*>(ptr)->count;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::gl::Texture::MipMaps>::field_data  { {
	Field { "count", "ag::gl::Texture::MipMaps::count", std::type_index{typeid(int)}, offsetof(ag::gl::Texture::MipMaps, count), &RecordReflect<ag::gl::Texture::MipMaps>::field_thunk_count, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::Texture::MipMaps>::metaobj { "ag::gl::Texture::MipMaps", 
	 RecordReflect<ag::gl::Texture::MipMaps>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Texture::MipMaps>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Texture::MipMaps>() {
return &RecordReflect<ag::gl::Texture::MipMaps>::metaobj;
}

///////////////////////////
// struct ag::gl::Texture::Samples
template <>
struct RecordReflect<ag::gl::Texture::Samples> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_count(void* ptr) {
		return &reinterpret_cast<ag::gl::Texture::Samples*>(ptr)->count;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::gl::Texture::Samples>::field_data  { {
	Field { "count", "ag::gl::Texture::Samples::count", std::type_index{typeid(int)}, offsetof(ag::gl::Texture::Samples, count), &RecordReflect<ag::gl::Texture::Samples>::field_thunk_count, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::Texture::Samples>::metaobj { "ag::gl::Texture::Samples", 
	 RecordReflect<ag::gl::Texture::Samples>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Texture::Samples>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Texture::Samples>() {
return &RecordReflect<ag::gl::Texture::Samples>::metaobj;
}

///////////////////////////
// struct ag::gl::Buffer
template <>
struct RecordReflect<ag::gl::Buffer> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Buffer>::metaobj { "ag::gl::Buffer", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Buffer>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Buffer>() {
return &RecordReflect<ag::gl::Buffer>::metaobj;
}

///////////////////////////
// struct ag::gl::Renderbuffer
template <>
struct RecordReflect<ag::gl::Renderbuffer> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Renderbuffer>::metaobj { "ag::gl::Renderbuffer", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Renderbuffer>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Renderbuffer>() {
return &RecordReflect<ag::gl::Renderbuffer>::metaobj;
}

///////////////////////////
// struct ag::gl::Renderbuffer::Samples
template <>
struct RecordReflect<ag::gl::Renderbuffer::Samples> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_count(void* ptr) {
		return &reinterpret_cast<ag::gl::Renderbuffer::Samples*>(ptr)->count;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::gl::Renderbuffer::Samples>::field_data  { {
	Field { "count", "ag::gl::Renderbuffer::Samples::count", std::type_index{typeid(int)}, offsetof(ag::gl::Renderbuffer::Samples, count), &RecordReflect<ag::gl::Renderbuffer::Samples>::field_thunk_count, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::Renderbuffer::Samples>::metaobj { "ag::gl::Renderbuffer::Samples", 
	 RecordReflect<ag::gl::Renderbuffer::Samples>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Renderbuffer::Samples>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Renderbuffer::Samples>() {
return &RecordReflect<ag::gl::Renderbuffer::Samples>::metaobj;
}

///////////////////////////
// struct ag::gl::Framebuffer
template <>
struct RecordReflect<ag::gl::Framebuffer> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Framebuffer>::metaobj { "ag::gl::Framebuffer", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Framebuffer>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Framebuffer>() {
return &RecordReflect<ag::gl::Framebuffer>::metaobj;
}

///////////////////////////
// struct ag::gl::BlendState
template <>
struct RecordReflect<ag::gl::BlendState> {
	// attribute instances



	static const std::array<Field, 7> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_enabled(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->enabled;
	}
	static void* field_thunk_modeRGB(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->modeRGB;
	}
	static void* field_thunk_modeAlpha(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->modeAlpha;
	}
	static void* field_thunk_funcSrcRGB(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->funcSrcRGB;
	}
	static void* field_thunk_funcDstRGB(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->funcDstRGB;
	}
	static void* field_thunk_funcSrcAlpha(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->funcSrcAlpha;
	}
	static void* field_thunk_funcDstAlpha(void* ptr) {
		return &reinterpret_cast<ag::gl::BlendState*>(ptr)->funcDstAlpha;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 7> RecordReflect<ag::gl::BlendState>::field_data  { {
	Field { "enabled", "ag::gl::BlendState::enabled", std::type_index{typeid(bool)}, offsetof(ag::gl::BlendState, enabled), &RecordReflect<ag::gl::BlendState>::field_thunk_enabled, 
	  {}  }, 
	Field { "modeRGB", "ag::gl::BlendState::modeRGB", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::BlendState, modeRGB), &RecordReflect<ag::gl::BlendState>::field_thunk_modeRGB, 
	  {}  }, 
	Field { "modeAlpha", "ag::gl::BlendState::modeAlpha", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::BlendState, modeAlpha), &RecordReflect<ag::gl::BlendState>::field_thunk_modeAlpha, 
	  {}  }, 
	Field { "funcSrcRGB", "ag::gl::BlendState::funcSrcRGB", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::BlendState, funcSrcRGB), &RecordReflect<ag::gl::BlendState>::field_thunk_funcSrcRGB, 
	  {}  }, 
	Field { "funcDstRGB", "ag::gl::BlendState::funcDstRGB", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::BlendState, funcDstRGB), &RecordReflect<ag::gl::BlendState>::field_thunk_funcDstRGB, 
	  {}  }, 
	Field { "funcSrcAlpha", "ag::gl::BlendState::funcSrcAlpha", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::BlendState, funcSrcAlpha), &RecordReflect<ag::gl::BlendState>::field_thunk_funcSrcAlpha, 
	  {}  }, 
	Field { "funcDstAlpha", "ag::gl::BlendState::funcDstAlpha", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::BlendState, funcDstAlpha), &RecordReflect<ag::gl::BlendState>::field_thunk_funcDstAlpha, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::BlendState>::metaobj { "ag::gl::BlendState", 
	 RecordReflect<ag::gl::BlendState>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::BlendState>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::BlendState>() {
return &RecordReflect<ag::gl::BlendState>::metaobj;
}

///////////////////////////
// struct ag::gl::DepthStencilState
template <>
struct RecordReflect<ag::gl::DepthStencilState> {
	// attribute instances



	static const std::array<Field, 11> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_depthTestEnable(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->depthTestEnable;
	}
	static void* field_thunk_depthWriteEnable(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->depthWriteEnable;
	}
	static void* field_thunk_stencilEnable(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilEnable;
	}
	static void* field_thunk_depthTestFunc(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->depthTestFunc;
	}
	static void* field_thunk_stencilFace(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilFace;
	}
	static void* field_thunk_stencilFunc(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilFunc;
	}
	static void* field_thunk_stencilRef(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilRef;
	}
	static void* field_thunk_stencilMask(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilMask;
	}
	static void* field_thunk_stencilOpSfail(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilOpSfail;
	}
	static void* field_thunk_stencilOpDPFail(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilOpDPFail;
	}
	static void* field_thunk_stencilOpDPPass(void* ptr) {
		return &reinterpret_cast<ag::gl::DepthStencilState*>(ptr)->stencilOpDPPass;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 11> RecordReflect<ag::gl::DepthStencilState>::field_data  { {
	Field { "depthTestEnable", "ag::gl::DepthStencilState::depthTestEnable", std::type_index{typeid(bool)}, offsetof(ag::gl::DepthStencilState, depthTestEnable), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_depthTestEnable, 
	  {}  }, 
	Field { "depthWriteEnable", "ag::gl::DepthStencilState::depthWriteEnable", std::type_index{typeid(bool)}, offsetof(ag::gl::DepthStencilState, depthWriteEnable), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_depthWriteEnable, 
	  {}  }, 
	Field { "stencilEnable", "ag::gl::DepthStencilState::stencilEnable", std::type_index{typeid(bool)}, offsetof(ag::gl::DepthStencilState, stencilEnable), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilEnable, 
	  {}  }, 
	Field { "depthTestFunc", "ag::gl::DepthStencilState::depthTestFunc", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, depthTestFunc), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_depthTestFunc, 
	  {}  }, 
	Field { "stencilFace", "ag::gl::DepthStencilState::stencilFace", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, stencilFace), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilFace, 
	  {}  }, 
	Field { "stencilFunc", "ag::gl::DepthStencilState::stencilFunc", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, stencilFunc), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilFunc, 
	  {}  }, 
	Field { "stencilRef", "ag::gl::DepthStencilState::stencilRef", std::type_index{typeid(int)}, offsetof(ag::gl::DepthStencilState, stencilRef), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilRef, 
	  {}  }, 
	Field { "stencilMask", "ag::gl::DepthStencilState::stencilMask", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, stencilMask), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilMask, 
	  {}  }, 
	Field { "stencilOpSfail", "ag::gl::DepthStencilState::stencilOpSfail", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, stencilOpSfail), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilOpSfail, 
	  {}  }, 
	Field { "stencilOpDPFail", "ag::gl::DepthStencilState::stencilOpDPFail", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, stencilOpDPFail), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilOpDPFail, 
	  {}  }, 
	Field { "stencilOpDPPass", "ag::gl::DepthStencilState::stencilOpDPPass", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DepthStencilState, stencilOpDPPass), &RecordReflect<ag::gl::DepthStencilState>::field_thunk_stencilOpDPPass, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::DepthStencilState>::metaobj { "ag::gl::DepthStencilState", 
	 RecordReflect<ag::gl::DepthStencilState>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::DepthStencilState>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::DepthStencilState>() {
return &RecordReflect<ag::gl::DepthStencilState>::metaobj;
}

///////////////////////////
// struct ag::gl::RasterizerState
template <>
struct RecordReflect<ag::gl::RasterizerState> {
	// attribute instances



	static const std::array<Field, 7> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_fillMode(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->fillMode;
	}
	static void* field_thunk_cullMode(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->cullMode;
	}
	static void* field_thunk_frontFace(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->frontFace;
	}
	static void* field_thunk_depthBias(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->depthBias;
	}
	static void* field_thunk_slopeScaledDepthBias(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->slopeScaledDepthBias;
	}
	static void* field_thunk_depthClipEnable(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->depthClipEnable;
	}
	static void* field_thunk_scissorEnable(void* ptr) {
		return &reinterpret_cast<ag::gl::RasterizerState*>(ptr)->scissorEnable;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 7> RecordReflect<ag::gl::RasterizerState>::field_data  { {
	Field { "fillMode", "ag::gl::RasterizerState::fillMode", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::RasterizerState, fillMode), &RecordReflect<ag::gl::RasterizerState>::field_thunk_fillMode, 
	  {}  }, 
	Field { "cullMode", "ag::gl::RasterizerState::cullMode", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::RasterizerState, cullMode), &RecordReflect<ag::gl::RasterizerState>::field_thunk_cullMode, 
	  {}  }, 
	Field { "frontFace", "ag::gl::RasterizerState::frontFace", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::RasterizerState, frontFace), &RecordReflect<ag::gl::RasterizerState>::field_thunk_frontFace, 
	  {}  }, 
	Field { "depthBias", "ag::gl::RasterizerState::depthBias", std::type_index{typeid(float)}, offsetof(ag::gl::RasterizerState, depthBias), &RecordReflect<ag::gl::RasterizerState>::field_thunk_depthBias, 
	  {}  }, 
	Field { "slopeScaledDepthBias", "ag::gl::RasterizerState::slopeScaledDepthBias", std::type_index{typeid(float)}, offsetof(ag::gl::RasterizerState, slopeScaledDepthBias), &RecordReflect<ag::gl::RasterizerState>::field_thunk_slopeScaledDepthBias, 
	  {}  }, 
	Field { "depthClipEnable", "ag::gl::RasterizerState::depthClipEnable", std::type_index{typeid(bool)}, offsetof(ag::gl::RasterizerState, depthClipEnable), &RecordReflect<ag::gl::RasterizerState>::field_thunk_depthClipEnable, 
	  {}  }, 
	Field { "scissorEnable", "ag::gl::RasterizerState::scissorEnable", std::type_index{typeid(bool)}, offsetof(ag::gl::RasterizerState, scissorEnable), &RecordReflect<ag::gl::RasterizerState>::field_thunk_scissorEnable, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::RasterizerState>::metaobj { "ag::gl::RasterizerState", 
	 RecordReflect<ag::gl::RasterizerState>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::RasterizerState>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::RasterizerState>() {
return &RecordReflect<ag::gl::RasterizerState>::metaobj;
}

///////////////////////////
// struct ag::gl::ScissorRect
template <>
struct RecordReflect<ag::gl::ScissorRect> {
	// attribute instances



	static const std::array<Field, 4> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_x(void* ptr) {
		return &reinterpret_cast<ag::gl::ScissorRect*>(ptr)->x;
	}
	static void* field_thunk_y(void* ptr) {
		return &reinterpret_cast<ag::gl::ScissorRect*>(ptr)->y;
	}
	static void* field_thunk_w(void* ptr) {
		return &reinterpret_cast<ag::gl::ScissorRect*>(ptr)->w;
	}
	static void* field_thunk_h(void* ptr) {
		return &reinterpret_cast<ag::gl::ScissorRect*>(ptr)->h;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 4> RecordReflect<ag::gl::ScissorRect>::field_data  { {
	Field { "x", "ag::gl::ScissorRect::x", std::type_index{typeid(int)}, offsetof(ag::gl::ScissorRect, x), &RecordReflect<ag::gl::ScissorRect>::field_thunk_x, 
	  {}  }, 
	Field { "y", "ag::gl::ScissorRect::y", std::type_index{typeid(int)}, offsetof(ag::gl::ScissorRect, y), &RecordReflect<ag::gl::ScissorRect>::field_thunk_y, 
	  {}  }, 
	Field { "w", "ag::gl::ScissorRect::w", std::type_index{typeid(int)}, offsetof(ag::gl::ScissorRect, w), &RecordReflect<ag::gl::ScissorRect>::field_thunk_w, 
	  {}  }, 
	Field { "h", "ag::gl::ScissorRect::h", std::type_index{typeid(int)}, offsetof(ag::gl::ScissorRect, h), &RecordReflect<ag::gl::ScissorRect>::field_thunk_h, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::ScissorRect>::metaobj { "ag::gl::ScissorRect", 
	 RecordReflect<ag::gl::ScissorRect>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::ScissorRect>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::ScissorRect>() {
return &RecordReflect<ag::gl::ScissorRect>::metaobj;
}

///////////////////////////
// struct ag::gl::Viewport
template <>
struct RecordReflect<ag::gl::Viewport> {
	// attribute instances



	static const std::array<Field, 4> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_x(void* ptr) {
		return &reinterpret_cast<ag::gl::Viewport*>(ptr)->x;
	}
	static void* field_thunk_y(void* ptr) {
		return &reinterpret_cast<ag::gl::Viewport*>(ptr)->y;
	}
	static void* field_thunk_w(void* ptr) {
		return &reinterpret_cast<ag::gl::Viewport*>(ptr)->w;
	}
	static void* field_thunk_h(void* ptr) {
		return &reinterpret_cast<ag::gl::Viewport*>(ptr)->h;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 4> RecordReflect<ag::gl::Viewport>::field_data  { {
	Field { "x", "ag::gl::Viewport::x", std::type_index{typeid(float)}, offsetof(ag::gl::Viewport, x), &RecordReflect<ag::gl::Viewport>::field_thunk_x, 
	  {}  }, 
	Field { "y", "ag::gl::Viewport::y", std::type_index{typeid(float)}, offsetof(ag::gl::Viewport, y), &RecordReflect<ag::gl::Viewport>::field_thunk_y, 
	  {}  }, 
	Field { "w", "ag::gl::Viewport::w", std::type_index{typeid(float)}, offsetof(ag::gl::Viewport, w), &RecordReflect<ag::gl::Viewport>::field_thunk_w, 
	  {}  }, 
	Field { "h", "ag::gl::Viewport::h", std::type_index{typeid(float)}, offsetof(ag::gl::Viewport, h), &RecordReflect<ag::gl::Viewport>::field_thunk_h, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::Viewport>::metaobj { "ag::gl::Viewport", 
	 RecordReflect<ag::gl::Viewport>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Viewport>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Viewport>() {
return &RecordReflect<ag::gl::Viewport>::metaobj;
}

///////////////////////////
// struct ag::gl::Uniforms
template <>
struct RecordReflect<ag::gl::Uniforms> {
	// attribute instances



	static const std::array<Field, 14> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_textures(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->textures;
	}
	static void* field_thunk_samplers(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->samplers;
	}
	static void* field_thunk_images(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->images;
	}
	static void* field_thunk_uniformBuffers(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->uniformBuffers;
	}
	static void* field_thunk_uniformBufferSizes(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->uniformBufferSizes;
	}
	static void* field_thunk_uniformBufferOffsets(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->uniformBufferOffsets;
	}
	static void* field_thunk_shaderStorageBuffers(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->shaderStorageBuffers;
	}
	static void* field_thunk_shaderStorageBufferSizes(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->shaderStorageBufferSizes;
	}
	static void* field_thunk_shaderStorageBufferOffsets(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->shaderStorageBufferOffsets;
	}
	static void* field_thunk_vertexBuffers(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->vertexBuffers;
	}
	static void* field_thunk_vertexBufferOffsets(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->vertexBufferOffsets;
	}
	static void* field_thunk_vertexBufferStrides(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->vertexBufferStrides;
	}
	static void* field_thunk_indexBuffer(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->indexBuffer;
	}
	static void* field_thunk_indexBufferType(void* ptr) {
		return &reinterpret_cast<ag::gl::Uniforms*>(ptr)->indexBufferType;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 14> RecordReflect<ag::gl::Uniforms>::field_data  { {
	Field { "textures", "ag::gl::Uniforms::textures", std::type_index{typeid(std::array<unsigned int, 16>)}, offsetof(ag::gl::Uniforms, textures), &RecordReflect<ag::gl::Uniforms>::field_thunk_textures, 
	  {}  }, 
	Field { "samplers", "ag::gl::Uniforms::samplers", std::type_index{typeid(std::array<unsigned int, 16>)}, offsetof(ag::gl::Uniforms, samplers), &RecordReflect<ag::gl::Uniforms>::field_thunk_samplers, 
	  {}  }, 
	Field { "images", "ag::gl::Uniforms::images", std::type_index{typeid(std::array<unsigned int, 8>)}, offsetof(ag::gl::Uniforms, images), &RecordReflect<ag::gl::Uniforms>::field_thunk_images, 
	  {}  }, 
	Field { "uniformBuffers", "ag::gl::Uniforms::uniformBuffers", std::type_index{typeid(std::array<unsigned int, 8>)}, offsetof(ag::gl::Uniforms, uniformBuffers), &RecordReflect<ag::gl::Uniforms>::field_thunk_uniformBuffers, 
	  {}  }, 
	Field { "uniformBufferSizes", "ag::gl::Uniforms::uniformBufferSizes", std::type_index{typeid(std::array<long long, 8>)}, offsetof(ag::gl::Uniforms, uniformBufferSizes), &RecordReflect<ag::gl::Uniforms>::field_thunk_uniformBufferSizes, 
	  {}  }, 
	Field { "uniformBufferOffsets", "ag::gl::Uniforms::uniformBufferOffsets", std::type_index{typeid(std::array<long long, 8>)}, offsetof(ag::gl::Uniforms, uniformBufferOffsets), &RecordReflect<ag::gl::Uniforms>::field_thunk_uniformBufferOffsets, 
	  {}  }, 
	Field { "shaderStorageBuffers", "ag::gl::Uniforms::shaderStorageBuffers", std::type_index{typeid(std::array<unsigned int, 8>)}, offsetof(ag::gl::Uniforms, shaderStorageBuffers), &RecordReflect<ag::gl::Uniforms>::field_thunk_shaderStorageBuffers, 
	  {}  }, 
	Field { "shaderStorageBufferSizes", "ag::gl::Uniforms::shaderStorageBufferSizes", std::type_index{typeid(std::array<long long, 8>)}, offsetof(ag::gl::Uniforms, shaderStorageBufferSizes), &RecordReflect<ag::gl::Uniforms>::field_thunk_shaderStorageBufferSizes, 
	  {}  }, 
	Field { "shaderStorageBufferOffsets", "ag::gl::Uniforms::shaderStorageBufferOffsets", std::type_index{typeid(std::array<long long, 8>)}, offsetof(ag::gl::Uniforms, shaderStorageBufferOffsets), &RecordReflect<ag::gl::Uniforms>::field_thunk_shaderStorageBufferOffsets, 
	  {}  }, 
	Field { "vertexBuffers", "ag::gl::Uniforms::vertexBuffers", std::type_index{typeid(std::array<unsigned int, 8>)}, offsetof(ag::gl::Uniforms, vertexBuffers), &RecordReflect<ag::gl::Uniforms>::field_thunk_vertexBuffers, 
	  {}  }, 
	Field { "vertexBufferOffsets", "ag::gl::Uniforms::vertexBufferOffsets", std::type_index{typeid(std::array<long long, 8>)}, offsetof(ag::gl::Uniforms, vertexBufferOffsets), &RecordReflect<ag::gl::Uniforms>::field_thunk_vertexBufferOffsets, 
	  {}  }, 
	Field { "vertexBufferStrides", "ag::gl::Uniforms::vertexBufferStrides", std::type_index{typeid(std::array<int, 8>)}, offsetof(ag::gl::Uniforms, vertexBufferStrides), &RecordReflect<ag::gl::Uniforms>::field_thunk_vertexBufferStrides, 
	  {}  }, 
	Field { "indexBuffer", "ag::gl::Uniforms::indexBuffer", std::type_index{typeid(ag::gl::BufferSlice)}, offsetof(ag::gl::Uniforms, indexBuffer), &RecordReflect<ag::gl::Uniforms>::field_thunk_indexBuffer, 
	  {}  }, 
	Field { "indexBufferType", "ag::gl::Uniforms::indexBufferType", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::Uniforms, indexBufferType), &RecordReflect<ag::gl::Uniforms>::field_thunk_indexBufferType, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::Uniforms>::metaobj { "ag::gl::Uniforms", 
	 RecordReflect<ag::gl::Uniforms>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Uniforms>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Uniforms>() {
return &RecordReflect<ag::gl::Uniforms>::metaobj;
}

///////////////////////////
// struct ag::gl::DrawStates
template <>
struct RecordReflect<ag::gl::DrawStates> {
	// attribute instances



	static const std::array<Field, 7> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_depthStencilState(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->depthStencilState;
	}
	static void* field_thunk_rasterizerState(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->rasterizerState;
	}
	static void* field_thunk_scissorRects(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->scissorRects;
	}
	static void* field_thunk_viewports(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->viewports;
	}
	static void* field_thunk_blendStates(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->blendStates;
	}
	static void* field_thunk_vertexArray(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->vertexArray;
	}
	static void* field_thunk_program(void* ptr) {
		return &reinterpret_cast<ag::gl::DrawStates*>(ptr)->program;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 7> RecordReflect<ag::gl::DrawStates>::field_data  { {
	Field { "depthStencilState", "ag::gl::DrawStates::depthStencilState", std::type_index{typeid(ag::gl::DepthStencilState)}, offsetof(ag::gl::DrawStates, depthStencilState), &RecordReflect<ag::gl::DrawStates>::field_thunk_depthStencilState, 
	  {}  }, 
	Field { "rasterizerState", "ag::gl::DrawStates::rasterizerState", std::type_index{typeid(ag::gl::RasterizerState)}, offsetof(ag::gl::DrawStates, rasterizerState), &RecordReflect<ag::gl::DrawStates>::field_thunk_rasterizerState, 
	  {}  }, 
	Field { "scissorRects", "ag::gl::DrawStates::scissorRects", std::type_index{typeid(std::array<ag::gl::ScissorRect, 8>)}, offsetof(ag::gl::DrawStates, scissorRects), &RecordReflect<ag::gl::DrawStates>::field_thunk_scissorRects, 
	  {}  }, 
	Field { "viewports", "ag::gl::DrawStates::viewports", std::type_index{typeid(std::array<ag::gl::Viewport, 8>)}, offsetof(ag::gl::DrawStates, viewports), &RecordReflect<ag::gl::DrawStates>::field_thunk_viewports, 
	  {}  }, 
	Field { "blendStates", "ag::gl::DrawStates::blendStates", std::type_index{typeid(std::array<ag::gl::BlendState, 8>)}, offsetof(ag::gl::DrawStates, blendStates), &RecordReflect<ag::gl::DrawStates>::field_thunk_blendStates, 
	  {}  }, 
	Field { "vertexArray", "ag::gl::DrawStates::vertexArray", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DrawStates, vertexArray), &RecordReflect<ag::gl::DrawStates>::field_thunk_vertexArray, 
	  {}  }, 
	Field { "program", "ag::gl::DrawStates::program", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::DrawStates, program), &RecordReflect<ag::gl::DrawStates>::field_thunk_program, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::DrawStates>::metaobj { "ag::gl::DrawStates", 
	 RecordReflect<ag::gl::DrawStates>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::DrawStates>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::DrawStates>() {
return &RecordReflect<ag::gl::DrawStates>::metaobj;
}

///////////////////////////
// struct ag::gl::StateGroup
template <>
struct RecordReflect<ag::gl::StateGroup> {
	// attribute instances



	static const std::array<Field, 4> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_mask(void* ptr) {
		return &reinterpret_cast<ag::gl::StateGroup*>(ptr)->mask;
	}
	static void* field_thunk_drawStates(void* ptr) {
		return &reinterpret_cast<ag::gl::StateGroup*>(ptr)->drawStates;
	}
	static void* field_thunk_uniforms(void* ptr) {
		return &reinterpret_cast<ag::gl::StateGroup*>(ptr)->uniforms;
	}
	static void* field_thunk_barrierBits(void* ptr) {
		return &reinterpret_cast<ag::gl::StateGroup*>(ptr)->barrierBits;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 4> RecordReflect<ag::gl::StateGroup>::field_data  { {
	Field { "mask", "ag::gl::StateGroup::mask", std::type_index{typeid(ag::gl::StateGroupMask)}, offsetof(ag::gl::StateGroup, mask), &RecordReflect<ag::gl::StateGroup>::field_thunk_mask, 
	  {}  }, 
	Field { "drawStates", "ag::gl::StateGroup::drawStates", std::type_index{typeid(ag::gl::DrawStates)}, offsetof(ag::gl::StateGroup, drawStates), &RecordReflect<ag::gl::StateGroup>::field_thunk_drawStates, 
	  {}  }, 
	Field { "uniforms", "ag::gl::StateGroup::uniforms", std::type_index{typeid(ag::gl::Uniforms)}, offsetof(ag::gl::StateGroup, uniforms), &RecordReflect<ag::gl::StateGroup>::field_thunk_uniforms, 
	  {}  }, 
	Field { "barrierBits", "ag::gl::StateGroup::barrierBits", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::StateGroup, barrierBits), &RecordReflect<ag::gl::StateGroup>::field_thunk_barrierBits, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::StateGroup>::metaobj { "ag::gl::StateGroup", 
	 RecordReflect<ag::gl::StateGroup>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::StateGroup>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::StateGroup>() {
return &RecordReflect<ag::gl::StateGroup>::metaobj;
}

///////////////////////////
// struct ag::gl::Shader
template <>
struct RecordReflect<ag::gl::Shader> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Shader>::metaobj { "ag::gl::Shader", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Shader>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Shader>() {
return &RecordReflect<ag::gl::Shader>::metaobj;
}

///////////////////////////
// struct ag::gl::Program
template <>
struct RecordReflect<ag::gl::Program> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Program>::metaobj { "ag::gl::Program", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Program>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Program>() {
return &RecordReflect<ag::gl::Program>::metaobj;
}

///////////////////////////
// struct ag::gl::Sampler
template <>
struct RecordReflect<ag::gl::Sampler> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::Sampler>::metaobj { "ag::gl::Sampler", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::Sampler>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::Sampler>() {
return &RecordReflect<ag::gl::Sampler>::metaobj;
}

///////////////////////////
// struct ag::gl::VertexAttribute
template <>
struct RecordReflect<ag::gl::VertexAttribute> {
	// attribute instances



	static const std::array<Field, 5> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_slot(void* ptr) {
		return &reinterpret_cast<ag::gl::VertexAttribute*>(ptr)->slot;
	}
	static void* field_thunk_type(void* ptr) {
		return &reinterpret_cast<ag::gl::VertexAttribute*>(ptr)->type;
	}
	static void* field_thunk_size(void* ptr) {
		return &reinterpret_cast<ag::gl::VertexAttribute*>(ptr)->size;
	}
	static void* field_thunk_relativeOffset(void* ptr) {
		return &reinterpret_cast<ag::gl::VertexAttribute*>(ptr)->relativeOffset;
	}
	static void* field_thunk_normalized(void* ptr) {
		return &reinterpret_cast<ag::gl::VertexAttribute*>(ptr)->normalized;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 5> RecordReflect<ag::gl::VertexAttribute>::field_data  { {
	Field { "slot", "ag::gl::VertexAttribute::slot", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::VertexAttribute, slot), &RecordReflect<ag::gl::VertexAttribute>::field_thunk_slot, 
	  {}  }, 
	Field { "type", "ag::gl::VertexAttribute::type", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::VertexAttribute, type), &RecordReflect<ag::gl::VertexAttribute>::field_thunk_type, 
	  {}  }, 
	Field { "size", "ag::gl::VertexAttribute::size", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::VertexAttribute, size), &RecordReflect<ag::gl::VertexAttribute>::field_thunk_size, 
	  {}  }, 
	Field { "relativeOffset", "ag::gl::VertexAttribute::relativeOffset", std::type_index{typeid(unsigned int)}, offsetof(ag::gl::VertexAttribute, relativeOffset), &RecordReflect<ag::gl::VertexAttribute>::field_thunk_relativeOffset, 
	  {}  }, 
	Field { "normalized", "ag::gl::VertexAttribute::normalized", std::type_index{typeid(bool)}, offsetof(ag::gl::VertexAttribute, normalized), &RecordReflect<ag::gl::VertexAttribute>::field_thunk_normalized, 
	  {}  }, 
}};



const Record RecordReflect<ag::gl::VertexAttribute>::metaobj { "ag::gl::VertexAttribute", 
	 RecordReflect<ag::gl::VertexAttribute>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::VertexAttribute>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::VertexAttribute>() {
return &RecordReflect<ag::gl::VertexAttribute>::metaobj;
}

///////////////////////////
// struct ag::gl::VertexArray
template <>
struct RecordReflect<ag::gl::VertexArray> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::gl::VertexArray>::metaobj { "ag::gl::VertexArray", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::gl::VertexArray>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::gl::VertexArray>() {
return &RecordReflect<ag::gl::VertexArray>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget
template <>
struct RecordReflect<ag::RenderTarget> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::RenderTarget>::metaobj { "ag::RenderTarget", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RenderTarget>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RenderTarget>() {
return &RecordReflect<ag::RenderTarget>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::DepthRenderbuffer
template <>
struct RecordReflect<ag::RenderTarget::DepthRenderbuffer> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_fmt(void* ptr) {
		return &reinterpret_cast<ag::RenderTarget::DepthRenderbuffer*>(ptr)->fmt;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::RenderTarget::DepthRenderbuffer>::field_data  { {
	Field { "fmt", "ag::RenderTarget::DepthRenderbuffer::fmt", std::type_index{typeid(ag::ImageFormat)}, offsetof(ag::RenderTarget::DepthRenderbuffer, fmt), &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::field_thunk_fmt, 
	  {}  }, 
}};



const Record RecordReflect<ag::RenderTarget::DepthRenderbuffer>::metaobj { "ag::RenderTarget::DepthRenderbuffer", 
	 RecordReflect<ag::RenderTarget::DepthRenderbuffer>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RenderTarget::DepthRenderbuffer>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RenderTarget::DepthRenderbuffer>() {
return &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::DepthTexture
template <>
struct RecordReflect<ag::RenderTarget::DepthTexture> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_fmt(void* ptr) {
		return &reinterpret_cast<ag::RenderTarget::DepthTexture*>(ptr)->fmt;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::RenderTarget::DepthTexture>::field_data  { {
	Field { "fmt", "ag::RenderTarget::DepthTexture::fmt", std::type_index{typeid(ag::ImageFormat)}, offsetof(ag::RenderTarget::DepthTexture, fmt), &RecordReflect<ag::RenderTarget::DepthTexture>::field_thunk_fmt, 
	  {}  }, 
}};



const Record RecordReflect<ag::RenderTarget::DepthTexture>::metaobj { "ag::RenderTarget::DepthTexture", 
	 RecordReflect<ag::RenderTarget::DepthTexture>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RenderTarget::DepthTexture>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RenderTarget::DepthTexture>() {
return &RecordReflect<ag::RenderTarget::DepthTexture>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::NoDepth
template <>
struct RecordReflect<ag::RenderTarget::NoDepth> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::RenderTarget::NoDepth>::metaobj { "ag::RenderTarget::NoDepth", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RenderTarget::NoDepth>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RenderTarget::NoDepth>() {
return &RecordReflect<ag::RenderTarget::NoDepth>::metaobj;
}

///////////////////////////
// struct ag::RenderTarget::Samples
template <>
struct RecordReflect<ag::RenderTarget::Samples> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_count(void* ptr) {
		return &reinterpret_cast<ag::RenderTarget::Samples*>(ptr)->count;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::RenderTarget::Samples>::field_data  { {
	Field { "count", "ag::RenderTarget::Samples::count", std::type_index{typeid(int)}, offsetof(ag::RenderTarget::Samples, count), &RecordReflect<ag::RenderTarget::Samples>::field_thunk_count, 
	  {}  }, 
}};



const Record RecordReflect<ag::RenderTarget::Samples>::metaobj { "ag::RenderTarget::Samples", 
	 RecordReflect<ag::RenderTarget::Samples>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RenderTarget::Samples>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RenderTarget::Samples>() {
return &RecordReflect<ag::RenderTarget::Samples>::metaobj;
}

///////////////////////////
// struct ag::Rect2D
template <>
struct RecordReflect<ag::Rect2D> {
	// attribute instances



	static const std::array<Field, 2> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_position(void* ptr) {
		return &reinterpret_cast<ag::Rect2D*>(ptr)->position;
	}
	static void* field_thunk_size(void* ptr) {
		return &reinterpret_cast<ag::Rect2D*>(ptr)->size;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 2> RecordReflect<ag::Rect2D>::field_data  { {
	Field { "position", "ag::Rect2D::position", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::Rect2D, position), &RecordReflect<ag::Rect2D>::field_thunk_position, 
	  {}  }, 
	Field { "size", "ag::Rect2D::size", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::Rect2D, size), &RecordReflect<ag::Rect2D>::field_thunk_size, 
	  {}  }, 
}};



const Record RecordReflect<ag::Rect2D>::metaobj { "ag::Rect2D", 
	 RecordReflect<ag::Rect2D>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::Rect2D>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::Rect2D>() {
return &RecordReflect<ag::Rect2D>::metaobj;
}

///////////////////////////
// struct ag::RenderUtils
template <>
struct RecordReflect<ag::RenderUtils> {
	// attribute instances



	static const std::array<Field, 2> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_samplerNearest(void* ptr) {
		return &reinterpret_cast<ag::RenderUtils*>(ptr)->samplerNearest;
	}
	static void* field_thunk_samplerLinear(void* ptr) {
		return &reinterpret_cast<ag::RenderUtils*>(ptr)->samplerLinear;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 2> RecordReflect<ag::RenderUtils>::field_data  { {
	Field { "samplerNearest", "ag::RenderUtils::samplerNearest", std::type_index{typeid(ag::gl::Sampler)}, offsetof(ag::RenderUtils, samplerNearest), &RecordReflect<ag::RenderUtils>::field_thunk_samplerNearest, 
	  {}  }, 
	Field { "samplerLinear", "ag::RenderUtils::samplerLinear", std::type_index{typeid(ag::gl::Sampler)}, offsetof(ag::RenderUtils, samplerLinear), &RecordReflect<ag::RenderUtils>::field_thunk_samplerLinear, 
	  {}  }, 
}};



const Record RecordReflect<ag::RenderUtils>::metaobj { "ag::RenderUtils", 
	 RecordReflect<ag::RenderUtils>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RenderUtils>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RenderUtils>() {
return &RecordReflect<ag::RenderUtils>::metaobj;
}

///////////////////////////
// struct ag::Action
template <>
struct RecordReflect<ag::Action> {
	// attribute instances



	static const std::array<std::type_index, 1> bases_data;
	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};



const std::array<std::type_index, 1> RecordReflect<ag::Action>::bases_data { {
	std::type_index { typeid(ag::Observable<void>) } ,
} };


const Record RecordReflect<ag::Action>::metaobj { "ag::Action", 
	  gsl::span<Field>{} ,
	 RecordReflect<ag::Action>::bases_data  ,
	{},	// attribute table
	&RecordReflect<ag::Action>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::Action>() {
return &RecordReflect<ag::Action>::metaobj;
}

///////////////////////////
// struct ag::KeyAction
template <>
struct RecordReflect<ag::KeyAction> {
	// attribute instances



	static const std::array<std::type_index, 1> bases_data;
	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};



const std::array<std::type_index, 1> RecordReflect<ag::KeyAction>::bases_data { {
	std::type_index { typeid(ag::Action) } ,
} };


const Record RecordReflect<ag::KeyAction>::metaobj { "ag::KeyAction", 
	  gsl::span<Field>{} ,
	 RecordReflect<ag::KeyAction>::bases_data  ,
	{},	// attribute table
	&RecordReflect<ag::KeyAction>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::KeyAction>() {
return &RecordReflect<ag::KeyAction>::metaobj;
}

///////////////////////////
// struct ag::GamepadButtonAction
template <>
struct RecordReflect<ag::GamepadButtonAction> {
	// attribute instances



	static const std::array<std::type_index, 1> bases_data;
	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};



const std::array<std::type_index, 1> RecordReflect<ag::GamepadButtonAction>::bases_data { {
	std::type_index { typeid(ag::Action) } ,
} };


const Record RecordReflect<ag::GamepadButtonAction>::metaobj { "ag::GamepadButtonAction", 
	  gsl::span<Field>{} ,
	 RecordReflect<ag::GamepadButtonAction>::bases_data  ,
	{},	// attribute table
	&RecordReflect<ag::GamepadButtonAction>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::GamepadButtonAction>() {
return &RecordReflect<ag::GamepadButtonAction>::metaobj;
}

///////////////////////////
// struct ag::CursorEnterEvent
template <>
struct RecordReflect<ag::CursorEnterEvent> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::CursorEnterEvent>::metaobj { "ag::CursorEnterEvent", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::CursorEnterEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::CursorEnterEvent>() {
return &RecordReflect<ag::CursorEnterEvent>::metaobj;
}

///////////////////////////
// struct ag::CursorExitEvent
template <>
struct RecordReflect<ag::CursorExitEvent> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::CursorExitEvent>::metaobj { "ag::CursorExitEvent", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::CursorExitEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::CursorExitEvent>() {
return &RecordReflect<ag::CursorExitEvent>::metaobj;
}

///////////////////////////
// struct ag::WindowResizeEvent
template <>
struct RecordReflect<ag::WindowResizeEvent> {
	// attribute instances



	static const std::array<Field, 2> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_width(void* ptr) {
		return &reinterpret_cast<ag::WindowResizeEvent*>(ptr)->width;
	}
	static void* field_thunk_height(void* ptr) {
		return &reinterpret_cast<ag::WindowResizeEvent*>(ptr)->height;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 2> RecordReflect<ag::WindowResizeEvent>::field_data  { {
	Field { "width", "ag::WindowResizeEvent::width", std::type_index{typeid(int)}, offsetof(ag::WindowResizeEvent, width), &RecordReflect<ag::WindowResizeEvent>::field_thunk_width, 
	  {}  }, 
	Field { "height", "ag::WindowResizeEvent::height", std::type_index{typeid(int)}, offsetof(ag::WindowResizeEvent, height), &RecordReflect<ag::WindowResizeEvent>::field_thunk_height, 
	  {}  }, 
}};



const Record RecordReflect<ag::WindowResizeEvent>::metaobj { "ag::WindowResizeEvent", 
	 RecordReflect<ag::WindowResizeEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::WindowResizeEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::WindowResizeEvent>() {
return &RecordReflect<ag::WindowResizeEvent>::metaobj;
}

///////////////////////////
// struct ag::MouseButtonEvent
template <>
struct RecordReflect<ag::MouseButtonEvent> {
	// attribute instances



	static const std::array<Field, 2> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_button(void* ptr) {
		return &reinterpret_cast<ag::MouseButtonEvent*>(ptr)->button;
	}
	static void* field_thunk_action(void* ptr) {
		return &reinterpret_cast<ag::MouseButtonEvent*>(ptr)->action;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 2> RecordReflect<ag::MouseButtonEvent>::field_data  { {
	Field { "button", "ag::MouseButtonEvent::button", std::type_index{typeid(int)}, offsetof(ag::MouseButtonEvent, button), &RecordReflect<ag::MouseButtonEvent>::field_thunk_button, 
	  {}  }, 
	Field { "action", "ag::MouseButtonEvent::action", std::type_index{typeid(ag::ButtonState)}, offsetof(ag::MouseButtonEvent, action), &RecordReflect<ag::MouseButtonEvent>::field_thunk_action, 
	  {}  }, 
}};



const Record RecordReflect<ag::MouseButtonEvent>::metaobj { "ag::MouseButtonEvent", 
	 RecordReflect<ag::MouseButtonEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::MouseButtonEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::MouseButtonEvent>() {
return &RecordReflect<ag::MouseButtonEvent>::metaobj;
}

///////////////////////////
// struct ag::CursorEvent
template <>
struct RecordReflect<ag::CursorEvent> {
	// attribute instances



	static const std::array<Field, 4> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_x(void* ptr) {
		return &reinterpret_cast<ag::CursorEvent*>(ptr)->x;
	}
	static void* field_thunk_y(void* ptr) {
		return &reinterpret_cast<ag::CursorEvent*>(ptr)->y;
	}
	static void* field_thunk_xFloat(void* ptr) {
		return &reinterpret_cast<ag::CursorEvent*>(ptr)->xFloat;
	}
	static void* field_thunk_yFloat(void* ptr) {
		return &reinterpret_cast<ag::CursorEvent*>(ptr)->yFloat;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 4> RecordReflect<ag::CursorEvent>::field_data  { {
	Field { "x", "ag::CursorEvent::x", std::type_index{typeid(int)}, offsetof(ag::CursorEvent, x), &RecordReflect<ag::CursorEvent>::field_thunk_x, 
	  {}  }, 
	Field { "y", "ag::CursorEvent::y", std::type_index{typeid(int)}, offsetof(ag::CursorEvent, y), &RecordReflect<ag::CursorEvent>::field_thunk_y, 
	  {}  }, 
	Field { "xFloat", "ag::CursorEvent::xFloat", std::type_index{typeid(double)}, offsetof(ag::CursorEvent, xFloat), &RecordReflect<ag::CursorEvent>::field_thunk_xFloat, 
	  {}  }, 
	Field { "yFloat", "ag::CursorEvent::yFloat", std::type_index{typeid(double)}, offsetof(ag::CursorEvent, yFloat), &RecordReflect<ag::CursorEvent>::field_thunk_yFloat, 
	  {}  }, 
}};



const Record RecordReflect<ag::CursorEvent>::metaobj { "ag::CursorEvent", 
	 RecordReflect<ag::CursorEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::CursorEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::CursorEvent>() {
return &RecordReflect<ag::CursorEvent>::metaobj;
}

///////////////////////////
// struct ag::MouseMoveEvent
template <>
struct RecordReflect<ag::MouseMoveEvent> {
	// attribute instances



	static const std::array<Field, 2> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_dx(void* ptr) {
		return &reinterpret_cast<ag::MouseMoveEvent*>(ptr)->dx;
	}
	static void* field_thunk_dy(void* ptr) {
		return &reinterpret_cast<ag::MouseMoveEvent*>(ptr)->dy;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 2> RecordReflect<ag::MouseMoveEvent>::field_data  { {
	Field { "dx", "ag::MouseMoveEvent::dx", std::type_index{typeid(double)}, offsetof(ag::MouseMoveEvent, dx), &RecordReflect<ag::MouseMoveEvent>::field_thunk_dx, 
	  {}  }, 
	Field { "dy", "ag::MouseMoveEvent::dy", std::type_index{typeid(double)}, offsetof(ag::MouseMoveEvent, dy), &RecordReflect<ag::MouseMoveEvent>::field_thunk_dy, 
	  {}  }, 
}};



const Record RecordReflect<ag::MouseMoveEvent>::metaobj { "ag::MouseMoveEvent", 
	 RecordReflect<ag::MouseMoveEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::MouseMoveEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::MouseMoveEvent>() {
return &RecordReflect<ag::MouseMoveEvent>::metaobj;
}

///////////////////////////
// struct ag::MouseScrollEvent
template <>
struct RecordReflect<ag::MouseScrollEvent> {
	// attribute instances



	static const std::array<Field, 2> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_dx(void* ptr) {
		return &reinterpret_cast<ag::MouseScrollEvent*>(ptr)->dx;
	}
	static void* field_thunk_dy(void* ptr) {
		return &reinterpret_cast<ag::MouseScrollEvent*>(ptr)->dy;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 2> RecordReflect<ag::MouseScrollEvent>::field_data  { {
	Field { "dx", "ag::MouseScrollEvent::dx", std::type_index{typeid(double)}, offsetof(ag::MouseScrollEvent, dx), &RecordReflect<ag::MouseScrollEvent>::field_thunk_dx, 
	  {}  }, 
	Field { "dy", "ag::MouseScrollEvent::dy", std::type_index{typeid(double)}, offsetof(ag::MouseScrollEvent, dy), &RecordReflect<ag::MouseScrollEvent>::field_thunk_dy, 
	  {}  }, 
}};



const Record RecordReflect<ag::MouseScrollEvent>::metaobj { "ag::MouseScrollEvent", 
	 RecordReflect<ag::MouseScrollEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::MouseScrollEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::MouseScrollEvent>() {
return &RecordReflect<ag::MouseScrollEvent>::metaobj;
}

///////////////////////////
// struct ag::KeyEvent
template <>
struct RecordReflect<ag::KeyEvent> {
	// attribute instances



	static const std::array<Field, 3> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_key(void* ptr) {
		return &reinterpret_cast<ag::KeyEvent*>(ptr)->key;
	}
	static void* field_thunk_scancode(void* ptr) {
		return &reinterpret_cast<ag::KeyEvent*>(ptr)->scancode;
	}
	static void* field_thunk_action(void* ptr) {
		return &reinterpret_cast<ag::KeyEvent*>(ptr)->action;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 3> RecordReflect<ag::KeyEvent>::field_data  { {
	Field { "key", "ag::KeyEvent::key", std::type_index{typeid(int)}, offsetof(ag::KeyEvent, key), &RecordReflect<ag::KeyEvent>::field_thunk_key, 
	  {}  }, 
	Field { "scancode", "ag::KeyEvent::scancode", std::type_index{typeid(int)}, offsetof(ag::KeyEvent, scancode), &RecordReflect<ag::KeyEvent>::field_thunk_scancode, 
	  {}  }, 
	Field { "action", "ag::KeyEvent::action", std::type_index{typeid(ag::KeyState)}, offsetof(ag::KeyEvent, action), &RecordReflect<ag::KeyEvent>::field_thunk_action, 
	  {}  }, 
}};



const Record RecordReflect<ag::KeyEvent>::metaobj { "ag::KeyEvent", 
	 RecordReflect<ag::KeyEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::KeyEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::KeyEvent>() {
return &RecordReflect<ag::KeyEvent>::metaobj;
}

///////////////////////////
// struct ag::TextEvent
template <>
struct RecordReflect<ag::TextEvent> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_codepoint(void* ptr) {
		return &reinterpret_cast<ag::TextEvent*>(ptr)->codepoint;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::TextEvent>::field_data  { {
	Field { "codepoint", "ag::TextEvent::codepoint", std::type_index{typeid(char32_t)}, offsetof(ag::TextEvent, codepoint), &RecordReflect<ag::TextEvent>::field_thunk_codepoint, 
	  {}  }, 
}};



const Record RecordReflect<ag::TextEvent>::metaobj { "ag::TextEvent", 
	 RecordReflect<ag::TextEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::TextEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::TextEvent>() {
return &RecordReflect<ag::TextEvent>::metaobj;
}

///////////////////////////
// struct ag::StylusProximityEvent
template <>
struct RecordReflect<ag::StylusProximityEvent> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::StylusProximityEvent>::metaobj { "ag::StylusProximityEvent", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::StylusProximityEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::StylusProximityEvent>() {
return &RecordReflect<ag::StylusProximityEvent>::metaobj;
}

///////////////////////////
// struct ag::StylusPropertiesEvent
template <>
struct RecordReflect<ag::StylusPropertiesEvent> {
	// attribute instances



	static const std::array<Field, 4> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_x(void* ptr) {
		return &reinterpret_cast<ag::StylusPropertiesEvent*>(ptr)->x;
	}
	static void* field_thunk_y(void* ptr) {
		return &reinterpret_cast<ag::StylusPropertiesEvent*>(ptr)->y;
	}
	static void* field_thunk_pressure(void* ptr) {
		return &reinterpret_cast<ag::StylusPropertiesEvent*>(ptr)->pressure;
	}
	static void* field_thunk_tilt(void* ptr) {
		return &reinterpret_cast<ag::StylusPropertiesEvent*>(ptr)->tilt;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 4> RecordReflect<ag::StylusPropertiesEvent>::field_data  { {
	Field { "x", "ag::StylusPropertiesEvent::x", std::type_index{typeid(double)}, offsetof(ag::StylusPropertiesEvent, x), &RecordReflect<ag::StylusPropertiesEvent>::field_thunk_x, 
	  {}  }, 
	Field { "y", "ag::StylusPropertiesEvent::y", std::type_index{typeid(double)}, offsetof(ag::StylusPropertiesEvent, y), &RecordReflect<ag::StylusPropertiesEvent>::field_thunk_y, 
	  {}  }, 
	Field { "pressure", "ag::StylusPropertiesEvent::pressure", std::type_index{typeid(double)}, offsetof(ag::StylusPropertiesEvent, pressure), &RecordReflect<ag::StylusPropertiesEvent>::field_thunk_pressure, 
	  {}  }, 
	Field { "tilt", "ag::StylusPropertiesEvent::tilt", std::type_index{typeid(double)}, offsetof(ag::StylusPropertiesEvent, tilt), &RecordReflect<ag::StylusPropertiesEvent>::field_thunk_tilt, 
	  {}  }, 
}};



const Record RecordReflect<ag::StylusPropertiesEvent>::metaobj { "ag::StylusPropertiesEvent", 
	 RecordReflect<ag::StylusPropertiesEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::StylusPropertiesEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::StylusPropertiesEvent>() {
return &RecordReflect<ag::StylusPropertiesEvent>::metaobj;
}

///////////////////////////
// struct ag::PointerInfo
template <>
struct RecordReflect<ag::PointerInfo> {
	// attribute instances



	static const std::array<Field, 10> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_id(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->id;
	}
	static void* field_thunk_type(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->type;
	}
	static void* field_thunk_button(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->button;
	}
	static void* field_thunk_buttons(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->buttons;
	}
	static void* field_thunk_x(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->x;
	}
	static void* field_thunk_y(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->y;
	}
	static void* field_thunk_mask(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->mask;
	}
	static void* field_thunk_pressure(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->pressure;
	}
	static void* field_thunk_tiltX(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->tiltX;
	}
	static void* field_thunk_tiltY(void* ptr) {
		return &reinterpret_cast<ag::PointerInfo*>(ptr)->tiltY;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 10> RecordReflect<ag::PointerInfo>::field_data  { {
	Field { "id", "ag::PointerInfo::id", std::type_index{typeid(int)}, offsetof(ag::PointerInfo, id), &RecordReflect<ag::PointerInfo>::field_thunk_id, 
	  {}  }, 
	Field { "type", "ag::PointerInfo::type", std::type_index{typeid(int)}, offsetof(ag::PointerInfo, type), &RecordReflect<ag::PointerInfo>::field_thunk_type, 
	  {}  }, 
	Field { "button", "ag::PointerInfo::button", std::type_index{typeid(int)}, offsetof(ag::PointerInfo, button), &RecordReflect<ag::PointerInfo>::field_thunk_button, 
	  {}  }, 
	Field { "buttons", "ag::PointerInfo::buttons", std::type_index{typeid(unsigned int)}, offsetof(ag::PointerInfo, buttons), &RecordReflect<ag::PointerInfo>::field_thunk_buttons, 
	  {}  }, 
	Field { "x", "ag::PointerInfo::x", std::type_index{typeid(double)}, offsetof(ag::PointerInfo, x), &RecordReflect<ag::PointerInfo>::field_thunk_x, 
	  {}  }, 
	Field { "y", "ag::PointerInfo::y", std::type_index{typeid(double)}, offsetof(ag::PointerInfo, y), &RecordReflect<ag::PointerInfo>::field_thunk_y, 
	  {}  }, 
	Field { "mask", "ag::PointerInfo::mask", std::type_index{typeid(int)}, offsetof(ag::PointerInfo, mask), &RecordReflect<ag::PointerInfo>::field_thunk_mask, 
	  {}  }, 
	Field { "pressure", "ag::PointerInfo::pressure", std::type_index{typeid(double)}, offsetof(ag::PointerInfo, pressure), &RecordReflect<ag::PointerInfo>::field_thunk_pressure, 
	  {}  }, 
	Field { "tiltX", "ag::PointerInfo::tiltX", std::type_index{typeid(double)}, offsetof(ag::PointerInfo, tiltX), &RecordReflect<ag::PointerInfo>::field_thunk_tiltX, 
	  {}  }, 
	Field { "tiltY", "ag::PointerInfo::tiltY", std::type_index{typeid(double)}, offsetof(ag::PointerInfo, tiltY), &RecordReflect<ag::PointerInfo>::field_thunk_tiltY, 
	  {}  }, 
}};



const Record RecordReflect<ag::PointerInfo>::metaobj { "ag::PointerInfo", 
	 RecordReflect<ag::PointerInfo>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::PointerInfo>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::PointerInfo>() {
return &RecordReflect<ag::PointerInfo>::metaobj;
}

///////////////////////////
// struct ag::PointerEvent
template <>
struct RecordReflect<ag::PointerEvent> {
	// attribute instances



	static const std::array<Field, 1> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_info(void* ptr) {
		return &reinterpret_cast<ag::PointerEvent*>(ptr)->info;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 1> RecordReflect<ag::PointerEvent>::field_data  { {
	Field { "info", "ag::PointerEvent::info", std::type_index{typeid(ag::PointerInfo)}, offsetof(ag::PointerEvent, info), &RecordReflect<ag::PointerEvent>::field_thunk_info, 
	  {}  }, 
}};



const Record RecordReflect<ag::PointerEvent>::metaobj { "ag::PointerEvent", 
	 RecordReflect<ag::PointerEvent>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::PointerEvent>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::PointerEvent>() {
return &RecordReflect<ag::PointerEvent>::metaobj;
}

///////////////////////////
// struct ag::Window
template <>
struct RecordReflect<ag::Window> {
	// attribute instances



	static const Record metaobj;

	// field access thunks

	static void serialize(packer& p, const void* data);
};





const Record RecordReflect<ag::Window>::metaobj { "ag::Window", 
	  gsl::span<Field>{} ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::Window>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::Window>() {
return &RecordReflect<ag::Window>::metaobj;
}

///////////////////////////
// struct ag::RectTransform
template <>
struct RecordReflect<ag::RectTransform> {
	// attribute instances



	static const std::array<Field, 8> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_offsetA(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->offsetA;
	}
	static void* field_thunk_offsetB(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->offsetB;
	}
	static void* field_thunk_anchorA(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->anchorA;
	}
	static void* field_thunk_anchorB(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->anchorB;
	}
	static void* field_thunk_pivot(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->pivot;
	}
	static void* field_thunk_rotation(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->rotation;
	}
	static void* field_thunk_scale(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->scale;
	}
	static void* field_thunk_zOrder(void* ptr) {
		return &reinterpret_cast<ag::RectTransform*>(ptr)->zOrder;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 8> RecordReflect<ag::RectTransform>::field_data  { {
	Field { "offsetA", "ag::RectTransform::offsetA", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, offsetA), &RecordReflect<ag::RectTransform>::field_thunk_offsetA, 
	  {}  }, 
	Field { "offsetB", "ag::RectTransform::offsetB", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, offsetB), &RecordReflect<ag::RectTransform>::field_thunk_offsetB, 
	  {}  }, 
	Field { "anchorA", "ag::RectTransform::anchorA", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, anchorA), &RecordReflect<ag::RectTransform>::field_thunk_anchorA, 
	  {}  }, 
	Field { "anchorB", "ag::RectTransform::anchorB", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, anchorB), &RecordReflect<ag::RectTransform>::field_thunk_anchorB, 
	  {}  }, 
	Field { "pivot", "ag::RectTransform::pivot", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, pivot), &RecordReflect<ag::RectTransform>::field_thunk_pivot, 
	  {}  }, 
	Field { "rotation", "ag::RectTransform::rotation", std::type_index{typeid(glm::tvec3<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, rotation), &RecordReflect<ag::RectTransform>::field_thunk_rotation, 
	  {}  }, 
	Field { "scale", "ag::RectTransform::scale", std::type_index{typeid(glm::tvec3<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform, scale), &RecordReflect<ag::RectTransform>::field_thunk_scale, 
	  {}  }, 
	Field { "zOrder", "ag::RectTransform::zOrder", std::type_index{typeid(float)}, offsetof(ag::RectTransform, zOrder), &RecordReflect<ag::RectTransform>::field_thunk_zOrder, 
	  {}  }, 
}};



const Record RecordReflect<ag::RectTransform>::metaobj { "ag::RectTransform", 
	 RecordReflect<ag::RectTransform>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RectTransform>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RectTransform>() {
return &RecordReflect<ag::RectTransform>::metaobj;
}

///////////////////////////
// struct ag::RectTransform::Calculated
template <>
struct RecordReflect<ag::RectTransform::Calculated> {
	// attribute instances



	static const std::array<Field, 3> field_data;
	static const Record metaobj;

	// field access thunks
	static void* field_thunk_transform(void* ptr) {
		return &reinterpret_cast<ag::RectTransform::Calculated*>(ptr)->transform;
	}
	static void* field_thunk_inverseTransform(void* ptr) {
		return &reinterpret_cast<ag::RectTransform::Calculated*>(ptr)->inverseTransform;
	}
	static void* field_thunk_size(void* ptr) {
		return &reinterpret_cast<ag::RectTransform::Calculated*>(ptr)->size;
	}

	static void serialize(packer& p, const void* data);
};


const std::array<Field, 3> RecordReflect<ag::RectTransform::Calculated>::field_data  { {
	Field { "transform", "ag::RectTransform::Calculated::transform", std::type_index{typeid(glm::tmat3x3<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform::Calculated, transform), &RecordReflect<ag::RectTransform::Calculated>::field_thunk_transform, 
	  {}  }, 
	Field { "inverseTransform", "ag::RectTransform::Calculated::inverseTransform", std::type_index{typeid(glm::tmat3x3<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform::Calculated, inverseTransform), &RecordReflect<ag::RectTransform::Calculated>::field_thunk_inverseTransform, 
	  {}  }, 
	Field { "size", "ag::RectTransform::Calculated::size", std::type_index{typeid(glm::tvec2<float, glm::precision::packed_highp>)}, offsetof(ag::RectTransform::Calculated, size), &RecordReflect<ag::RectTransform::Calculated>::field_thunk_size, 
	  {}  }, 
}};



const Record RecordReflect<ag::RectTransform::Calculated>::metaobj { "ag::RectTransform::Calculated", 
	 RecordReflect<ag::RectTransform::Calculated>::field_data  ,
	  gsl::span<std::type_index>{} ,
	{},	// attribute table
	&RecordReflect<ag::RectTransform::Calculated>::serialize,	// serialize
	nullptr // deserialize
	};

template <>
const Type* typeOf<ag::RectTransform::Calculated>() {
return &RecordReflect<ag::RectTransform::Calculated>::metaobj;
}



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

template<> struct EnumReflect<ag::ImageFormat> 
{

	static const std::array<Enumerator, 17> enumerator_data;
	static const Enum metaobj;
	static void serialize(packer& p, const void* data) ;
};



const std::array<Enumerator, 17> EnumReflect<ag::ImageFormat>::enumerator_data  { {
	Enumerator { "R32G32B32A32_SFLOAT", static_cast<uint64_t>(ag::ImageFormat::R32G32B32A32_SFLOAT), 
	  {}  }, 
	Enumerator { "R16G16B16A16_SFLOAT", static_cast<uint64_t>(ag::ImageFormat::R16G16B16A16_SFLOAT), 
	  {}  }, 
	Enumerator { "R32G32_SFLOAT", static_cast<uint64_t>(ag::ImageFormat::R32G32_SFLOAT), 
	  {}  }, 
	Enumerator { "R32_SFLOAT", static_cast<uint64_t>(ag::ImageFormat::R32_SFLOAT), 
	  {}  }, 
	Enumerator { "R8_UNORM", static_cast<uint64_t>(ag::ImageFormat::R8_UNORM), 
	  {}  }, 
	Enumerator { "R8G8B8A8_UNORM", static_cast<uint64_t>(ag::ImageFormat::R8G8B8A8_UNORM), 
	  {}  }, 
	Enumerator { "R8G8B8A8_SNORM", static_cast<uint64_t>(ag::ImageFormat::R8G8B8A8_SNORM), 
	  {}  }, 
	Enumerator { "B10G11R11_UFLOAT_PACK32", static_cast<uint64_t>(ag::ImageFormat::B10G11R11_UFLOAT_PACK32), 
	  {}  }, 
	Enumerator { "D32_SFLOAT", static_cast<uint64_t>(ag::ImageFormat::D32_SFLOAT), 
	  {}  }, 
	Enumerator { "A2R10G10B10_UNORM_PACK32", static_cast<uint64_t>(ag::ImageFormat::A2R10G10B10_UNORM_PACK32), 
	  {}  }, 
	Enumerator { "A2R10G10B10_SNORM_PACK32", static_cast<uint64_t>(ag::ImageFormat::A2R10G10B10_SNORM_PACK32), 
	  {}  }, 
	Enumerator { "R8_SRGB", static_cast<uint64_t>(ag::ImageFormat::R8_SRGB), 
	  {}  }, 
	Enumerator { "R8G8_SRGB", static_cast<uint64_t>(ag::ImageFormat::R8G8_SRGB), 
	  {}  }, 
	Enumerator { "R8G8B8_SRGB", static_cast<uint64_t>(ag::ImageFormat::R8G8B8_SRGB), 
	  {}  }, 
	Enumerator { "R8G8B8A8_SRGB", static_cast<uint64_t>(ag::ImageFormat::R8G8B8A8_SRGB), 
	  {}  }, 
	Enumerator { "R32G32B32A32_UINT", static_cast<uint64_t>(ag::ImageFormat::R32G32B32A32_UINT), 
	  {}  }, 
	Enumerator { "Max", static_cast<uint64_t>(ag::ImageFormat::Max), 
	  {}  }, 
}};


const Enum EnumReflect<ag::ImageFormat>::metaobj { "ag::ImageFormat", 
	 EnumReflect<ag::ImageFormat>::enumerator_data  ,	// enumerators
	&getEnumValue<std::underlying_type_t<ag::ImageFormat>>,	// get_value
	&setEnumValue<std::underlying_type_t<ag::ImageFormat>>,	// set_value
	{},	// attribute table
	&EnumReflect<ag::ImageFormat>::serialize, // serialize
	nullptr		//deserialize
};

///////////////////////////
// enum ag::ImageFormat
template <> const char* getEnumeratorName<ag::ImageFormat, void>(ag::ImageFormat value) {
	switch (value) {
	case ag::ImageFormat::R32G32B32A32_SFLOAT: return "R32G32B32A32_SFLOAT";
	case ag::ImageFormat::R16G16B16A16_SFLOAT: return "R16G16B16A16_SFLOAT";
	case ag::ImageFormat::R32G32_SFLOAT: return "R32G32_SFLOAT";
	case ag::ImageFormat::R32_SFLOAT: return "R32_SFLOAT";
	case ag::ImageFormat::R8_UNORM: return "R8_UNORM";
	case ag::ImageFormat::R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
	case ag::ImageFormat::R8G8B8A8_SNORM: return "R8G8B8A8_SNORM";
	case ag::ImageFormat::B10G11R11_UFLOAT_PACK32: return "B10G11R11_UFLOAT_PACK32";
	case ag::ImageFormat::D32_SFLOAT: return "D32_SFLOAT";
	case ag::ImageFormat::A2R10G10B10_UNORM_PACK32: return "A2R10G10B10_UNORM_PACK32";
	case ag::ImageFormat::A2R10G10B10_SNORM_PACK32: return "A2R10G10B10_SNORM_PACK32";
	case ag::ImageFormat::R8_SRGB: return "R8_SRGB";
	case ag::ImageFormat::R8G8_SRGB: return "R8G8_SRGB";
	case ag::ImageFormat::R8G8B8_SRGB: return "R8G8B8_SRGB";
	case ag::ImageFormat::R8G8B8A8_SRGB: return "R8G8B8A8_SRGB";
	case ag::ImageFormat::R32G32B32A32_UINT: return "R32G32B32A32_UINT";
	case ag::ImageFormat::Max: return "Max";
	default: return "<invalid>";
	}
}

// generated by meta<enum_type>::getEnumValue(const char* name)
template <> ag::ImageFormat getEnumeratorValue<ag::ImageFormat, void>(const char* name) {
	std::string namestr{name};
	if (namestr == "R32G32B32A32_SFLOAT") { return ag::ImageFormat::R32G32B32A32_SFLOAT; }
	else if (namestr == "R16G16B16A16_SFLOAT") { return ag::ImageFormat::R16G16B16A16_SFLOAT; }
	else if (namestr == "R32G32_SFLOAT") { return ag::ImageFormat::R32G32_SFLOAT; }
	else if (namestr == "R32_SFLOAT") { return ag::ImageFormat::R32_SFLOAT; }
	else if (namestr == "R8_UNORM") { return ag::ImageFormat::R8_UNORM; }
	else if (namestr == "R8G8B8A8_UNORM") { return ag::ImageFormat::R8G8B8A8_UNORM; }
	else if (namestr == "R8G8B8A8_SNORM") { return ag::ImageFormat::R8G8B8A8_SNORM; }
	else if (namestr == "B10G11R11_UFLOAT_PACK32") { return ag::ImageFormat::B10G11R11_UFLOAT_PACK32; }
	else if (namestr == "D32_SFLOAT") { return ag::ImageFormat::D32_SFLOAT; }
	else if (namestr == "A2R10G10B10_UNORM_PACK32") { return ag::ImageFormat::A2R10G10B10_UNORM_PACK32; }
	else if (namestr == "A2R10G10B10_SNORM_PACK32") { return ag::ImageFormat::A2R10G10B10_SNORM_PACK32; }
	else if (namestr == "R8_SRGB") { return ag::ImageFormat::R8_SRGB; }
	else if (namestr == "R8G8_SRGB") { return ag::ImageFormat::R8G8_SRGB; }
	else if (namestr == "R8G8B8_SRGB") { return ag::ImageFormat::R8G8B8_SRGB; }
	else if (namestr == "R8G8B8A8_SRGB") { return ag::ImageFormat::R8G8B8A8_SRGB; }
	else if (namestr == "R32G32B32A32_UINT") { return ag::ImageFormat::R32G32B32A32_UINT; }
	else if (namestr == "Max") { return ag::ImageFormat::Max; }
	std::terminate();
}


template <>
const Type* typeOf<ag::ImageFormat>() {
return &EnumReflect<ag::ImageFormat>::metaobj;
}

template<> struct EnumReflect<ag::gl::StateGroupMask> 
{

	static const std::array<Enumerator, 16> enumerator_data;
	static const Enum metaobj;
	static void serialize(packer& p, const void* data) ;
};



const std::array<Enumerator, 16> EnumReflect<ag::gl::StateGroupMask>::enumerator_data  { {
	Enumerator { "Viewports", static_cast<uint64_t>(ag::gl::StateGroupMask::Viewports), 
	  {}  }, 
	Enumerator { "Framebuffer", static_cast<uint64_t>(ag::gl::StateGroupMask::Framebuffer), 
	  {}  }, 
	Enumerator { "ScissorRect", static_cast<uint64_t>(ag::gl::StateGroupMask::ScissorRect), 
	  {}  }, 
	Enumerator { "BlendStates", static_cast<uint64_t>(ag::gl::StateGroupMask::BlendStates), 
	  {}  }, 
	Enumerator { "RasterizerState", static_cast<uint64_t>(ag::gl::StateGroupMask::RasterizerState), 
	  {}  }, 
	Enumerator { "DepthStencilState", static_cast<uint64_t>(ag::gl::StateGroupMask::DepthStencilState), 
	  {}  }, 
	Enumerator { "Textures", static_cast<uint64_t>(ag::gl::StateGroupMask::Textures), 
	  {}  }, 
	Enumerator { "Samplers", static_cast<uint64_t>(ag::gl::StateGroupMask::Samplers), 
	  {}  }, 
	Enumerator { "UniformBuffers", static_cast<uint64_t>(ag::gl::StateGroupMask::UniformBuffers), 
	  {}  }, 
	Enumerator { "ShaderStorageBuffers", static_cast<uint64_t>(ag::gl::StateGroupMask::ShaderStorageBuffers), 
	  {}  }, 
	Enumerator { "VertexArray", static_cast<uint64_t>(ag::gl::StateGroupMask::VertexArray), 
	  {}  }, 
	Enumerator { "Program", static_cast<uint64_t>(ag::gl::StateGroupMask::Program), 
	  {}  }, 
	Enumerator { "VertexBuffers", static_cast<uint64_t>(ag::gl::StateGroupMask::VertexBuffers), 
	  {}  }, 
	Enumerator { "IndexBuffer", static_cast<uint64_t>(ag::gl::StateGroupMask::IndexBuffer), 
	  {}  }, 
	Enumerator { "Images", static_cast<uint64_t>(ag::gl::StateGroupMask::Images), 
	  {}  }, 
	Enumerator { "All", static_cast<uint64_t>(ag::gl::StateGroupMask::All), 
	  {}  }, 
}};


const Enum EnumReflect<ag::gl::StateGroupMask>::metaobj { "ag::gl::StateGroupMask", 
	 EnumReflect<ag::gl::StateGroupMask>::enumerator_data  ,	// enumerators
	&getEnumValue<std::underlying_type_t<ag::gl::StateGroupMask>>,	// get_value
	&setEnumValue<std::underlying_type_t<ag::gl::StateGroupMask>>,	// set_value
	{},	// attribute table
	&EnumReflect<ag::gl::StateGroupMask>::serialize, // serialize
	nullptr		//deserialize
};

///////////////////////////
// enum ag::gl::StateGroupMask
template <> const char* getEnumeratorName<ag::gl::StateGroupMask, void>(ag::gl::StateGroupMask value) {
	switch (value) {
	case ag::gl::StateGroupMask::Viewports: return "Viewports";
	case ag::gl::StateGroupMask::Framebuffer: return "Framebuffer";
	case ag::gl::StateGroupMask::ScissorRect: return "ScissorRect";
	case ag::gl::StateGroupMask::BlendStates: return "BlendStates";
	case ag::gl::StateGroupMask::RasterizerState: return "RasterizerState";
	case ag::gl::StateGroupMask::DepthStencilState: return "DepthStencilState";
	case ag::gl::StateGroupMask::Textures: return "Textures";
	case ag::gl::StateGroupMask::Samplers: return "Samplers";
	case ag::gl::StateGroupMask::UniformBuffers: return "UniformBuffers";
	case ag::gl::StateGroupMask::ShaderStorageBuffers: return "ShaderStorageBuffers";
	case ag::gl::StateGroupMask::VertexArray: return "VertexArray";
	case ag::gl::StateGroupMask::Program: return "Program";
	case ag::gl::StateGroupMask::VertexBuffers: return "VertexBuffers";
	case ag::gl::StateGroupMask::IndexBuffer: return "IndexBuffer";
	case ag::gl::StateGroupMask::Images: return "Images";
	case ag::gl::StateGroupMask::All: return "All";
	default: return "<invalid>";
	}
}

// generated by meta<enum_type>::getEnumValue(const char* name)
template <> ag::gl::StateGroupMask getEnumeratorValue<ag::gl::StateGroupMask, void>(const char* name) {
	std::string namestr{name};
	if (namestr == "Viewports") { return ag::gl::StateGroupMask::Viewports; }
	else if (namestr == "Framebuffer") { return ag::gl::StateGroupMask::Framebuffer; }
	else if (namestr == "ScissorRect") { return ag::gl::StateGroupMask::ScissorRect; }
	else if (namestr == "BlendStates") { return ag::gl::StateGroupMask::BlendStates; }
	else if (namestr == "RasterizerState") { return ag::gl::StateGroupMask::RasterizerState; }
	else if (namestr == "DepthStencilState") { return ag::gl::StateGroupMask::DepthStencilState; }
	else if (namestr == "Textures") { return ag::gl::StateGroupMask::Textures; }
	else if (namestr == "Samplers") { return ag::gl::StateGroupMask::Samplers; }
	else if (namestr == "UniformBuffers") { return ag::gl::StateGroupMask::UniformBuffers; }
	else if (namestr == "ShaderStorageBuffers") { return ag::gl::StateGroupMask::ShaderStorageBuffers; }
	else if (namestr == "VertexArray") { return ag::gl::StateGroupMask::VertexArray; }
	else if (namestr == "Program") { return ag::gl::StateGroupMask::Program; }
	else if (namestr == "VertexBuffers") { return ag::gl::StateGroupMask::VertexBuffers; }
	else if (namestr == "IndexBuffer") { return ag::gl::StateGroupMask::IndexBuffer; }
	else if (namestr == "Images") { return ag::gl::StateGroupMask::Images; }
	else if (namestr == "All") { return ag::gl::StateGroupMask::All; }
	std::terminate();
}


template <>
const Type* typeOf<ag::gl::StateGroupMask>() {
return &EnumReflect<ag::gl::StateGroupMask>::metaobj;
}

template<> struct EnumReflect<ag::EventType> 
{

	static const std::array<Enumerator, 16> enumerator_data;
	static const Enum metaobj;
	static void serialize(packer& p, const void* data) ;
};



const std::array<Enumerator, 16> EnumReflect<ag::EventType>::enumerator_data  { {
	Enumerator { "MouseButton", static_cast<uint64_t>(ag::EventType::MouseButton), 
	  {}  }, 
	Enumerator { "MouseMove", static_cast<uint64_t>(ag::EventType::MouseMove), 
	  {}  }, 
	Enumerator { "Cursor", static_cast<uint64_t>(ag::EventType::Cursor), 
	  {}  }, 
	Enumerator { "CursorEnter", static_cast<uint64_t>(ag::EventType::CursorEnter), 
	  {}  }, 
	Enumerator { "CursorExit", static_cast<uint64_t>(ag::EventType::CursorExit), 
	  {}  }, 
	Enumerator { "MouseScroll", static_cast<uint64_t>(ag::EventType::MouseScroll), 
	  {}  }, 
	Enumerator { "Key", static_cast<uint64_t>(ag::EventType::Key), 
	  {}  }, 
	Enumerator { "Text", static_cast<uint64_t>(ag::EventType::Text), 
	  {}  }, 
	Enumerator { "StylusProximity", static_cast<uint64_t>(ag::EventType::StylusProximity), 
	  {}  }, 
	Enumerator { "StylusProperties", static_cast<uint64_t>(ag::EventType::StylusProperties), 
	  {}  }, 
	Enumerator { "WindowResize", static_cast<uint64_t>(ag::EventType::WindowResize), 
	  {}  }, 
	Enumerator { "PointerEnter", static_cast<uint64_t>(ag::EventType::PointerEnter), 
	  {}  }, 
	Enumerator { "PointerDown", static_cast<uint64_t>(ag::EventType::PointerDown), 
	  {}  }, 
	Enumerator { "PointerUp", static_cast<uint64_t>(ag::EventType::PointerUp), 
	  {}  }, 
	Enumerator { "PointerMove", static_cast<uint64_t>(ag::EventType::PointerMove), 
	  {}  }, 
	Enumerator { "PointerLeave", static_cast<uint64_t>(ag::EventType::PointerLeave), 
	  {}  }, 
}};


const Enum EnumReflect<ag::EventType>::metaobj { "ag::EventType", 
	 EnumReflect<ag::EventType>::enumerator_data  ,	// enumerators
	&getEnumValue<std::underlying_type_t<ag::EventType>>,	// get_value
	&setEnumValue<std::underlying_type_t<ag::EventType>>,	// set_value
	{},	// attribute table
	&EnumReflect<ag::EventType>::serialize, // serialize
	nullptr		//deserialize
};

///////////////////////////
// enum ag::EventType
template <> const char* getEnumeratorName<ag::EventType, void>(ag::EventType value) {
	switch (value) {
	case ag::EventType::MouseButton: return "MouseButton";
	case ag::EventType::MouseMove: return "MouseMove";
	case ag::EventType::Cursor: return "Cursor";
	case ag::EventType::CursorEnter: return "CursorEnter";
	case ag::EventType::CursorExit: return "CursorExit";
	case ag::EventType::MouseScroll: return "MouseScroll";
	case ag::EventType::Key: return "Key";
	case ag::EventType::Text: return "Text";
	case ag::EventType::StylusProximity: return "StylusProximity";
	case ag::EventType::StylusProperties: return "StylusProperties";
	case ag::EventType::WindowResize: return "WindowResize";
	case ag::EventType::PointerEnter: return "PointerEnter";
	case ag::EventType::PointerDown: return "PointerDown";
	case ag::EventType::PointerUp: return "PointerUp";
	case ag::EventType::PointerMove: return "PointerMove";
	case ag::EventType::PointerLeave: return "PointerLeave";
	default: return "<invalid>";
	}
}

// generated by meta<enum_type>::getEnumValue(const char* name)
template <> ag::EventType getEnumeratorValue<ag::EventType, void>(const char* name) {
	std::string namestr{name};
	if (namestr == "MouseButton") { return ag::EventType::MouseButton; }
	else if (namestr == "MouseMove") { return ag::EventType::MouseMove; }
	else if (namestr == "Cursor") { return ag::EventType::Cursor; }
	else if (namestr == "CursorEnter") { return ag::EventType::CursorEnter; }
	else if (namestr == "CursorExit") { return ag::EventType::CursorExit; }
	else if (namestr == "MouseScroll") { return ag::EventType::MouseScroll; }
	else if (namestr == "Key") { return ag::EventType::Key; }
	else if (namestr == "Text") { return ag::EventType::Text; }
	else if (namestr == "StylusProximity") { return ag::EventType::StylusProximity; }
	else if (namestr == "StylusProperties") { return ag::EventType::StylusProperties; }
	else if (namestr == "WindowResize") { return ag::EventType::WindowResize; }
	else if (namestr == "PointerEnter") { return ag::EventType::PointerEnter; }
	else if (namestr == "PointerDown") { return ag::EventType::PointerDown; }
	else if (namestr == "PointerUp") { return ag::EventType::PointerUp; }
	else if (namestr == "PointerMove") { return ag::EventType::PointerMove; }
	else if (namestr == "PointerLeave") { return ag::EventType::PointerLeave; }
	std::terminate();
}


template <>
const Type* typeOf<ag::EventType>() {
return &EnumReflect<ag::EventType>::metaobj;
}



/////////////////////////////////////////////////////////////////////
// SERIALIZATION


void RecordReflect<ag::ResourceManager>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::ResourceManager*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::ResourcePool>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::ResourcePool*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::ImageFormatInfo>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::ImageFormatInfo*>(data);
p.pack_map(3);
p.pack("name");
serialize_impl(p, obj->name);
p.pack("size");
serialize_impl(p, obj->size);
p.pack("numChannels");
serialize_impl(p, obj->numChannels);
*/
}



void RecordReflect<ag::gl::Texture>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Texture*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::Texture::MipMaps>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Texture::MipMaps*>(data);
p.pack_map(1);
p.pack("count");
serialize_impl(p, obj->count);
*/
}



void RecordReflect<ag::gl::Texture::Samples>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Texture::Samples*>(data);
p.pack_map(1);
p.pack("count");
serialize_impl(p, obj->count);
*/
}



void RecordReflect<ag::gl::Buffer>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Buffer*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::Renderbuffer>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Renderbuffer*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::Renderbuffer::Samples>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Renderbuffer::Samples*>(data);
p.pack_map(1);
p.pack("count");
serialize_impl(p, obj->count);
*/
}



void RecordReflect<ag::gl::Framebuffer>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Framebuffer*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::BlendState>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::BlendState*>(data);
p.pack_map(7);
p.pack("enabled");
serialize_impl(p, obj->enabled);
p.pack("modeRGB");
serialize_impl(p, obj->modeRGB);
p.pack("modeAlpha");
serialize_impl(p, obj->modeAlpha);
p.pack("funcSrcRGB");
serialize_impl(p, obj->funcSrcRGB);
p.pack("funcDstRGB");
serialize_impl(p, obj->funcDstRGB);
p.pack("funcSrcAlpha");
serialize_impl(p, obj->funcSrcAlpha);
p.pack("funcDstAlpha");
serialize_impl(p, obj->funcDstAlpha);
*/
}



void RecordReflect<ag::gl::DepthStencilState>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::DepthStencilState*>(data);
p.pack_map(11);
p.pack("depthTestEnable");
serialize_impl(p, obj->depthTestEnable);
p.pack("depthWriteEnable");
serialize_impl(p, obj->depthWriteEnable);
p.pack("stencilEnable");
serialize_impl(p, obj->stencilEnable);
p.pack("depthTestFunc");
serialize_impl(p, obj->depthTestFunc);
p.pack("stencilFace");
serialize_impl(p, obj->stencilFace);
p.pack("stencilFunc");
serialize_impl(p, obj->stencilFunc);
p.pack("stencilRef");
serialize_impl(p, obj->stencilRef);
p.pack("stencilMask");
serialize_impl(p, obj->stencilMask);
p.pack("stencilOpSfail");
serialize_impl(p, obj->stencilOpSfail);
p.pack("stencilOpDPFail");
serialize_impl(p, obj->stencilOpDPFail);
p.pack("stencilOpDPPass");
serialize_impl(p, obj->stencilOpDPPass);
*/
}



void RecordReflect<ag::gl::RasterizerState>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::RasterizerState*>(data);
p.pack_map(7);
p.pack("fillMode");
serialize_impl(p, obj->fillMode);
p.pack("cullMode");
serialize_impl(p, obj->cullMode);
p.pack("frontFace");
serialize_impl(p, obj->frontFace);
p.pack("depthBias");
serialize_impl(p, obj->depthBias);
p.pack("slopeScaledDepthBias");
serialize_impl(p, obj->slopeScaledDepthBias);
p.pack("depthClipEnable");
serialize_impl(p, obj->depthClipEnable);
p.pack("scissorEnable");
serialize_impl(p, obj->scissorEnable);
*/
}



void RecordReflect<ag::gl::ScissorRect>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::ScissorRect*>(data);
p.pack_map(4);
p.pack("x");
serialize_impl(p, obj->x);
p.pack("y");
serialize_impl(p, obj->y);
p.pack("w");
serialize_impl(p, obj->w);
p.pack("h");
serialize_impl(p, obj->h);
*/
}



void RecordReflect<ag::gl::Viewport>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Viewport*>(data);
p.pack_map(4);
p.pack("x");
serialize_impl(p, obj->x);
p.pack("y");
serialize_impl(p, obj->y);
p.pack("w");
serialize_impl(p, obj->w);
p.pack("h");
serialize_impl(p, obj->h);
*/
}



void RecordReflect<ag::gl::Uniforms>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Uniforms*>(data);
p.pack_map(14);
p.pack("textures");
serialize_impl(p, obj->textures);
p.pack("samplers");
serialize_impl(p, obj->samplers);
p.pack("images");
serialize_impl(p, obj->images);
p.pack("uniformBuffers");
serialize_impl(p, obj->uniformBuffers);
p.pack("uniformBufferSizes");
serialize_impl(p, obj->uniformBufferSizes);
p.pack("uniformBufferOffsets");
serialize_impl(p, obj->uniformBufferOffsets);
p.pack("shaderStorageBuffers");
serialize_impl(p, obj->shaderStorageBuffers);
p.pack("shaderStorageBufferSizes");
serialize_impl(p, obj->shaderStorageBufferSizes);
p.pack("shaderStorageBufferOffsets");
serialize_impl(p, obj->shaderStorageBufferOffsets);
p.pack("vertexBuffers");
serialize_impl(p, obj->vertexBuffers);
p.pack("vertexBufferOffsets");
serialize_impl(p, obj->vertexBufferOffsets);
p.pack("vertexBufferStrides");
serialize_impl(p, obj->vertexBufferStrides);
p.pack("indexBuffer");
serialize_impl(p, obj->indexBuffer);
p.pack("indexBufferType");
serialize_impl(p, obj->indexBufferType);
*/
}



void RecordReflect<ag::gl::DrawStates>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::DrawStates*>(data);
p.pack_map(7);
p.pack("depthStencilState");
serialize_impl(p, obj->depthStencilState);
p.pack("rasterizerState");
serialize_impl(p, obj->rasterizerState);
p.pack("scissorRects");
serialize_impl(p, obj->scissorRects);
p.pack("viewports");
serialize_impl(p, obj->viewports);
p.pack("blendStates");
serialize_impl(p, obj->blendStates);
p.pack("vertexArray");
serialize_impl(p, obj->vertexArray);
p.pack("program");
serialize_impl(p, obj->program);
*/
}



void RecordReflect<ag::gl::StateGroup>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::StateGroup*>(data);
p.pack_map(4);
p.pack("mask");
serialize_impl(p, obj->mask);
p.pack("drawStates");
serialize_impl(p, obj->drawStates);
p.pack("uniforms");
serialize_impl(p, obj->uniforms);
p.pack("barrierBits");
serialize_impl(p, obj->barrierBits);
*/
}



void RecordReflect<ag::gl::Shader>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Shader*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::Program>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Program*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::Sampler>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::Sampler*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::gl::VertexAttribute>::serialize(packer& p, const void* data) 
{
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



void RecordReflect<ag::gl::VertexArray>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::gl::VertexArray*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::RenderTarget>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RenderTarget*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::RenderTarget::DepthRenderbuffer>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RenderTarget::DepthRenderbuffer*>(data);
p.pack_map(1);
p.pack("fmt");
serialize_impl(p, obj->fmt);
*/
}



void RecordReflect<ag::RenderTarget::DepthTexture>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RenderTarget::DepthTexture*>(data);
p.pack_map(1);
p.pack("fmt");
serialize_impl(p, obj->fmt);
*/
}



void RecordReflect<ag::RenderTarget::NoDepth>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RenderTarget::NoDepth*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::RenderTarget::Samples>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RenderTarget::Samples*>(data);
p.pack_map(1);
p.pack("count");
serialize_impl(p, obj->count);
*/
}



void RecordReflect<ag::Rect2D>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::Rect2D*>(data);
p.pack_map(2);
p.pack("position");
serialize_impl(p, obj->position);
p.pack("size");
serialize_impl(p, obj->size);
*/
}



void RecordReflect<ag::RenderUtils>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RenderUtils*>(data);
p.pack_map(2);
p.pack("samplerNearest");
serialize_impl(p, obj->samplerNearest);
p.pack("samplerLinear");
serialize_impl(p, obj->samplerLinear);
*/
}



void RecordReflect<ag::Action>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::Action*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::KeyAction>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::KeyAction*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::GamepadButtonAction>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::GamepadButtonAction*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::CursorEnterEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::CursorEnterEvent*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::CursorExitEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::CursorExitEvent*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::WindowResizeEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::WindowResizeEvent*>(data);
p.pack_map(2);
p.pack("width");
serialize_impl(p, obj->width);
p.pack("height");
serialize_impl(p, obj->height);
*/
}



void RecordReflect<ag::MouseButtonEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::MouseButtonEvent*>(data);
p.pack_map(2);
p.pack("button");
serialize_impl(p, obj->button);
p.pack("action");
serialize_impl(p, obj->action);
*/
}



void RecordReflect<ag::CursorEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::CursorEvent*>(data);
p.pack_map(4);
p.pack("x");
serialize_impl(p, obj->x);
p.pack("y");
serialize_impl(p, obj->y);
p.pack("xFloat");
serialize_impl(p, obj->xFloat);
p.pack("yFloat");
serialize_impl(p, obj->yFloat);
*/
}



void RecordReflect<ag::MouseMoveEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::MouseMoveEvent*>(data);
p.pack_map(2);
p.pack("dx");
serialize_impl(p, obj->dx);
p.pack("dy");
serialize_impl(p, obj->dy);
*/
}



void RecordReflect<ag::MouseScrollEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::MouseScrollEvent*>(data);
p.pack_map(2);
p.pack("dx");
serialize_impl(p, obj->dx);
p.pack("dy");
serialize_impl(p, obj->dy);
*/
}



void RecordReflect<ag::KeyEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::KeyEvent*>(data);
p.pack_map(3);
p.pack("key");
serialize_impl(p, obj->key);
p.pack("scancode");
serialize_impl(p, obj->scancode);
p.pack("action");
serialize_impl(p, obj->action);
*/
}



void RecordReflect<ag::TextEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::TextEvent*>(data);
p.pack_map(1);
p.pack("codepoint");
serialize_impl(p, obj->codepoint);
*/
}



void RecordReflect<ag::StylusProximityEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::StylusProximityEvent*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::StylusPropertiesEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::StylusPropertiesEvent*>(data);
p.pack_map(4);
p.pack("x");
serialize_impl(p, obj->x);
p.pack("y");
serialize_impl(p, obj->y);
p.pack("pressure");
serialize_impl(p, obj->pressure);
p.pack("tilt");
serialize_impl(p, obj->tilt);
*/
}



void RecordReflect<ag::PointerInfo>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::PointerInfo*>(data);
p.pack_map(10);
p.pack("id");
serialize_impl(p, obj->id);
p.pack("type");
serialize_impl(p, obj->type);
p.pack("button");
serialize_impl(p, obj->button);
p.pack("buttons");
serialize_impl(p, obj->buttons);
p.pack("x");
serialize_impl(p, obj->x);
p.pack("y");
serialize_impl(p, obj->y);
p.pack("mask");
serialize_impl(p, obj->mask);
p.pack("pressure");
serialize_impl(p, obj->pressure);
p.pack("tiltX");
serialize_impl(p, obj->tiltX);
p.pack("tiltY");
serialize_impl(p, obj->tiltY);
*/
}



void RecordReflect<ag::PointerEvent>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::PointerEvent*>(data);
p.pack_map(1);
p.pack("info");
serialize_impl(p, obj->info);
*/
}



void RecordReflect<ag::Window>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::Window*>(data);
p.pack_map(0);
*/
}



void RecordReflect<ag::RectTransform>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RectTransform*>(data);
p.pack_map(8);
p.pack("offsetA");
serialize_impl(p, obj->offsetA);
p.pack("offsetB");
serialize_impl(p, obj->offsetB);
p.pack("anchorA");
serialize_impl(p, obj->anchorA);
p.pack("anchorB");
serialize_impl(p, obj->anchorB);
p.pack("pivot");
serialize_impl(p, obj->pivot);
p.pack("rotation");
serialize_impl(p, obj->rotation);
p.pack("scale");
serialize_impl(p, obj->scale);
p.pack("zOrder");
serialize_impl(p, obj->zOrder);
*/
}



void RecordReflect<ag::RectTransform::Calculated>::serialize(packer& p, const void* data) 
{
/*auto obj = reinterpret_cast<const ag::RectTransform::Calculated*>(data);
p.pack_map(3);
p.pack("transform");
serialize_impl(p, obj->transform);
p.pack("inverseTransform");
serialize_impl(p, obj->inverseTransform);
p.pack("size");
serialize_impl(p, obj->size);
*/
}







void EnumReflect<ag::ImageFormat>::serialize(packer& p, const void* data) 
{
/*auto v = *reinterpret_cast<const ag::ImageFormat*>(data);
p.pack(getEnumeratorName(v));*/
}



void EnumReflect<ag::gl::StateGroupMask>::serialize(packer& p, const void* data) 
{
/*auto v = *reinterpret_cast<const ag::gl::StateGroupMask*>(data);
p.pack(getEnumeratorName(v));*/
}



void EnumReflect<ag::EventType>::serialize(packer& p, const void* data) 
{
/*auto v = *reinterpret_cast<const ag::EventType*>(data);
p.pack(getEnumeratorName(v));*/
}



/////////////////////////////////////////////////////////////////////
// TYPEINDEX MAP
static std::unordered_map<std::type_index, const Type*> typeindex_to_metaobject;
static bool typeindex_to_metaobject_initialized = false;

const Type *typeOf(std::type_index ti)
{
	if (!typeindex_to_metaobject_initialized) {
		using kv_pair = decltype(typeindex_to_metaobject)::value_type;
		typeindex_to_metaobject.insert({
			kv_pair{typeid(ag::ImageFormat), &EnumReflect<ag::ImageFormat>::metaobj},
			kv_pair{typeid(ag::gl::StateGroupMask), &EnumReflect<ag::gl::StateGroupMask>::metaobj},
			kv_pair{typeid(ag::EventType), &EnumReflect<ag::EventType>::metaobj},
			kv_pair{typeid(ag::ResourceManager), &RecordReflect<ag::ResourceManager>::metaobj},
			kv_pair{typeid(ag::ResourcePool), &RecordReflect<ag::ResourcePool>::metaobj},
			kv_pair{typeid(ag::ImageFormatInfo), &RecordReflect<ag::ImageFormatInfo>::metaobj},
			kv_pair{typeid(ag::gl::Texture), &RecordReflect<ag::gl::Texture>::metaobj},
			kv_pair{typeid(ag::gl::Texture::MipMaps), &RecordReflect<ag::gl::Texture::MipMaps>::metaobj},
			kv_pair{typeid(ag::gl::Texture::Samples), &RecordReflect<ag::gl::Texture::Samples>::metaobj},
			kv_pair{typeid(ag::gl::Buffer), &RecordReflect<ag::gl::Buffer>::metaobj},
			kv_pair{typeid(ag::gl::Renderbuffer), &RecordReflect<ag::gl::Renderbuffer>::metaobj},
			kv_pair{typeid(ag::gl::Renderbuffer::Samples), &RecordReflect<ag::gl::Renderbuffer::Samples>::metaobj},
			kv_pair{typeid(ag::gl::Framebuffer), &RecordReflect<ag::gl::Framebuffer>::metaobj},
			kv_pair{typeid(ag::gl::BlendState), &RecordReflect<ag::gl::BlendState>::metaobj},
			kv_pair{typeid(ag::gl::DepthStencilState), &RecordReflect<ag::gl::DepthStencilState>::metaobj},
			kv_pair{typeid(ag::gl::RasterizerState), &RecordReflect<ag::gl::RasterizerState>::metaobj},
			kv_pair{typeid(ag::gl::ScissorRect), &RecordReflect<ag::gl::ScissorRect>::metaobj},
			kv_pair{typeid(ag::gl::Viewport), &RecordReflect<ag::gl::Viewport>::metaobj},
			kv_pair{typeid(ag::gl::Uniforms), &RecordReflect<ag::gl::Uniforms>::metaobj},
			kv_pair{typeid(ag::gl::DrawStates), &RecordReflect<ag::gl::DrawStates>::metaobj},
			kv_pair{typeid(ag::gl::StateGroup), &RecordReflect<ag::gl::StateGroup>::metaobj},
			kv_pair{typeid(ag::gl::Shader), &RecordReflect<ag::gl::Shader>::metaobj},
			kv_pair{typeid(ag::gl::Program), &RecordReflect<ag::gl::Program>::metaobj},
			kv_pair{typeid(ag::gl::Sampler), &RecordReflect<ag::gl::Sampler>::metaobj},
			kv_pair{typeid(ag::gl::VertexAttribute), &RecordReflect<ag::gl::VertexAttribute>::metaobj},
			kv_pair{typeid(ag::gl::VertexArray), &RecordReflect<ag::gl::VertexArray>::metaobj},
			kv_pair{typeid(ag::RenderTarget), &RecordReflect<ag::RenderTarget>::metaobj},
			kv_pair{typeid(ag::RenderTarget::DepthRenderbuffer), &RecordReflect<ag::RenderTarget::DepthRenderbuffer>::metaobj},
			kv_pair{typeid(ag::RenderTarget::DepthTexture), &RecordReflect<ag::RenderTarget::DepthTexture>::metaobj},
			kv_pair{typeid(ag::RenderTarget::NoDepth), &RecordReflect<ag::RenderTarget::NoDepth>::metaobj},
			kv_pair{typeid(ag::RenderTarget::Samples), &RecordReflect<ag::RenderTarget::Samples>::metaobj},
			kv_pair{typeid(ag::Rect2D), &RecordReflect<ag::Rect2D>::metaobj},
			kv_pair{typeid(ag::RenderUtils), &RecordReflect<ag::RenderUtils>::metaobj},
			kv_pair{typeid(ag::Action), &RecordReflect<ag::Action>::metaobj},
			kv_pair{typeid(ag::KeyAction), &RecordReflect<ag::KeyAction>::metaobj},
			kv_pair{typeid(ag::GamepadButtonAction), &RecordReflect<ag::GamepadButtonAction>::metaobj},
			kv_pair{typeid(ag::CursorEnterEvent), &RecordReflect<ag::CursorEnterEvent>::metaobj},
			kv_pair{typeid(ag::CursorExitEvent), &RecordReflect<ag::CursorExitEvent>::metaobj},
			kv_pair{typeid(ag::WindowResizeEvent), &RecordReflect<ag::WindowResizeEvent>::metaobj},
			kv_pair{typeid(ag::MouseButtonEvent), &RecordReflect<ag::MouseButtonEvent>::metaobj},
			kv_pair{typeid(ag::CursorEvent), &RecordReflect<ag::CursorEvent>::metaobj},
			kv_pair{typeid(ag::MouseMoveEvent), &RecordReflect<ag::MouseMoveEvent>::metaobj},
			kv_pair{typeid(ag::MouseScrollEvent), &RecordReflect<ag::MouseScrollEvent>::metaobj},
			kv_pair{typeid(ag::KeyEvent), &RecordReflect<ag::KeyEvent>::metaobj},
			kv_pair{typeid(ag::TextEvent), &RecordReflect<ag::TextEvent>::metaobj},
			kv_pair{typeid(ag::StylusProximityEvent), &RecordReflect<ag::StylusProximityEvent>::metaobj},
			kv_pair{typeid(ag::StylusPropertiesEvent), &RecordReflect<ag::StylusPropertiesEvent>::metaobj},
			kv_pair{typeid(ag::PointerInfo), &RecordReflect<ag::PointerInfo>::metaobj},
			kv_pair{typeid(ag::PointerEvent), &RecordReflect<ag::PointerEvent>::metaobj},
			kv_pair{typeid(ag::Window), &RecordReflect<ag::Window>::metaobj},
			kv_pair{typeid(ag::RectTransform), &RecordReflect<ag::RectTransform>::metaobj},
			kv_pair{typeid(ag::RectTransform::Calculated), &RecordReflect<ag::RectTransform::Calculated>::metaobj},
		});
		typeindex_to_metaobject_initialized = true;
	}
	auto it = typeindex_to_metaobject.find(ti);
	if (it != typeindex_to_metaobject.end()) {
		return it->second;
	}
	return nullptr;
}


/*template <> struct meta_attr<ag::ResourceManager> {
	// attribute instances for ag::ResourceManager
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
/*template <> struct meta_attr<ag::ImageFormatInfo> {
	// attribute instances for ag::ImageFormatInfo
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Texture> {
	// attribute instances for ag::gl::Texture
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Texture::MipMaps> {
	// attribute instances for ag::gl::Texture::MipMaps
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Texture::Samples> {
	// attribute instances for ag::gl::Texture::Samples
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Buffer> {
	// attribute instances for ag::gl::Buffer
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Renderbuffer> {
	// attribute instances for ag::gl::Renderbuffer
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Renderbuffer::Samples> {
	// attribute instances for ag::gl::Renderbuffer::Samples
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Framebuffer> {
	// attribute instances for ag::gl::Framebuffer
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::BlendState> {
	// attribute instances for ag::gl::BlendState
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::DepthStencilState> {
	// attribute instances for ag::gl::DepthStencilState
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::RasterizerState> {
	// attribute instances for ag::gl::RasterizerState
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::ScissorRect> {
	// attribute instances for ag::gl::ScissorRect
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Viewport> {
	// attribute instances for ag::gl::Viewport
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Uniforms> {
	// attribute instances for ag::gl::Uniforms
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::DrawStates> {
	// attribute instances for ag::gl::DrawStates
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::StateGroup> {
	// attribute instances for ag::gl::StateGroup
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Shader> {
	// attribute instances for ag::gl::Shader
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Program> {
	// attribute instances for ag::gl::Program
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::Sampler> {
	// attribute instances for ag::gl::Sampler
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
/*template <> struct meta_attr<ag::gl::VertexArray> {
	// attribute instances for ag::gl::VertexArray
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
/*template <> struct meta_attr<ag::Rect2D> {
	// attribute instances for ag::Rect2D
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::RenderUtils> {
	// attribute instances for ag::RenderUtils
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
/*template <> struct meta_attr<ag::CursorEnterEvent> {
	// attribute instances for ag::CursorEnterEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::CursorExitEvent> {
	// attribute instances for ag::CursorExitEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::WindowResizeEvent> {
	// attribute instances for ag::WindowResizeEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::MouseButtonEvent> {
	// attribute instances for ag::MouseButtonEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::CursorEvent> {
	// attribute instances for ag::CursorEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::MouseMoveEvent> {
	// attribute instances for ag::MouseMoveEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::MouseScrollEvent> {
	// attribute instances for ag::MouseScrollEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::KeyEvent> {
	// attribute instances for ag::KeyEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::TextEvent> {
	// attribute instances for ag::TextEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::StylusProximityEvent> {
	// attribute instances for ag::StylusProximityEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::StylusPropertiesEvent> {
	// attribute instances for ag::StylusPropertiesEvent
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::PointerInfo> {
	// attribute instances for ag::PointerInfo
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::PointerEvent> {
	// attribute instances for ag::PointerEvent
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
/*template <> struct meta_attr<ag::RectTransform> {
	// attribute instances for ag::RectTransform
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::RectTransform::Calculated> {
	// attribute instances for ag::RectTransform::Calculated
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::ImageFormat> {
	// attribute instances for ag::ImageFormat
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::gl::StateGroupMask> {
	// attribute instances for ag::gl::StateGroupMask
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/
/*template <> struct meta_attr<ag::EventType> {
	// attribute instances for ag::EventType
	// attribute table
	const std::array<meta_attr_entry, 0>& table() {
		static std::array<meta_attr_entry, 0> data_ = { {
		} };
		return data_;
	}
};*/

}
}