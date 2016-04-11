#ifndef DRAW_HPP
#define DRAW_HPP

#include <tuple>

#include "bind.hpp"
#include "optional.hpp"
#include "rect.hpp"

namespace ag {

////////////////////////// Drawables
template <typename TIndexSource, typename... TVertexSource>
struct IndexedMesh_ {
  // Primitive type
  PrimitiveType primitiveType;
  // vertex buffers
  std::tuple<TVertexSource...> vertex_sources;
  // index buffer
  TIndexSource index_source;
};

////////////////////////// Draw command: DrawArrays
struct DrawArrays_ {
  PrimitiveType primitiveType;
  GLuint buffer;
  size_t offset;
  size_t size;
  uint32_t stride;
  uint32_t count;

  void draw(Device& device, BindContext& context) {
    device.bindVertexBuffer(context.vertexBufferBindingIndex++, buffer, offset,
                            size, stride);
    device.draw(primitiveType, 0, count);
  }
};

////////////////////////// Draw command: DrawArrays (reduced form)
struct DrawArrays0_ {
  PrimitiveType primitiveType;
  uint32_t first;
  uint32_t count;

  void draw(Device& device, BindContext& context) {
    device.draw(primitiveType, 0, count);
  }
};

inline DrawArrays0_ DrawArrays(PrimitiveType primitiveType, uint32_t first,
                               uint32_t count) {
  return DrawArrays0_{primitiveType, first, count};
}

////////////////////////// Draw command: DrawIndexed (reduced form)
struct DrawIndexed0_ {
  PrimitiveType primitiveType;
  uint32_t first;
  uint32_t count;
  uint32_t baseVertex;

  void draw(Device& device, BindContext& context) {
    device.drawIndexed(primitiveType, 0, count, baseVertex);
  }
};

inline DrawIndexed0_ DrawIndexed(PrimitiveType primitiveType, uint32_t first,
                                 uint32_t count, uint32_t baseVertex) {
  return DrawIndexed0_{primitiveType, first, count, baseVertex};
}

// Immediate version (put vertex data in the default upload buffer)
template <typename TVertex> struct DrawArraysImmediate_ {
  PrimitiveType primitiveType;
  gsl::span<TVertex> vertices;

  void draw(Device& device, BindContext& context) {
    auto slice = device.upload_frame_data(vertices);
    device.bindVertexBuffer(context.vertexBufferBindingIndex++,
                                    slice.obj, slice.offset, slice.byteSize,
                                    sizeof(TVertex));
    device.draw(primitiveType, 0, (uint32_t)vertices.size());
  }
};

template <typename TVertex>
DrawArrays_ DrawArrays(PrimitiveType primitiveType,
                       const Buffer<TVertex[]>& vertex_buffer) {
  return DrawArrays_{primitiveType,
                     vertex_buffer.obj.get(),
                     0,
                     vertex_buffer.byteSize,
                     (uint32_t)sizeof(TVertex),
                     (uint32_t)vertex_buffer.size()};
}

template <typename TVertex>
DrawArraysImmediate_<TVertex> DrawArrays(PrimitiveType primitiveType,
                                         gsl::span<TVertex> vertices) {
  return DrawArraysImmediate_<TVertex>{primitiveType, vertices};
}

////////////////////////// ag::draw (no resources)
template <typename TSurface, typename Drawable>
void draw(Device& device, TSurface&& surface,
          const GraphicsPipeline& graphicsPipeline, Drawable&& drawable) {
  BindContext context;
  bindRenderTarget(device, context, surface);
  device.bindGraphicsPipeline(graphicsPipeline);
  drawable.draw(device, context);
}

////////////////////////// ag::draw
template <typename TSurface, typename Drawable, typename... TShaderResources>
void draw(Device& device, TSurface&& surface,
          const GraphicsPipeline& graphicsPipeline, Drawable&& drawable,
          TShaderResources&&... resources) {
  BindContext context;
  bindImpl(device, context, resources...);
  bindRenderTarget(device, context, surface);
  device.bindGraphicsPipeline(graphicsPipeline);
  drawable.draw(device, context);
}

////////////////////////// ag::clear(Surface)
template <typename Depth, typename... Pixels>
void clear(Device& device, Surface<Depth, Pixels...>& surface,
           const ClearColor& color) {
  gl::ClearNamedFramebufferfv(surface.obj, gl::COLOR, 0, color.rgba);
}

////////////////////////// ag::clear(Surface)
template <typename Depth, typename... Pixels>
void clearDepth(Device& device, Surface<Depth, Pixels...>& surface,
                float depth) {
  gl::ClearNamedFramebufferfv(surface.obj, gl::DEPTH, 0, &depth);
}

////////////////////////// ag::clearDepth(Texture2D)
template <typename Depth>
void clearDepth(Device& device, Texture2D<Depth>& tex, float depth) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::DEPTH_COMPONENT, gl::FLOAT, &depth);
}

////////////////////////// ag::clear(Texture1D)
template <typename Pixel>
void clear(Device& device, Texture1D<Pixel>& tex, const ClearColor& color) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::RGBA, gl::FLOAT, color.rgba);
}

////////////////////////// ag::clear(Texture2D)
template <typename Pixel>
void clear(Device& device, Texture2D<Pixel>& tex, const ClearColor& color) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::RGBA, gl::FLOAT, color.rgba);
}

////////////////////////// ag::clear(Texture3D)
template <typename Pixel>
void clear(Device& device, Texture3D<Pixel>& tex, const ClearColor& color) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::RGBA, gl::FLOAT, color.rgba);
}

////////////////////////// ag::clear(Texture2D<Integer>)
template <typename IPixel>
void clearInteger(Device& device, Texture1D<IPixel>& tex,
                  const ClearColorInt& color) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::RGBA_INTEGER, gl::UNSIGNED_INT,
                    color.rgba);
}

template <typename IPixel>
void clearInteger(Device& device, Texture2D<IPixel>& tex,
                  const ClearColorInt& color) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::RGBA_INTEGER, gl::UNSIGNED_INT,
                    color.rgba);
}

template <typename IPixel>
void clearInteger(Device& device, Texture3D<IPixel>& tex,
                  const ClearColorInt& color) {
  gl::ClearTexImage(tex.obj.get(), 0, gl::RGBA_INTEGER, gl::UNSIGNED_INT,
                    color.rgba);
}
}

#endif // !DRAW_HPP
