#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <algorithm>
#include <memory>
#include <gsl.h>

#include "buffer.hpp"
#include "error.hpp"
#include "fence.hpp"
#include "gl_types.hpp"
#include "pipeline.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "upload_buffer.hpp"

namespace ag {

struct DeviceConfig {
  unsigned init_fb_width;
  unsigned init_fb_height;
  unsigned max_frames_in_flight;
};

inline FenceValue getFrameExpirationDate(unsigned frame_id) {
  // Frame N expires when the fence has reached the value N+1
  return frame_id + 1;
}

////////////////////////// PrimitiveType
enum class PrimitiveType { Points, Lines, Triangles };

////////////////////////// IndexType
enum class IndexType { UShort, UInt };

struct ClearColor {
  float rgba[4];
};

struct ClearColorInt {
  uint32_t rgba[4];
};

struct GLPixelFormat {
  GLenum internalFormat;
  GLenum externalFormat;
  GLenum type;
  int numComponents;
};

inline GLPixelFormat pixelFormatToGL(PixelFormat format) {
  switch (format) {
  case PixelFormat::Uint8:
    return GLPixelFormat{gl::R8UI, gl::RED_INTEGER, gl::UNSIGNED_BYTE, 1};
  case PixelFormat::Uint8x2:
    return GLPixelFormat{gl::RG8UI, gl::RG_INTEGER, gl::UNSIGNED_BYTE, 2};
  case PixelFormat::Uint8x3:
    return GLPixelFormat{gl::RGB8UI, gl::RGB_INTEGER, gl::UNSIGNED_BYTE, 3};
  case PixelFormat::Uint8x4:
    return GLPixelFormat{gl::RGBA8UI, gl::RGBA_INTEGER, gl::UNSIGNED_BYTE, 4};
  case PixelFormat::Unorm8:
    return GLPixelFormat{gl::R8, gl::RED, gl::UNSIGNED_BYTE, 1};
  case PixelFormat::Unorm8x2:
    return GLPixelFormat{gl::RG8, gl::RG, gl::UNSIGNED_BYTE, 2};
  case PixelFormat::Unorm8x3:
    return GLPixelFormat{gl::RGB8, gl::RGB, gl::UNSIGNED_BYTE, 3};
  case PixelFormat::Unorm8x4:
    return GLPixelFormat{gl::RGBA8, gl::RGBA, gl::UNSIGNED_BYTE, 4};
  case PixelFormat::Float:
    return GLPixelFormat{gl::R32F, gl::RED, gl::FLOAT, 1};
  case PixelFormat::Float2:
    return GLPixelFormat{gl::RG32F, gl::RG, gl::FLOAT, 2};
  case PixelFormat::Float3:
    return GLPixelFormat{gl::RGB32F, gl::RGB, gl::FLOAT, 3};
  case PixelFormat::Float4:
    return GLPixelFormat{gl::RGBA32F, gl::RGBA, gl::FLOAT, 4};
  case PixelFormat::Uint32:
    return GLPixelFormat{gl::R32UI, gl::RED_INTEGER, gl::UNSIGNED_INT, 1};
  case PixelFormat::Depth32:
    return GLPixelFormat{gl::DEPTH_COMPONENT32F, 0, 0, 1}; // TODO
  case PixelFormat::Depth24_Stencil8:
    return GLPixelFormat{gl::DEPTH24_STENCIL8, 0, 0, 1};
  case PixelFormat::Snorm10x3_1x2: // Oops, does not exist in OpenGL
    return GLPixelFormat{gl::RGB10_A2, gl::RGBA, gl::UNSIGNED_INT_10_10_10_2,
                         4};
  case PixelFormat::Unorm10x3_1x2:
    return GLPixelFormat{gl::RGB10_A2, gl::RGBA, gl::UNSIGNED_INT_10_10_10_2,
                         4};
  default:
    failWith("TODO");
  }
}

class Device {
public:
  Device(const DeviceConfig& config_);
  ~Device() {}

  Surface<float, RGBA8>& getOutputSurface() { return screen_surface; }

  ///////////////////// createTexture1D
  template <typename Pixel> Texture1D<Pixel> createTexture1D(unsigned width, const char* label = "") {
    static_assert(PixelTypeTraits<Pixel>::kIsPixelType,
                  "Unsupported pixel type");
    auto fmt = PixelTypeTraits<Pixel>::kFormat;
    auto obj = createTexture1D(fmt, width);
    gl::ObjectLabel(gl::TEXTURE, obj, std::strlen(label), label);
    return Texture1D<Pixel>{obj, fmt, width};
  }

  ///////////////////// createTexture2D
  template <typename Pixel>
  Texture2D<Pixel> createTexture2D(unsigned width, unsigned height, const char* label = "") {
    static_assert(PixelTypeTraits<Pixel>::kIsPixelType,
                  "Unsupported pixel type");
    auto fmt = PixelTypeTraits<Pixel>::kFormat;
    auto obj = createTexture2D(fmt, width, height);
    gl::ObjectLabel(gl::TEXTURE, obj, std::strlen(label), label);
    return Texture2D<Pixel>{obj, fmt, width,
                            height};
  }

