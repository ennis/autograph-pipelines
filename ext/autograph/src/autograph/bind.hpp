#ifndef BIND_HPP
#define BIND_HPP

#include <tuple>

#include "buffer.hpp"
#include "device.hpp"
#include "texture.hpp"
#include "utils.hpp"

namespace ag {

////////////////////////// BindContext
struct BindContext {
  unsigned renderTargetBindingIndex = 0;
  unsigned textureBindingIndex = 0;
  unsigned samplerBindingIndex = 0;
  unsigned vertexBufferBindingIndex = 0;
  unsigned uniformBufferBindingIndex = 0;
  unsigned RWTextureBindingIndex = 0;
};

////////////////////////// Binder: vertex buffer
template <typename VertexTy> struct VertexBuffer_ {
  const Buffer<VertexTy[]>& buf;
};

template <typename VertexTy>
VertexBuffer_<VertexTy> VertexBuffer(const Buffer<VertexTy[]>& buf) {
  return VertexBuffer_<VertexTy>{buf};
}

////////////////////////// Binder: index buffer
template <typename T> struct IndexBuffer_ { const Buffer<T[]>& buf; };

template <typename T> IndexBuffer_<T> IndexBuffer(const Buffer<T[]>& buf) {
  static_assert(std::is_same<T, unsigned short>::value ||
                    std::is_same<T, unsigned int>::value,
                "Unsupported index type");
  return IndexBuffer_<T>{buf};
}

////////////////////////// Binder: vertex array (vertices on the CPU)
template <typename VertexTy> struct VertexArray_ {
  gsl::span<const VertexTy> data;
};

template <typename VertexTy>
VertexArray_<VertexTy> VertexArray(gsl::span<const VertexTy> data) {
  return VertexArray_<VertexTy>{data};
}

////////////////////////// Binder: texture unit
template <typename TextureTy> struct TextureUnit_ {
  TextureUnit_(unsigned unit_, const TextureTy& tex_, const Sampler& sampler_)
      : unit(unit_), tex(tex_), sampler(sampler_) {}

  unsigned unit;
  const TextureTy& tex;
  const Sampler& sampler;
};

template <typename T>
TextureUnit_<Texture1D<T>> TextureUnit(unsigned unit_, const Texture1D<T>& tex_,
                                       const Sampler& sampler_) {
  return TextureUnit_<Texture1D<T>>(unit_, tex_, sampler_);
}

template <typename T>
TextureUnit_<Texture2D<T>> TextureUnit(unsigned unit_, const Texture2D<T>& tex_,
                                       const Sampler& sampler_) {
  return TextureUnit_<Texture2D<T>>(unit_, tex_, sampler_);
}

template <typename T>
TextureUnit_<Texture3D<T>> TextureUnit(unsigned unit_, const Texture3D<T>& tex_,
                                       const Sampler& sampler_) {
  return TextureUnit_<Texture3D<T>>(unit_, tex_, sampler_);
}

inline
TextureUnit_<TextureBase> TextureUnitTypeless(unsigned unit_, const TextureBase& tex_,
                                       const Sampler& sampler_) {
  return TextureUnit_<TextureBase>(unit_, tex_, sampler_);
}

////////////////////////// Binder: RWTexture unit
template <typename TextureTy> struct RWTextureUnit_ {
  RWTextureUnit_(unsigned unit_, TextureTy& tex_) : unit(unit_), tex(tex_) {}

  unsigned unit;
  TextureTy& tex;
};

template <typename T>
RWTextureUnit_<Texture1D<T>> RWTextureUnit(unsigned unit_, Texture1D<T>& tex_) {
  return RWTextureUnit_<Texture1D<T>>(unit_, tex_);
}

template <typename T>
RWTextureUnit_<Texture2D<T>> RWTextureUnit(unsigned unit_, Texture2D<T>& tex_) {
  return RWTextureUnit_<Texture2D<T>>(unit_, tex_);
}

template <typename T>
RWTextureUnit_<Texture3D<T>> RWTextureUnit(unsigned unit_, Texture3D<T>& tex_) {
  return RWTextureUnit_<Texture3D<T>>(unit_, tex_);
}

inline RWTextureUnit_<TextureBase> RWTextureUnitTypeless(unsigned unit_, TextureBase& tex_) {
  return RWTextureUnit_<TextureBase>(unit_, tex_);
}

////////////////////////// Binder: uniform slot
template <typename ResTy // Buffer, BufferSlice or just a value
          >
struct Uniform_ {
  Uniform_(unsigned slot_, const ResTy& buf_) : slot(slot_), buf(buf_) {}

