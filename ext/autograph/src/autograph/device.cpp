#include "device.hpp"
#include <sstream>

namespace ag {

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length, const GLubyte* msg,
                            void* data) {
  if (severity != gl::DEBUG_SEVERITY_LOW &&
      severity != gl::DEBUG_SEVERITY_NOTIFICATION)
    std::clog << "(GL) " << msg << std::endl;
}

void setDebugCallback() {
  gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
  gl::DebugMessageCallback((GLDEBUGPROC)debugCallback, nullptr);
  gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0,
                          nullptr, true);
  gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER,
                         1111, gl::DEBUG_SEVERITY_NOTIFICATION, -1,
                         "Started logging OpenGL messages");
}

GLenum textureAddressModeToGLenum(TextureAddressMode mode) {
  switch (mode) {
  case TextureAddressMode::Clamp:
    return gl::CLAMP_TO_EDGE;
  case TextureAddressMode::Mirror:
    return gl::MIRRORED_REPEAT;
  case TextureAddressMode::Repeat:
    return gl::REPEAT;
  default:
    return gl::REPEAT;
  }
}

GLenum textureFilterToGLenum(TextureFilter filter) {
  switch (filter) {
  case TextureFilter::Nearest:
    return gl::NEAREST;
  case TextureFilter::Linear:
    return gl::LINEAR;
  default:
    return gl::NEAREST;
  }
}

GLenum primitiveTypeToGLenum(PrimitiveType primitiveType) {
  switch (primitiveType) {
  case PrimitiveType::Triangles:
    return gl::TRIANGLES;
  case PrimitiveType::Lines:
    return gl::LINES;
  case PrimitiveType::Points:
    return gl::POINTS;
  default:
    return gl::POINTS;
  }
}

GLuint compileShader(GLenum stage, const char* source, std::ostream& infoLog) {
  GLuint obj = gl::CreateShader(stage);
  const char* shaderSources[1] = {source};
  gl::ShaderSource(obj, 1, shaderSources, NULL);
  gl::CompileShader(obj);
  GLint status = gl::TRUE_;
  GLint logsize = 0;
  gl::GetShaderiv(obj, gl::COMPILE_STATUS, &status);
  gl::GetShaderiv(obj, gl::INFO_LOG_LENGTH, &logsize);
  if (status != gl::TRUE_) {
    if (logsize != 0) {
      char* logbuf = new char[logsize];
      gl::GetShaderInfoLog(obj, logsize, &logsize, logbuf);
      infoLog << logbuf;
      delete[] logbuf;
      gl::DeleteShader(obj);
    }
    return 0;
  }
  return obj;
}

bool linkProgram(GLuint program, std::ostream& infoLog) {
  GLint status = gl::TRUE_;
  GLint logsize = 0;
  gl::LinkProgram(program);
  gl::GetProgramiv(program, gl::LINK_STATUS, &status);
  gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &logsize);
  if (status != gl::TRUE_) {
    if (logsize != 0) {
      char* logbuf = new char[logsize];
      gl::GetProgramInfoLog(program, logsize, &logsize, logbuf);
      infoLog << logbuf;
      delete[] logbuf;
    }
    return true;
  }
  return false;
}

// constructor
Device::Device(const DeviceConfig& config_)
	: config{ config_ }, frame_id{ 0 }, last_framebuffer_obj{ 0 }, screen_surface{0, config_.init_fb_width, config_.init_fb_height } {

  setDebugCallback();
  gl::CreateFramebuffers(1, &render_to_texture_fbo);
  // (this is a GL 4.5 extension, so we are deliberately
  //  sacrificing compatibility here)
  // TODO remove this, this is useless
  //gl::ClipControl(gl::UPPER_LEFT, gl::ZERO_TO_ONE);

  constexpr std::size_t upload_buf_size = 3 * 1024 * 1024;
  frame_fence = std::move(createFence(0));
  GLuint upload_buf_obj =
      createBuffer(upload_buf_size, nullptr, BufferUsage::Upload);
  void* mapped = mapBuffer(upload_buf_obj, BufferUsage::Upload, 0, upload_buf_size);
  default_upload_buffer = std::make_unique<UploadBuffer>(
      RawBuffer{upload_buf_size, BufferUsage::Upload, upload_buf_obj}, mapped);
  bind_state.reset();
}