  ///////////////////// createTexture2DBase
  TextureBase createTexture2D(unsigned width, unsigned height, ag::PixelFormat fmt, const char* label = "") {
    auto obj = createTexture2D(fmt, width, height);
    gl::ObjectLabel(gl::TEXTURE, obj, std::strlen(label), label);
    return TextureBase{obj, fmt, texture_dimension::tex_2d, width, height};
  }

  ///////////////////// createTexture3D
  template <typename Pixel>
  Texture3D<Pixel> createTexture3D(unsigned width, unsigned height,
                                   unsigned depth, const char* label = "") {
    static_assert(PixelTypeTraits<Pixel>::kIsPixelType,
                  "Unsupported pixel type");
    auto fmt = PixelTypeTraits<Pixel>::kFormat;
    auto obj = createTexture3D(fmt, width, height, depth);
    gl::ObjectLabel(gl::TEXTURE, obj, std::strlen(label), label);
    return Texture3D<Pixel>{obj, fmt,
                            width, height, depth};
  }

  ///////////////////// createSampler
  Sampler createSampler(const SamplerInfo& info) {
    return Sampler{createSamplerGL(info), info};
  }

  RawBuffer createRawBuffer(std::size_t size, const void* data,
                            BufferUsage usage = BufferUsage::Default) {
    return RawBuffer{size, usage, createBuffer(size, data, usage)};
  }

  template <typename T>
  RawBuffer createRawBuffer(const T& data,
                            BufferUsage usage = BufferUsage::Default) {
    return RawBuffer{sizeof(T), usage, createBuffer(sizeof(T), &data, usage)};
  }

  ///////////////////// createBuffer(T)
  template <typename T> Buffer<T> createBuffer(const T& data) {
    return Buffer<T>(
        createBuffer(sizeof(T), &data, BufferUsage::Default));
  }

  ///////////////////// createBuffer(span)
  /*template <typename T>
  Buffer<T[]> createBufferFromSpan(gsl::span<const T> data) {
    return Buffer<T[]>(BufferUsage::Default, data.size(),
                       createBuffer(data.size_bytes(), data.data(),
                                            BufferUsage::Default));
  }*/

  ///////////////////// createBuffer(fixed-size array)
  template <typename T, size_t N> Buffer<T[]> createBuffer(const T (&data)[N]) {
    return Buffer<T[]>{BufferUsage::Default, N,
                       createBuffer(N * sizeof(T), data, BufferUsage::Default)};
  }
  
  ///////////////////// createBuffer(fixed-size array)
  template <typename T> Buffer<T[]> createBuffer(const T* data, std::size_t size) {
	  return Buffer<T[]>{BufferUsage::Default, size,
		  createBuffer(size * sizeof(T), data, BufferUsage::Default)};
  }

  ///////////////////// Upload heap management
  template <typename T>
  RawBufferSlice upload_frame_data(const T& value,
                                        size_t alignment = kUniformBufferOffsetAlignment) {
    RawBufferSlice out_slice;
    if (!default_upload_buffer->uploadRaw(&value, sizeof(T), alignment,
                                          getFrameExpirationDate(frame_id),
                                          out_slice)) {
      failWith("Upload buffer is full");
    }
    return std::move(out_slice);
  }

  template <typename T>
  RawBufferSlice upload_frame_data(gsl::span<T> span,
                                        size_t alignment = kUniformBufferOffsetAlignment) {
    RawBufferSlice out_slice;
    if (!default_upload_buffer->uploadRaw(
            span.data(), span.size_bytes(), alignment,
            getFrameExpirationDate(frame_id), out_slice)) {
      failWith("Upload buffer is full");
    }
    return std::move(out_slice);
  }

  ///////////////////// end-of-frame cleanup
  void endFrame() {
    // sync on frame N-(max-in-flight)
    frame_id++;
    frame_fence.signal(frame_id);
    if (frame_id >= config.max_frames_in_flight) {
      frame_fence.wait(getFrameExpirationDate(frame_id - config.max_frames_in_flight));
      default_upload_buffer->reclaim(
          getFrameExpirationDate(frame_id - config.max_frames_in_flight));
    }
  }

  ///////////////////// debug
  void pushDebugGroup(const char* message);
  void popDebugGroup();

  ///////////////////// pipeline
  GraphicsPipeline createGraphicsPipeline(const GraphicsPipelineDesc& desc);
  ComputePipeline createComputePipeline(const ComputePipelineDesc& desc);

