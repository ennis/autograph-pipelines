#pragma once
#include <autograph/Core/Support/HashCombine.h>
#include <autograph/Core/Support/SmallVector.h>
#include <autograph/Core/Support/Variant.h>
#include <autograph/Engine/Exports.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Texture.h>
#include <functional>
#include <vector>

//#if __has_include(<any>)
#include <any>
//#else
//# include <experimental/any>
//#endif

namespace ag {

/// TODO Description
/// (Represents the graph of rendering passes in a frame, specify passes, dependencies and transient resources,
/// and the frame graph allocates memory for the transient resources in 
/// TODO explain what is a transient resource: resource that only lives for a frame, discarded afterwards
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
      ag::Buffer::Usage usage;
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
    //AG_DEBUG("-> Add pass {}", p->name.c_str());
    passes.push_back(std::move(p));
    return *pdata;
  }

  void compile();
  void execute();
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