GLuint Device::createBuffer(std::size_t size, const void* data,
                            BufferUsage usage) {
  GLbitfield flags = 0;
  if (usage == BufferUsage::Readback) {
    flags |= gl::MAP_READ_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else if (usage == BufferUsage::Upload) {
    flags |= gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else {
    flags = 0;
  }

  GLuint buf_obj;
  gl::CreateBuffers(1, &buf_obj);
  gl::NamedBufferStorage(buf_obj, size, data, flags);
  return buf_obj;
}

void* Device::mapBuffer(GLuint handle, BufferUsage usage, size_t offset,
                        size_t size) {
  // all our operations are unsynchronized
  GLbitfield flags = gl::MAP_UNSYNCHRONIZED_BIT;
  if (usage == BufferUsage::Readback) {
    flags |= gl::MAP_READ_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else if (usage == BufferUsage::Upload) {
    flags |= gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT;
  } else {
    // cannot map a DEFAULT buffer
    throw std::logic_error(
        "Trying to map a buffer allocated with BufferUsage::Default");
  }
  return gl::MapNamedBufferRange(handle, offset, size, flags);
}

void Device::bindTexture(unsigned slot, GLuint handle) {
  assert(slot < kMaxTextureUnits);
    bind_state.textures[slot] = handle;
}


void Device::bindSampler(unsigned slot, GLuint handle) {
  assert(slot < kMaxTextureUnits);
    bind_state.samplers[slot] = handle;
}


void Device::pushDebugGroup(const char* message)
{
    gl::PushDebugGroup(gl::DEBUG_SOURCE_APPLICATION, 0, std::strlen(message), message);
}

void Device::popDebugGroup()
{
    gl::PopDebugGroup();
}

GLuint Device::createSamplerGL(const SamplerInfo& info) {
  GLuint sampler_obj;
  gl::CreateSamplers(1, &sampler_obj);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MIN_FILTER,
                        textureFilterToGLenum(info.minFilter));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MAG_FILTER,
                        textureFilterToGLenum(info.magFilter));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_R,
                        textureAddressModeToGLenum(info.addrU));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_S,
                        textureAddressModeToGLenum(info.addrV));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_T,
                        textureAddressModeToGLenum(info.addrW));
  return sampler_obj;
}

const char* getShaderStageName(GLenum stage) {
  switch (stage) {
  case gl::VERTEX_SHADER:
    return "VERTEX_SHADER";
  case gl::FRAGMENT_SHADER:
    return "FRAGMENT_SHADER";
  case gl::GEOMETRY_SHADER:
    return "GEOMETRY_SHADER";
  case gl::TESS_CONTROL_SHADER:
    return "TESS_CONTROL_SHADER";
  case gl::TESS_EVALUATION_SHADER:
    return "TESS_EVALUATION_SHADER";
  case gl::COMPUTE_SHADER:
    return "COMPUTE_SHADER";
  default:
    return "<invalid>";
  }
}

GLuint compileAndAttachShader(GLuint program_obj, GLenum stage,
                              const char* source) {
  std::ostringstream infoLog;
  GLuint shader_obj = compileShader(stage, source, infoLog);
  if (!shader_obj) {
    std::clog << "===============================================================\n";
	std::clog << "Shader compilation error (stage:" << getShaderStageName(stage) << ")\n";
	std::clog << "Compilation log follows:\n\n" << infoLog.str() << "\n\n";
    return 0;
  }
  gl::AttachShader(program_obj, shader_obj);
  return shader_obj;
}


Fence Device::createFence(uint64_t initialValue) {
  Fence fence;
  fence.current_value = initialValue;
  return std::move(fence);
}

GLuint Device::createComputeProgram(const ComputePipelineDesc& desc) {
  GLuint cs_obj = 0;
  GLuint program_obj = gl::CreateProgram();
  cs_obj =
      compileAndAttachShader(program_obj, gl::COMPUTE_SHADER, desc.CSSource);
  if (!cs_obj) {
    gl::DeleteProgram(program_obj);
    return 0;
  }
  std::ostringstream linkInfoLog;
  bool link_error = linkProgram(program_obj, linkInfoLog);
  gl::DetachShader(program_obj, cs_obj);
  gl::DeleteShader(cs_obj);
  if (link_error) {
	  std::clog << "===============================================================\n";
		  std::clog << "Shader link error\n";
		  std::clog << "Compilation log follows:\n\n" << linkInfoLog.str() << "\n\n";
    gl::DeleteProgram(program_obj);
    program_obj = 0;
  }
  return program_obj;
}

