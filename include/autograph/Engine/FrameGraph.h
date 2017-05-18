#pragma once
#include <any>
#include <autograph/Core/Support/HashCombine.h>
#include <autograph/Core/Support/SmallVector.h>
#include <autograph/Gfx/Texture.h>
#include <autograph/Gfx/Buffer.h>
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

class FrameGraph {
public:
  friend class PassBuilder;
  friend class PassResources;

  ////////////////////////////////////////////
  struct Resource {
    int handle;
    int renameIndex;
  };

  struct TextureResourceDesc {
	  Texture::Desc desc;
  };

  struct BufferResourceDesc {
	  size_t size;
	  BufferUsage usage;
  };

  struct ResourceDesc {
	  int handle = -1;
	  int lifetimeBegin = 0;
	  int lifetimeEnd = 0;
	  int allocatedResource = -1;
	  ag::variant<TextureResourceDesc, BufferResourceDesc> desc;
  };

  ////////////////////////////////////////////
  struct Pass {
    template <typename Data, typename ExecuteFn>
    Pass(Data &&d, ExecuteFn &&exec) : data{std::forward<Data>(d)} {
      /*execute =
          [ this, exec = std::move(exec) ](PassResources & passResources) {
        exec(std::any_cast<Data &>(this->data), passResources);
      };*/
    }

    SmallVector<Resource, 4> reads;
    SmallVector<Resource, 4> writes;
    SmallVector<Resource, 4> creates;

    std::string name;
    std::function<void(PassResources &)> execute;
    std::any data;
  };

  ////////////////////////////////////////////
  class PassBuilder {
  public:
    PassBuilder(FrameGraph &fg, Pass &p) : frameGraph{fg}, pass{p} {}

    Resource createTexture2D(ImageFormat fmt, int w, int h,
                             Texture::MipMaps mipMaps = Texture::MipMaps{1},
                             Texture::Samples ms = Texture::Samples{0},
                             Texture::Options opts = (Texture::Options)0) {
      Texture::Desc d;
      d.fmt = fmt;
      d.width = w;
      d.height = h;
      d.depth = 1;
      d.mipMapCount = mipMaps.count;
      d.sampleCount = ms.count;
      d.opts = opts;
      ResourceDesc rd;
      rd.res = TextureResource{d, -1};
	  Resource r = frameGraph.addResourceDesc(rd);
      pass.creates.push_back(r);
	  //pass.writes.push_back(r);
      AG_DEBUG("[FrameGraph] createTexture2D {}.{}", r.handle, r.renameIndex);
      return r;
    }

    Resource read(Resource in) {
      AG_DEBUG("[FrameGraph] read {}.{}", in.handle, in.renameIndex);
	  pass.reads.push_back(in);
      return in;
    }

    Resource write(Resource out) {
      AG_DEBUG("[FrameGraph] write {}.{}", out.handle, out.renameIndex);
      // same resource, bump the rename index
	  pass.writes.push_back(out);
      return Resource{out.handle, out.renameIndex + 1};
    }

	Resource copy(Resource in) {
		const ResourceDesc& rd = frameGraph.getResourceDesc(in.handle);
		Resource out = frameGraph.addResourceDesc(rd);
		pass.creates.push_back(out);
		return out;
	}

	bool isTexture(Resource in) const {
		return frameGraph.isTexture(in.handle);
	}

	Texture::Desc getTextureDesc(Resource in) const {
		return frameGraph.getTextureDesc(in.handle);
	}

	bool isBuffer(Resource in) const { return frameGraph.isBuffer(in.handle); }

	BufferResourceDesc getBufferDesc(Resource in) const {
		return frameGraph.getBufferSize(in.handle);
	}

	ResourceDesc getResourceDesc(Resource in) const {
		return frameGraph.getResourceDesc(in.handle);
	}

    void setName(const char *name_) { pass.name = name_; }

    /*Resource use(Resource inout) {
      AG_DEBUG("[FrameGraph] use {}.{}", inout.handle, inout.renameIndex);
      return Resource{out.handle, out.renameIndex+1};
    }*/

  private:
    FrameGraph &frameGraph;
    Pass &pass;
  };

  ////////////////////////////////////////////
  class PassResources {
  public:
    Texture &getTexture(Resource res);

  private:
    FrameGraph &fg;
    int pass;
  };

  template <typename State, typename SetupFn, typename ExecuteFn>
  State &addPass(SetupFn &setup, ExecuteFn &&exec) {
    // create user data and pass, copy exec callback
    auto p = Pass{State{}, std::forward<ExecuteFn>(exec)};
    // get pointer to user data
    auto pdata = std::any_cast<State>(&p.data);
    // create pass builder and call setup callback
    PassBuilder pb{*this, p};
    setup(pb, *pdata);
    // add pass to list
	AG_DEBUG("-> Add pass {}", p.name.c_str());
    passes.push_back(std::move(p));
    return *pdata;
  }

  void compile();

  ////////////////////////////////////////////
private:
	Resource addResourceDesc(const ResourceDesc &rd);
	const ResourceDesc& getResourceDesc(int handle);
	const Texture::Desc& getTextureDesc(int handle) {
	  auto &rd = getResourceDesc();
	  auto ptexdesc = ag::get_if<TextureResourceDesc>(&rd.desc);
	  if (!ptexdesc)
		  ag::failWith("");
  }

  // virtual resources (can be aliased)
  std::vector<std::unique_ptr<ResourceDesc>> resources;
  // list of passes
  std::vector<Pass> passes;

  struct AllocatedResource
  {
	  bool isInUse = false;
	  //variant<Texture,Buffer> 
  };

  // allocated resources
  struct AllocatedTexture {
	  Texture::Desc desc;
	  Texture tex;
	  int used = -1;
  };

  int getCompatibleResource(const Texture::Desc &desc, int passId) {
    int index = 0;
    for (auto &t : allocatedTextures) {
      // check compatibility of descriptors
      if (t.desc.dims == desc.dims && t.desc.width == desc.width &&
          t.desc.height == desc.height && t.desc.depth == desc.depth &&
          t.desc.fmt == desc.fmt && t.desc.sampleCount == desc.sampleCount &&
          t.desc.mipMapCount == desc.mipMapCount) {
        return index;
      }
      ++index;
    }
    // create a new one
    allocatedTextures.push_back(AllocatedTexture{});
    auto &newtex = allocatedTextures.back();
    newtex.desc = desc;
    newtex.used = passId;
    newtex.tex = Texture{desc};
    return (int)(allocatedTextures.size() - 1);
  }

  // we can keep the list from one frame to another,
  // although I suspect that allocating memory for textures
  // on the GPU should not be that costly
  std::vector<AllocatedTexture> allocatedTextures;

  // std::vector<Texture>
};

// Resource allocation:
// getCompatibleResource(ResourceDesc)
// createResource(ResourceDesc)

/*Texture &FrameGraph::PassResources::getTexture(Resource res) {
  assert(res.handle < fg.resources.size());
  // fg.resources[res.handle].
}*/

/*FrameGraph::Resource test(FrameGraph &fg, FrameGraph::Resource inTex) {
  struct Data {
    FrameGraph::Resource input;
    FrameGraph::Resource output;
  };
  auto &data = fg.addPass<Data>(
      // setup
      [&](Data &data, FrameGraph::PassBuilder &builder) {
        data.input = builder.read(inTex);
        data.output = builder.copyOf(inTex);
      },
      // execute
      [=](Data &data, FrameGraph::PassResources &passResources) {
        // TODO
        Texture &tex = passResources.getTexture(data.input);
      });
  return data.output;
}*/

} // namespace ag