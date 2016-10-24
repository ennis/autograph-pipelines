#pragma once
#include <autograph/Config.h>
#include <autograph/gl/DrawState.h>
#include <autograph/gl/Bind.h>
#include <autograph/support/Optional.h>
#include <autograph/support/SmallVector.h>
#include <autograph/support/Utils.h>
#include <gl_core_4_5.h>

namespace ag {
namespace gl {

enum class StateGroupMask {
  Viewports = (1 << 0),		// DONE
  Framebuffer = (1 << 1),	// DONE
  ScissorRect = (1 << 2),
  BlendStates = (1 << 3),	// DONE
  RasterizerState = (1 << 4),	// DONE
  DepthStencilState = (1 << 5),	// DONE
  Textures = (1 << 6), // DONE
  Samplers = (1 << 7),
  UniformBuffers = (1 << 8),       // DONE
  ShaderStorageBuffers = (1 << 9), // DONE
  VertexArray = (1 << 10),         // DONE
  Program = (1 << 11),             // DONE
  VertexBuffers = (1 << 12),       // DONE
  IndexBuffer = (1 << 13),         // DONE
  Images = (1 << 14)               // DONE
};

ENUM_BIT_FLAGS_OPERATORS(StateGroupMask)

//////////////////////////////////////////////////
struct StateGroup {

  RasterizerState rasterizerState;
  DepthStencilState depthStencilState;

  // Blend states
  struct IndexedBlendState {
    int slot;
    BlendState state;
  };
  SmallVector<IndexedBlendState> blendStates;

  // viewports
  struct IndexedViewport {
    int slot;
    Viewport v;
  };
  SmallVector<IndexedViewport> viewports;

};

//////////////////////////////////////////////////
struct UniformGroup {
  // textures
  struct TextureSlot {
    int slot;
    Texture *tex;
  };
  SmallVector<TextureSlot> textures;

  // images
  struct ImageSlot {
    int slot;
    Texture *tex;
  };
  SmallVector<ImageSlot> images;

  // samplers
  struct SamplerSlot {
    int slot;
    Sampler *sampler;
  };
  SmallVector<SamplerSlot> samplers;
};

struct StateGroup {
  StateGroupMask mask;
  VertexArray *vao;
  Framebuffer *fbo;
  Program *prog;
  BufferSlice ibo;


  // uniform buffers
  struct UniformBuffer {
    int slot;
    BufferSlice buf;
  };
  SmallVector<UniformBuffer> ubos;

  // shader storage buffers
  struct ShaderStorageBuffer {
    int slot;
    BufferSlice buf;
  };
  SmallVector<ShaderStorageBuffer> ssbos;

  // vbos
  struct VertexBuffer {
    int slot;
    int stride;
    BufferSlice buf;
  };
  SmallVector<VertexBuffer> vbos;

  void operator()(GLBindContext &bindContext);
};

}
}