GLuint Device::createProgramFromShaderPipeline(const GraphicsPipelineDesc& desc) {
  // compile programs
  GLuint vs_obj = 0;
  GLuint fs_obj = 0;
  GLuint gs_obj = 0;
  GLuint tcs_obj = 0;
  GLuint tes_obj = 0;
  GLuint program_obj = gl::CreateProgram();
  bool compilation_error = false;
  vs_obj =
      compileAndAttachShader(program_obj, gl::VERTEX_SHADER, desc.VSSource);
  if (!vs_obj)
    compilation_error = true;
  fs_obj =
      compileAndAttachShader(program_obj, gl::FRAGMENT_SHADER, desc.PSSource);
  if (!fs_obj)
    compilation_error = true;
  if (desc.GSSource) {
    gs_obj =
        compileAndAttachShader(program_obj, gl::GEOMETRY_SHADER, desc.GSSource);
    if (!gs_obj)
      compilation_error = true;
  }
  if (desc.DSSource) {
    tes_obj = compileAndAttachShader(program_obj, gl::TESS_EVALUATION_SHADER,
		desc.DSSource);
    if (!tes_obj)
      compilation_error = true;
  }
  if (desc.HSSource) {
    tcs_obj = compileAndAttachShader(program_obj, gl::TESS_CONTROL_SHADER,
		desc.HSSource);
    if (!tcs_obj)
      compilation_error = true;
  }

  bool link_error = false;
  if (!compilation_error) {
    std::ostringstream linkInfoLog;
    link_error = linkProgram(program_obj, linkInfoLog);
    if (link_error) {
		std::clog << "===============================================================\n";
      std::clog << "Shader link error\n";
      std::clog << "Compilation log follows:\n\n" << linkInfoLog.str() << "\n\n";
    }
  }

  if (vs_obj)
    gl::DeleteShader(vs_obj);
  if (fs_obj)
    gl::DeleteShader(fs_obj);
  if (gs_obj)
    gl::DeleteShader(gs_obj);
  if (tcs_obj)
    gl::DeleteShader(tcs_obj);
  if (tes_obj)
    gl::DeleteShader(tes_obj);
  if (link_error) {
    gl::DeleteProgram(program_obj);
    program_obj = 0;
  }

  return program_obj;
}

GLuint createVertexArrayObject(gsl::span<const VertexAttribute> attribs) {
  GLuint strides[Device::kMaxVertexBufferSlots] = {0};
  GLuint vertex_array_obj;
  gl::CreateVertexArrays(1, &vertex_array_obj);
  for (int attribindex = 0; attribindex < attribs.size(); ++attribindex) {
    const auto& a = attribs[attribindex];
    assert(a.slot < Device::kMaxVertexBufferSlots);
    gl::EnableVertexArrayAttrib(vertex_array_obj, attribindex);
    gl::VertexArrayAttribFormat(vertex_array_obj, attribindex, a.size, a.type,
                                a.normalized, strides[a.slot]);
    gl::VertexArrayAttribBinding(vertex_array_obj, attribindex, a.slot);
    strides[a.slot] += a.stride;
  }
  gl::BindVertexArray(0);
  return vertex_array_obj;
}

GraphicsPipeline
Device::createGraphicsPipeline(const GraphicsPipelineDesc& desc) {
	GraphicsPipeline pp;
	pp.program = createProgramFromShaderPipeline(desc);
	pp.vao = createVertexArrayObject(desc.vertexAttribs);
	pp.blend_state = desc.blendState;
	pp.depth_stencil_state = desc.depthStencilState;
	pp.rasterizer_state = desc.rasterizerState;
	return std::move(pp);
}

ComputePipeline Device::createComputePipeline(const ComputePipelineDesc& desc) {
	ComputePipeline pp;
	pp.program = createComputeProgram(desc);
	return std::move(pp);
}