  ///////////////////// Timeout values for fence wait operations
  ///////////////////// Alignment constraints for buffers
  static constexpr std::size_t kBufferAlignment = 64;
  static constexpr std::size_t kUniformBufferOffsetAlignment = 256;
  ///////////////////// arbitrary binding limits
  static constexpr unsigned kMaxTextureUnits = 16;
  static constexpr unsigned kMaxImageUnits = 8;
  static constexpr unsigned kMaxVertexBufferSlots = 8;
  static constexpr unsigned kMaxUniformBufferSlots = 8;
  static constexpr unsigned kMaxShaderStorageBufferSlots = 8;

  ///////////////////// Resources: Textures
  GLuint createSamplerGL(const SamplerInfo& info);
  GLuint createTexture1D(PixelFormat format, unsigned width);
  GLuint createTexture2D(PixelFormat format, unsigned width, unsigned height);
  GLuint createTexture3D(PixelFormat format, unsigned width, unsigned height,
                         unsigned depth);

  ///////////////////// Resources: Buffers
  GLuint createBuffer(std::size_t size, const void* data, BufferUsage usage);
  void* mapBuffer(GLuint handle, BufferUsage usage, size_t offset, size_t size);

  ///////////////////// Resources: fences
  Fence createFence(uint64_t initialValue);

  GLuint createComputeProgram(const ComputePipelineDesc& desc);
  GLuint createProgramFromShaderPipeline(const GraphicsPipelineDesc& desc);

  ///////////////////// internals
  void bindTexture(unsigned slot, GLuint obj);
  void bindRWTexture(unsigned slot, GLuint obj);
  void bindSampler(unsigned slot, GLuint obj);
  void bindVertexBuffer(unsigned slot, GLuint obj, size_t offset, size_t size,
                        unsigned stride);
  void bindIndexBuffer(GLuint obj, size_t offset, size_t size, IndexType type);
  void bindUniformBuffer(unsigned slot, GLuint obj, size_t offset, size_t size);
  void bindGraphicsPipeline(const GraphicsPipeline& pp);
  void clearColor(GLuint framebuffer_obj, const ag::ClearColor& color);
  void bindComputePipeline(const ComputePipeline& pp);
  void bindSurface(GLuint obj);
  void bindRenderTexture(unsigned slot, GLuint obj);
  void bindViewport(int x, int y, int width, int height);
  void bindDepthRenderTexture(GLuint obj);
  void clearDepth(GLuint framebuffer_obj, float depth);
  void draw(PrimitiveType primitiveType, unsigned first, unsigned count);
  void drawIndexed(PrimitiveType primitiveType, unsigned first, unsigned count,
                   unsigned baseVertex);
  void dispatchCompute(unsigned threadGroupCountX, unsigned threadGroupCountY,
                       unsigned threadGroupCountZ);
  void swapBuffers();
  void bindFramebufferObject(GLuint framebuffer_obj);
  void unbindState();
  void bindState();

private:
  struct BindState {
    std::array<GLuint, kMaxVertexBufferSlots> vertexBuffers;
    std::array<GLintptr, kMaxVertexBufferSlots> vertexBufferOffsets;
    std::array<GLsizei, kMaxVertexBufferSlots> vertexBufferStrides;
    bool vertexBuffersUpdated = false;
    std::array<GLuint, kMaxTextureUnits> textures;
    bool textureUpdated = false;
    std::array<GLuint, kMaxTextureUnits> samplers;
    bool samplersUpdated = false;
    std::array<GLuint, kMaxImageUnits> images;
    bool imagesUpdated = false;
    std::array<GLuint, kMaxUniformBufferSlots> uniformBuffers;
    std::array<GLsizeiptr, kMaxUniformBufferSlots> uniformBufferSizes;
    std::array<GLintptr, kMaxUniformBufferSlots> uniformBufferOffsets;
    bool uniformBuffersUpdated = false;
    std::array<GLuint, kMaxShaderStorageBufferSlots> shaderStorageBuffers;
    bool shaderStorageBuffersUpdated = false;
    GLuint indexBuffer;
    GLenum indexBufferType;

    void reset()
    {
        indexBuffer = 0;
        vertexBuffers.fill(0);
        vertexBufferOffsets.fill(0);
        vertexBufferStrides.fill(0);
        images.fill(0);
        textures.fill(0);
        samplers.fill(0);
        shaderStorageBuffers.fill(0);
        uniformBuffers.fill(0);
        uniformBufferSizes.fill(0);
        uniformBufferOffsets.fill(0);
        vertexBuffersUpdated = false;
        textureUpdated = false;
        samplersUpdated = false;
        imagesUpdated = false;
        uniformBuffersUpdated = false;
        shaderStorageBuffersUpdated = false;
    }
  };

  // last bound FBO
  DeviceConfig config;
  GLuint last_framebuffer_obj;
  GLuint render_to_texture_fbo;
  BindState bind_state;
  Fence frame_fence;
  unsigned frame_id;
  Surface<float, RGBA8> screen_surface;
  std::unique_ptr<UploadBuffer> default_upload_buffer;
};
}

#endif // !DEVICE_HPP
