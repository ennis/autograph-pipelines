#pragma once
#include <any>
#include <autograph/Core/Support/HashCombine.h>
#include <autograph/Core/Support/SmallVector.h>
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

class FrameGraph {
public:
  friend class PassBuilder;
  friend class PassResources;

  ////////////////////////////////////////////
  struct Resource {
    int handle;
    int renameIndex;
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
	  Resource r = frameGraph.addResource(rd);
      pass.creates.push_back(r);
	  pass.writes.push_back(r);
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

  void compile() {
    //////////////////////////////////////////////
    // Topological sorting
    // by construction, 'passes' already contains
    // a topological sort of the frame graph

    //////////////////////////////////////////////
    // concurrent resource writes detection and resource lifetime calculation
    // (begin = pass that creates the resource, end = last pass that reads the
    // resource)
    bool hasWriteConflicts = false;
    std::vector<int> r_ren; // read rename list (contains the rename index that
                            // should appear for the next read from a resource)
    std::vector<int> w_ren; // write rename list (contains the rename index that
                            // should appear for the next write to a resource)
    r_ren.resize(resources.size(), 0);
    w_ren.resize(resources.size(), 0);

    int pass_index = 0;
    for (auto &p : passes) {
      for (auto &r : p.reads) {
        // ensure we can read this resource
        if (r_ren[r.handle] > r.renameIndex) {
          // we already bumped the rename index for this resource: it was
          // already written to
          warningMessage(
              "[FrameGraph] read/write conflict detected on resource {}.{}",
              r.handle, r.renameIndex);
          hasWriteConflicts = true;
        } else {
			AG_DEBUG("write rename index for {}: .{} -> .{}", r.handle, r.renameIndex, r.renameIndex + 1);
			w_ren[r.handle] = r.renameIndex + 1;
        }
        auto &res = resources[r.handle];
        // update lifetime end
        // resource is read during this pass, so it must outlive it
        if (res.lifetimeEnd < pass_index) {
          res.lifetimeEnd = pass_index;
		  AG_DEBUG("lifetime of {}: pass {} -> {}", r.handle, res.lifetimeBegin, res.lifetimeEnd);
        }
      }

      for (auto &w : p.writes) {
        auto &res = resources[w.handle];
        if (res.lifetimeBegin == -1) {
          res.lifetimeBegin = pass_index;
        }

        // Note: p.writes should not contain the same resource twice with a
        // different rename index
        if (w_ren[w.handle] > w.renameIndex) {
          warningMessage(
              "[FrameGraph] read/write conflict detected on resource {}.{}",
              w.handle, w.renameIndex);
          hasWriteConflicts = true;
        } else {
          // the next pass cannot use this rename for either read or write
          // operations
			AG_DEBUG("read rename index for {}: .{} -> .{}", w.handle, w.renameIndex, w.renameIndex + 1);
			AG_DEBUG("write rename index for {}: .{} -> .{}", w.handle, w.renameIndex, w.renameIndex + 1);
			r_ren[w.handle] = w.renameIndex + 1;
          w_ren[w.handle] = w.renameIndex + 1;
        }
      }
      ++pass_index;
    }

    //////////////////////////////////////////////
    // transient resource allocation
    pass_index = 0;

    for (auto &p : passes) {
      // create resources that should be created
      for (auto &c : p.creates) {
        auto &r = resources[c.handle];
        if (auto texres = ag::get_if<TextureResource>(&r.res)) {
          // create the texture
          texres->texId =
              getCompatibleTextureResource(texres->desc, pass_index);
        } else {
          // TODO other resources
        }
      }

      ++pass_index;

      // release read-from resources that should be released on this pass
      for (auto &read : p.reads) {
		  auto &r = resources[read.handle];
        if (r.lifetimeEnd >= pass_index) {
          if (auto texres = ag::get_if<TextureResource>(&r.res)) {
            allocatedTextures[texres->texId].used = -1;
          } else {
            // TODO other resources
          }
        }
      }
    }
  }

  ////////////////////////////////////////////
private:
  struct TextureResource {
    Texture::Desc desc;
    int texId;
  };

  struct ResourceDesc {
    int handle;
    int lifetimeBegin;
    int lifetimeEnd;
    ag::variant<TextureResource> res;
  };

  Resource addResource(const ResourceDesc &rd) {
	  resources.push_back(rd);
	  int handle = (int)(resources.size() - 1);
	  AG_DEBUG("addResource {}.{}", handle, 0);
	  return Resource{ handle,0 };
  }

  // virtual resources (can be aliased)
  std::vector<ResourceDesc> resources;
  // list of passes
  std::vector<Pass> passes;

  // allocated resources
  struct AllocatedTexture {
	  Texture::Desc desc;
	  Texture tex;
	  int used = -1;
  };

  int getCompatibleTextureResource(const Texture::Desc &desc, int passId) {
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