void Device::bindVertexBuffer(unsigned slot, GLuint handle, size_t offset,
                              size_t size, unsigned stride) {
  bind_state.vertexBuffers[slot] = handle;
  bind_state.vertexBufferOffsets[slot] = offset;
  bind_state.vertexBufferStrides[slot] = stride;
  bind_state.vertexBuffersUpdated = true;
}

void Device::bindIndexBuffer(GLuint handle, size_t offset, size_t size,
                             IndexType type) {
  if (type == IndexType::UShort)
    bind_state.indexBufferType = gl::UNSIGNED_SHORT;
  else
    bind_state.indexBufferType = gl::UNSIGNED_INT;
  bind_state.indexBuffer = handle;
}

void Device::bindUniformBuffer(unsigned slot, GLuint handle, size_t offset,
                               size_t size) {
  bind_state.uniformBuffers[slot] = handle;
  bind_state.uniformBufferSizes[slot] = size;
  bind_state.uniformBufferOffsets[slot] = offset;
}

void Device::bindSurface(GLuint handle) { bindFramebufferObject(handle); }

void Device::bindRWTexture(unsigned slot, GLuint handle) {
  assert(slot < kMaxTextureUnits);
  bind_state.images[slot] = handle;
}

void Device::bindRenderTexture(unsigned slot, GLuint handle) {
  bindFramebufferObject(render_to_texture_fbo);
  gl::NamedFramebufferTexture(render_to_texture_fbo,
                              gl::COLOR_ATTACHMENT0 + slot, handle, 0);

  static const GLenum drawBuffers[8] = {
      gl::COLOR_ATTACHMENT0,     gl::COLOR_ATTACHMENT0 + 1,
      gl::COLOR_ATTACHMENT0 + 2, gl::COLOR_ATTACHMENT0 + 3,
      gl::COLOR_ATTACHMENT0 + 4, gl::COLOR_ATTACHMENT0 + 5,
      gl::COLOR_ATTACHMENT0 + 6, gl::COLOR_ATTACHMENT0 + 7};
  gl::DrawBuffers(slot + 1, drawBuffers); // ??
}

void Device::bindViewport(int x, int y, int width, int height)
{
	gl::Viewport(x, y, width, height);
}

void Device::bindDepthRenderTexture(GLuint handle) {
  bindFramebufferObject(render_to_texture_fbo);
  gl::NamedFramebufferTexture(render_to_texture_fbo, gl::DEPTH_ATTACHMENT,
                              handle, 0);
}

void Device::bindComputePipeline(const ComputePipeline& pp) {
  gl::UseProgram(pp.program.get());
}

void Device::bindGraphicsPipeline(const GraphicsPipeline& pp) {
  gl::UseProgram(pp.program.get());
  gl::BindVertexArray(pp.vao.get());
  if (pp.depth_stencil_state.depthTestEnable)
    gl::Enable(gl::DEPTH_TEST);
  else
    gl::Disable(gl::DEPTH_TEST);
  if (pp.blend_state.enabled) {
    gl::Enable(gl::BLEND);
    gl::BlendEquationSeparatei(0, pp.blend_state.modeRGB,
                               pp.blend_state.modeAlpha);
    gl::BlendFuncSeparatei(0, pp.blend_state.funcSrcRGB,
                           pp.blend_state.funcDstRGB, pp.blend_state.funcSrcAlpha,
                           pp.blend_state.funcDstAlpha);
  } else
    gl::Disable(gl::BLEND);
  if (pp.depth_stencil_state.stencilEnable) {
    gl::Enable(gl::STENCIL_TEST);
    gl::StencilFuncSeparate(
        pp.depth_stencil_state.stencilFace, pp.depth_stencil_state.stencilFunc,
        pp.depth_stencil_state.stencilRef, pp.depth_stencil_state.stencilMask);
    gl::StencilOp(pp.depth_stencil_state.stencilOpSfail,
                  pp.depth_stencil_state.stencilOpDPFail,
                  pp.depth_stencil_state.stencilOpDPPass);
  } else
    gl::Disable(gl::STENCIL_TEST);
  gl::PolygonMode(gl::FRONT_AND_BACK, pp.rasterizer_state.fillMode);
  gl::Disable(gl::CULL_FACE);
}

void Device::clearColor(GLuint framebuffer_obj, const ag::ClearColor& color) {
  // XXX: clear specific draw buffer?
  gl::ClearNamedFramebufferfv(framebuffer_obj, gl::COLOR, 0, color.rgba);
}

