#pragma once
#include <any>
#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/HashCombine.h>
#include <autograph/Core/Support/SmallVector.h>
#include <autograph/Core/Support/Variant.h>
#include <autograph/Engine/Exports.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Texture.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace ag {

// frame graph resources: textures, buffers, render targets (collection of
// textures bound to a framebuffer) bikeshedding: FGResource FGTextureResource,
// FGRenderTargetResource + mutable versions? FGMutableTexture,
// FGMutableRenderTarget? FrameGraph::TextureResource,
// FrameGraph::RenderTargetResource Template type?
//
// Pointer-like types? .get() method to get the allocated resource during
// execution Wrappers around IDs: must query the framegraph for the allocated
// resource using fg.get(<resource-id>)
//
// Can a resource be viewed as different types?
//
// Bikeshedding / FrameGraph API for requesting transient resources (setup
// object) builder.read(<resource-id>) => registers an input dependency
// modifications:
// builder.<modify>(<resource-id>)
//
// use cases for rendertarget resources: don't want to re-create a framebuffer
// each time don't want to specify too many inputs?
//
// input: builder.read(<...>)
// mutable resources: builder.useXXX(<...>)
// output: builder.writeXXX()
//
// read: builder.read(XXX, usage flags)
// modify: builder.use(XXX, usage flags)
// write: builder.write(XXX, usage flags)
// create: builder.createXXX(usage flags)
//	  all these functions return a resource handle for the pass in question
//(handles have usage metadata)
//
// Query metadata for resources:
// texture: width, height, depth, format, etc.
// buffer: size, etc.
// Q: access part of a resource?
//		e.g. one mip level, one face, part of a render target?
//		for now, just textures
//
// Should resource handles be typed?
// - by mutability? by resource type (many types)?
// - one resource can be seen as different concrete types? (maybe)
//		texture as buffer, texture as render target, buffer as UBO,
// buffer  as SSBO/UAV, buffer as transform feedback buffer
// - one pass can take several types of input:
//		texture, individual mip level, part of render target
// Two great categories: texture (i.e. full tex, mip levels, faces), and buffers
//
// DECISIONS:
// Resources: Textures and buffers, wrappers around untyped handles
// Accessing part of a resource done during execution
// Do not bother too much about resource usage
// Render target: wrapper around a set of textures?
// At first, no way of querying metadata outside execute

// FrameGraph
// - addPass
// - getTexture(id)
// - getBuffer(id)
// - [private] addTextureResource(...)
// - [private] addBufferResource(...)
// FrameGraph::Pass
// - reads: vector<int>
// - writes: vector<int>
// FrameGraph::PassBuilder
// FrameGraph::AllocatedResource (index)
// - pointer to resource
// FrameGraph::ResourceDesc (index)
// - clone()
// FrameGraph::TextureResourceDesc
// - w, h, fmt, mips, samples
// FrameGraph::BufferResourceDesc
// - size, usage

//
// Next iteration:
// Resource should be an indirect pointer to a texture / buffer / framebuffer
// resource FrameGraph::Texture -> ResourceHandle<TexturePrivate>

class AG_ENGINE_API FrameGraph {
public:
  friend class PassBuilder;
  friend class PassResources;

  FrameGraph() = default;
  FrameGraph(const FrameGraph &) = delete;
  FrameGraph &operator=(const FrameGraph &) = delete;
  FrameGraph(FrameGraph &&) = default;
  FrameGraph &operator=(FrameGraph &&) = default;

  ////////////////////////////////////////////
  struct Resource {
    int handle;
    int renameIndex;
  };

  struct ResourceDesc {
    int lifetimeBegin = 0;
    int lifetimeEnd = 0;
    std::string name;

    struct Texture {
      ag::Texture::Desc desc;
      ag::Texture *ptex;
    };
    struct Buffer {
      size_t size;
      ag::BufferUsage usage;
      ag::Buffer *buf;
    };

    ag::variant<Texture, Buffer> v;
  };

  ////////////////////////////////////////////
  class AG_ENGINE_API PassResources {
  public:
    PassResources(FrameGraph &fg) : fg_{fg} {}

    Texture &getTexture(Resource res) const;
    Buffer &getBuffer(Resource res) const;

  private:
    FrameGraph &fg_;
    int pass;
  };

  ////////////////////////////////////////////
  struct Pass {
    template <typename Data, typename ExecuteFn>
    Pass(Data &&d, ExecuteFn &&exec) : data{std::forward<Data>(d)} {
      execute =
          [ this, exec = std::move(exec) ](PassResources & passResources) {
        exec(std::any_cast<Data &>(this->data), passResources);
      };
    }

    SmallVector<Resource, 4> reads;
    SmallVector<Resource, 4> writes;
    SmallVector<Resource, 4> creates;

    std::string name;
    std::function<void(PassResources &)> execute;
    std::any data;
  };

  ////////////////////////////////////////////
  class AG_ENGINE_API PassBuilder {
  public:
    PassBuilder(FrameGraph &fg, Pass &p);
    Resource createTexture2D(ImageFormat fmt, int w, int h,
                             const char *name = nullptr,
                             Texture::MipMaps mipMaps = Texture::MipMaps{1},
                             Texture::Samples ms = Texture::Samples{0},
                             Texture::Options opts = (Texture::Options)0);
    Resource read(Resource in);
    Resource write(Resource out);
    Resource copy(Resource in);
    void setName(const char *name_);

  private:
    FrameGraph &frameGraph;
    Pass &pass;
  };

  template <typename State, typename SetupFn, typename ExecuteFn>
  State &addPass(SetupFn &setup, ExecuteFn &&exec) {
    // create user data and pass, copy exec callback
    auto p = std::make_unique<Pass>(State{}, std::forward<ExecuteFn>(exec));
    // get pointer to user data
    auto pdata = std::any_cast<State>(&p->data);
    // create pass builder and call setup callback
    PassBuilder pb{*this, *p};
    setup(pb, *pdata);
    // add pass to list
    AG_DEBUG("-> Add pass {}", p->name.c_str());
    passes.push_back(std::move(p));
    return *pdata;
  }

  void compile();

  void dumpGraph(const char *path);

  const ResourceDesc &getResourceDesc(int handle) const;
  bool isBuffer(int handle) const;
  bool isBuffer(Resource r) const;
  bool isTexture(int handle) const;
  bool isTexture(Resource r) const;
  const Texture::Desc &getTextureDesc(int handle) const;
  const Texture::Desc &getTextureDesc(Resource r) const;
  size_t getBufferSize(int handle) const;
  size_t getBufferSize(Resource r) const;

  ////////////////////////////////////////////
private:
  Resource addResourceDesc(const ResourceDesc &rd);

  // virtual resources (can be aliased)
  std::vector<std::unique_ptr<ResourceDesc>> resources;
  // we can keep the list from one frame to another,
  // although I suspect that re-allocating memory for resources
  // on the GPU should not be that costly
  std::vector<std::unique_ptr<Texture>> textures;
  std::vector<std::unique_ptr<Buffer>> buffers;
  // list of passes
  std::vector<std::unique_ptr<Pass>> passes;
};

} // namespace ag