  unsigned slot;
  const ResTy& buf;
};

template <typename ResTy>
Uniform_<ResTy> Uniform(unsigned slot_, const ResTy& buf_) {
  return Uniform_<ResTy>(slot_, buf_);
}

////////////////////////// Binder: SurfaceRT
template <typename T>
void bindRTImpl(Device& device, BindContext& context, Texture2D<T>& resource) {
	device.bindViewport(0, 0, resource.width, resource.height);
  device.bindRenderTexture(context.renderTargetBindingIndex++,
                           resource.obj.get());
}

template <typename T, typename... Rest>
void bindRTImpl(Device& device, BindContext& context, Texture2D<T>& resource,
                Rest&&... rest) {
  bindRTOne(device, context, std::forward<T>(resource));
  bindRTImpl(device, context, std::forward<Rest>(rest)...);
}

template <typename TDepth, typename... TPixels> struct SurfaceRT_ {
  // TODO check for a valid depth format
  Texture2D<TDepth>& depth_target;
  std::tuple<Texture2D<TPixels>&...> color_targets;

  void bind(Device& device, BindContext& context) {
    device.bindDepthRenderTexture(depth_target.obj.get());
    call(bindRTImpl,
         std::tuple_cat(std::forward_as_tuple(device, context), color_targets));
  }
};

// specialization for depth-less rendering
template <typename... TPixels> struct SurfaceRT_<void, TPixels...> {
  std::tuple<Texture2D<TPixels>&...> color_targets;

  void bind(Device& device, BindContext& context) {
    call(bindRTImpl,
         std::tuple_cat(std::forward_as_tuple(device, context), color_targets));
  }
};

template <typename TDepth, typename... TPixels>
SurfaceRT_<TDepth, TPixels...> SurfaceRT(Texture2D<TDepth>& tex_depth,
                                         Texture2D<TPixels>&... tex_color) {
  return SurfaceRT_<TDepth, TPixels...>{tex_depth,
                                        std::forward_as_tuple(tex_color...)};
}

template <typename... TPixels>
SurfaceRT_<void, TPixels...>
SurfaceRTNoDepth(Texture2D<TPixels>&... tex_color) {
  return SurfaceRT_<void, TPixels...>{std::forward_as_tuple(tex_color...)};
}

////////////////////////// bindOne<T> template declaration
//template <typename T>
//void bindOne(Device& device, BindContext& context, const T& value);

////////////////////////// Bind<VertexBuffer_>
template <typename T>
void bindOne(Device& device, BindContext& context,
             const VertexBuffer_<T>& vbuf) {
  device.bindVertexBuffer(context.vertexBufferBindingIndex++,
                          vbuf.buf.obj.get(), 0, vbuf.buf.byteSize, sizeof(T));
}

////////////////////////// Bind<VertexArray_>
template <typename TVertex>
void bindOne(Device& device, BindContext& context,
             const VertexArray_<TVertex>& vbuf) {
  auto slice = device.upload_frame_data(vbuf.data);
  device.bindVertexBuffer(context.vertexBufferBindingIndex++, slice.handle,
                          slice.offset, slice.byteSize, sizeof(TVertex));
}

////////////////////////// Bind<IndexBuffer_<T> >
template <typename T>
void bindOne(Device& device, BindContext& context,
             const IndexBuffer_<T>& ibuf) {
  IndexType indexType;
  if (std::is_same<T, unsigned short>::value)
    indexType = IndexType::UShort;
  if (std::is_same<T, unsigned int>::value)
    indexType = IndexType::UInt;
  device.bindIndexBuffer(ibuf.buf.obj.get(), 0, ibuf.buf.byteSize, indexType);
}

////////////////////////// Bind<TextureBase>
inline void bindOne(Device& device, BindContext& context,
             const TextureBase& tex) {
  device.bindTexture(context.textureBindingIndex++, tex.obj.get());
}

template <typename Pixel>
void bindOne(Device& device, BindContext& context,
             const Texture1D<Pixel>& tex) {
  device.bindTexture(context.textureBindingIndex++, tex.obj.get());
}
template <typename Pixel>
void bindOne(Device& device, BindContext& context,
             const Texture2D<Pixel>& tex) {
  device.bindTexture(context.textureBindingIndex++, tex.obj.get());
}
template <typename Pixel>
void bindOne(Device& device, BindContext& context,
             const Texture3D<Pixel>& tex) {
  device.bindTexture(context.textureBindingIndex++, tex.obj.get());
}

////////////////////////// Bind<Sampler>
inline void bindOne(Device& device, BindContext& context,
                    const Sampler& sampler) {
  device.bindSampler(context.samplerBindingIndex++, sampler.obj.get());
}

////////////////////////// Bind<TextureUnit<>>
template <typename TextureTy>
void bindOne(Device& device, BindContext& context,
             const TextureUnit_<TextureTy>& tex_unit) {
  context.textureBindingIndex = tex_unit.unit;
  context.samplerBindingIndex = tex_unit.unit;
  bindOne(device, context, tex_unit.sampler);
  bindOne(device, context, tex_unit.tex);
}

////////////////////////// Bind<RWTextureUnit<>>
template <typename TextureTy>
void bindOne(Device& device, BindContext& context,
             const RWTextureUnit_<TextureTy>& tex_unit) {
  context.RWTextureBindingIndex = tex_unit.unit;
  device.bindRWTexture(context.RWTextureBindingIndex++,
                         tex_unit.tex.obj.get());
}

////////////////////////// Bind<RawBufferSlice>
inline void bindOne(Device& device, BindContext& context,
             const RawBufferSlice& buf_slice) {
  device.bindUniformBuffer(context.uniformBufferBindingIndex++,
                           buf_slice.obj, buf_slice.offset,
                           buf_slice.byteSize);
}

////////////////////////// Bind<T>
template <typename T>
void bindOne(Device& device, BindContext& context, const T& value) {
  // allocate a temporary uniform buffer from the default upload buffer
  auto slice = device.upload_frame_data(value);
  bindOne(device, context, slice);
}

////////////////////////// bindImpl<T>: recursive binding of draw resources
template <typename T>
void bindImpl(Device& device, BindContext& context, T&& resource) {
  bindOne(device, context, std::forward<T>(resource));
}

template <typename T, typename... Rest>
void bindImpl(Device& device, BindContext& context, T&& resource,
              Rest&&... rest) {
  bindOne(device, context, std::forward<T>(resource));
  bindImpl(device, context, std::forward<Rest>(rest)...);
}

////////////////////////// Bind render targets
////////////////////////// BindRT<Texture2D>
template <typename T>
void bindRenderTarget(Device& device, BindContext& context, Texture2D<T>& tex) {
    device.bindViewport(0, 0, tex.width, tex.height);
  device.bindRenderTexture(context.renderTargetBindingIndex++, tex.obj.get());
}

////////////////////////// BindRT<Surface>
template <typename Depth, typename... Colors>
void bindRenderTarget(Device& device, BindContext& context,
                      Surface<Depth, Colors...>& surface) {
	device.bindViewport(0, 0, surface.width, surface.height);
  device.bindSurface(surface.obj);
}

// http://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element
namespace detail {
template <int... Is> struct seq {};

template <int N, int... Is> struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <int... Is> struct gen_seq<0, Is...> : seq<Is...> {};
}

namespace detail {
template <typename T, typename F, int... Is>
void for_each(T&& t, F f, seq<Is...>) {
  auto l = {(f(std::get<Is>(t)), 0)...};
}
}

template <typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> const& t, F f) {
  detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

////////////////////////// BindRT<SurfaceRT>
template <typename Depth, typename... Colors>
void bindRenderTarget(Device& device, BindContext& context,
                      const SurfaceRT_<Depth, Colors...>& surfaceRT) {
    device.bindViewport(0, 0, surfaceRT.depth_target.width, surfaceRT.depth_target.height);
  for_each_in_tuple(surfaceRT.color_targets, [&](auto&& v) {
    device.bindRenderTexture(context.renderTargetBindingIndex++, v.obj.get());
  });
  device.bindDepthRenderTexture(surfaceRT.depth_target.obj.get());
}

////////////////////////// BindRT<SurfaceRT> depth-less
template <typename... Colors>
void bindRenderTarget(Device& device, BindContext& context,
                      const SurfaceRT_<void, Colors...>& surfaceRT) {
    device.bindViewport(0, 0, surfaceRT.color_targets.template get<0>().width, surfaceRT.color_targets.template get<0>().height);
  for_each_in_tuple(surfaceRT.color_targets, [&](auto&& v) {
    device.bindRenderTexture(context.renderTargetBindingIndex++, v.obj.get());
  });
}

}

#endif // !BIND_HPP