void Device::clearDepth(GLuint framebuffer_obj, float depth) {
  gl::ClearNamedFramebufferfv(framebuffer_obj, gl::DEPTH, 0, &depth);
}

void Device::draw(PrimitiveType primitiveType, unsigned first, unsigned count) {
  bindState();
  gl::DrawArrays(primitiveTypeToGLenum(primitiveType), first, count);
  unbindState();
}

void Device::drawIndexed(PrimitiveType primitiveType, unsigned first,
                         unsigned count, unsigned baseVertex) {
  bindState();
  auto indexStride = bind_state.indexBufferType == gl::UNSIGNED_INT ? 4 : 2;
  gl::DrawElementsBaseVertex(
      primitiveTypeToGLenum(primitiveType), count, bind_state.indexBufferType,
      ((const char*)((uintptr_t)first * indexStride)), baseVertex);
  unbindState();
}

void Device::dispatchCompute(unsigned threadGroupCountX,
                             unsigned threadGroupCountY,
                             unsigned threadGroupCountZ) {
  bindState();
  gl::DispatchCompute(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void Device::bindFramebufferObject(GLuint framebuffer_obj) {
  if (last_framebuffer_obj != framebuffer_obj) {
    last_framebuffer_obj = framebuffer_obj;
    gl::BindFramebuffer(gl::FRAMEBUFFER, framebuffer_obj);
  }
}

void Device::unbindState()
{
	// huge hack: re-create the render-to-texture FBO
	if (last_framebuffer_obj == render_to_texture_fbo) {
		gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
		last_framebuffer_obj = 0;
		gl::DeleteFramebuffers(1, &render_to_texture_fbo);
		gl::CreateFramebuffers(1, &render_to_texture_fbo);
	}
}

void Device::bindState() {
    for (unsigned i = 0; i < kMaxVertexBufferSlots; ++i)
      if (bind_state.vertexBuffers[i])
        gl::BindVertexBuffer(i, bind_state.vertexBuffers[i],
                             bind_state.vertexBufferOffsets[i],
                             bind_state.vertexBufferStrides[i]);
      else
        gl::BindVertexBuffer(i, 0, 0, 0);
    gl::BindTextures(0, kMaxTextureUnits, bind_state.textures.data());
    gl::BindSamplers(0, kMaxTextureUnits, bind_state.samplers.data());
    gl::BindImageTextures(0, kMaxImageUnits, bind_state.images.data());
    for (unsigned i = 0; i < kMaxUniformBufferSlots; ++i) {
      if (bind_state.uniformBuffers[i])
        gl::BindBufferRange(gl::UNIFORM_BUFFER, i, bind_state.uniformBuffers[i],
                            bind_state.uniformBufferOffsets[i],
                            bind_state.uniformBufferSizes[i]);
      else
        gl::BindBufferBase(gl::UNIFORM_BUFFER, i, 0);
    }

  if (bind_state.indexBuffer)
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, bind_state.indexBuffer);
  bind_state.reset();
}

GLuint Device::createTexture1D(PixelFormat format, unsigned width) {
  GLuint tex_obj;
  auto glfmt = pixelFormatToGL(format);
  gl::CreateTextures(gl::TEXTURE_1D, 1, &tex_obj);
  gl::TextureStorage1D(tex_obj, 1, glfmt.internalFormat, width);
  return tex_obj;
}

GLuint Device::createTexture2D(PixelFormat format, unsigned width,
                               unsigned height) {
  GLuint tex_obj;
  auto glfmt = pixelFormatToGL(format);
  gl::CreateTextures(gl::TEXTURE_2D, 1, &tex_obj);
  gl::TextureStorage2D(tex_obj, 1, glfmt.internalFormat, width, height);
  return tex_obj;
}

GLuint Device::createTexture3D(PixelFormat format, unsigned width,
                               unsigned height, unsigned depth) {
  GLuint tex_obj;
  auto glfmt = pixelFormatToGL(format);
  gl::CreateTextures(gl::TEXTURE_3D, 1, &tex_obj);
  gl::TextureStorage3D(tex_obj, 1, glfmt.internalFormat, width, height, depth);
  return tex_obj;
}
}
