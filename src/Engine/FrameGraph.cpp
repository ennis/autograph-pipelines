#include <autograph/Engine/FrameGraph.h>
#include <autograph/Core/Support/Debug.h>
#include <fstream>
#include <set>

namespace ag {
using ag::get_if;

FrameGraph::PassBuilder::PassBuilder(FrameGraph &fg, Pass &p)
    : frameGraph{fg}, pass{p} {}

FrameGraph::Resource FrameGraph::PassBuilder::createTexture2D(
    ImageFormat fmt, int w, int h, const char *name, Texture::MipMaps mipMaps,
    Texture::Samples ms, Texture::Options opts) {
  Texture::Desc d;
  d.fmt = fmt;
  d.width = w;
  d.height = h;
  d.depth = 1;
  d.mipMapCount = mipMaps.count;
  d.sampleCount = ms.count;
  d.opts = opts;
  ResourceDesc rd;
  rd.name = name;
  rd.v = ResourceDesc::Texture{d, nullptr};
  Resource r = frameGraph.addResourceDesc(rd);
  pass.creates.push_back(r);
  // pass.writes.push_back(r);
  //AG_DEBUG("createTexture2D {}.{}", r.handle, r.renameIndex);
  return r;
}

FrameGraph::Resource FrameGraph::PassBuilder::read(Resource in) {
  //AG_DEBUG("read {}.{}", in.handle, in.renameIndex);
  pass.reads.push_back(in);
  return in;
}

FrameGraph::Resource FrameGraph::PassBuilder::write(Resource out) {
  //AG_DEBUG("write {}.{}", out.handle, out.renameIndex);
  // same resource, bump the rename index
  pass.reads.push_back(out);
  Resource ret{out.handle, out.renameIndex + 1};
  pass.writes.push_back(ret);
  return ret;
}

FrameGraph::Resource FrameGraph::PassBuilder::copy(Resource in) {
  const ResourceDesc &rd = frameGraph.getResourceDesc(in.handle);
  Resource out = frameGraph.addResourceDesc(rd);
 // AG_DEBUG("copy {}.{} -> {}.{}", in.handle, in.renameIndex, out.handle,
  //         out.renameIndex);
  pass.creates.push_back(out);
  return out;
}

void FrameGraph::PassBuilder::setName(const char *name_) { pass.name = name_; }

bool FrameGraph::isBuffer(int handle) const {
  return ag::get_if<ResourceDesc::Buffer>(&getResourceDesc(handle).v) !=
         nullptr;
}

bool FrameGraph::isBuffer(Resource r) const { return isBuffer(r.handle); }

bool FrameGraph::isTexture(int handle) const {
  return ag::get_if<ResourceDesc::Texture>(&getResourceDesc(handle).v) !=
         nullptr;
}

bool FrameGraph::isTexture(Resource r) const { return isTexture(r.handle); }

const Texture::Desc &FrameGraph::getTextureDesc(int handle) const {
  auto &rd = getResourceDesc(handle);
  auto ptexdesc = ag::get_if<ResourceDesc::Texture>(&rd.v);
  if (!ptexdesc)
    ag::failWith("Not a texture");
  return ptexdesc->desc;
}

const Texture::Desc &FrameGraph::getTextureDesc(Resource r) const {
  return getTextureDesc(r.handle);
}

size_t FrameGraph::getBufferSize(int handle) const {
  auto &rd = getResourceDesc(handle);
  auto bufdesc = ag::get_if<ResourceDesc::Buffer>(&rd.v);
  if (!bufdesc)
    ag::failWith("Not a buffer");
  return bufdesc->size;
}

size_t FrameGraph::getBufferSize(Resource r) const {
  return getBufferSize(r.handle);
}

FrameGraph::Resource FrameGraph::addResourceDesc(const ResourceDesc &rd) {
  resources.push_back(std::make_unique<ResourceDesc>(rd));
  int handle = (int)(resources.size() - 1);
  //AG_DEBUG("addResource {}.{}", handle, 0);
  return Resource{handle, 0};
}

const FrameGraph::ResourceDesc &FrameGraph::getResourceDesc(int handle) const {
  return *resources[handle];
}

Buffer &FrameGraph::PassResources::getBuffer(Resource res) const {
  return *ag::get<FrameGraph::ResourceDesc::Buffer>(
              fg_.getResourceDesc(res.handle).v)
              .buf;
}

Texture &FrameGraph::PassResources::getTexture(Resource res) const {
  return *ag::get<FrameGraph::ResourceDesc::Texture>(
              fg_.getResourceDesc(res.handle).v)
              .ptex;
}

void FrameGraph::execute() {
  for (auto&& p : passes) {
    PassResources pass{*this};
    p->execute(pass);
  }
}

void FrameGraph::compile() {
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

  //AG_DEBUG("** BEGIN CONCURRENT RESOURCE USAGE DETECTION");
  int pass_index = 0;
  for (auto &&p : passes) {
    //AG_DEBUG("** PASS {}", p->name);
    for (auto &&r : p->reads) {
      // ensure we can read this resource
      if (r_ren[r.handle] > r.renameIndex) {
        // we already bumped the rename index for this resource: it was
        // already written to
        warningMessage(
            "[FrameGraph] read/write conflict detected on resource {}.{}",
            r.handle, r.renameIndex);
        hasWriteConflicts = true;
      } else {
        //AG_DEBUG("write rename index for {}: .{} -> .{}", r.handle,
        //         r.renameIndex, r.renameIndex + 1);
        w_ren[r.handle] = r.renameIndex + 1;
      }
      auto &&res = resources[r.handle];
      // update lifetime end
      // resource is read during this pass, so it must outlive it
      if (res->lifetimeEnd < pass_index) {
        res->lifetimeEnd = pass_index;
        //AG_DEBUG("lifetime of {}: pass {} -> {}", r.handle, res->lifetimeBegin,
        //         res->lifetimeEnd);
      }
    }

    for (auto &&w : p->writes) {
      auto &&res = resources[w.handle];
      if (res->lifetimeBegin == -1) {
        res->lifetimeBegin = pass_index;
      }

      // Note: p.writes should not contain the same resource twice with a
      // different rename index
      if (w_ren[w.handle] > w.renameIndex) {
        warningMessage(
            "[FrameGraph] read/write conflict detected on resource {}.{}",
            w.handle, w.renameIndex);
        hasWriteConflicts = true;
      } else {
        // the next pass cannot use this rename for write operations
        //AG_DEBUG("write rename index for {}: .{} -> .{}", w.handle,
        //         w.renameIndex, w.renameIndex + 1);
        w_ren[w.handle] = w.renameIndex + 1;
        r_ren[w.handle] = w.renameIndex;
      }
    }
    ++pass_index;
  }

  //////////////////////////////////////////////
  // transient resource allocation
  //AG_DEBUG("** BEGIN RESOURCE ALLOCATION");
  pass_index = 0;
  std::set<Texture *> texturesInUse;
  auto assignTexture = [&](ResourceDesc::Texture &texdesc) {
    for (auto &&tex : textures) {
      if (texturesInUse.count(tex.get()))
        continue;
      auto ptex = tex.get();
      if (ptex->desc() == texdesc.desc) {
        //AG_DEBUG("[REUSING TEXTURE @{} {}x{}x{} {}]", (const void *)ptex,
        //         texdesc.desc.width, texdesc.desc.height, texdesc.desc.depth,
        //         getImageFormatInfo(texdesc.desc.fmt).name);
        texdesc.ptex = ptex;
        texturesInUse.insert(ptex);
        return;
      }
    }
    auto tex = std::make_unique<Texture>(texdesc.desc);
    auto ptex = tex.get();
    textures.push_back(std::move(tex));
    texturesInUse.insert(ptex);
    //AG_DEBUG("[creating new texture @{} {}x{}x{} {}]", (const void *)ptex,
    //         texdesc.desc.width, texdesc.desc.height, texdesc.desc.depth,
    //         getImageFormatInfo(texdesc.desc.fmt).name);
    texdesc.ptex = ptex;
  };

  auto assignBuffer = [&](ResourceDesc::Buffer &bufdesc) {
    auto buf = std::make_unique<Buffer>(bufdesc.size, bufdesc.usage);
    auto pbuf = buf.get();
    buffers.push_back(std::move(buf));
    //AG_DEBUG("[creating new buffer @{} of size {}]", (const void *)pbuf,
    //         bufdesc.size);
    bufdesc.buf = pbuf;
  };

  auto assignCompatibleResource = [&](ResourceDesc &rd) {
    if (auto ptexdesc = get_if<ResourceDesc::Texture>(&rd.v)) {
      assignTexture(*ptexdesc);
    } else if (auto pbufdesc = get_if<ResourceDesc::Buffer>(&rd.v)) {
      assignBuffer(*pbufdesc);
      // always create another buffer, for now
    }
  };

  auto releaseResource = [&](ResourceDesc &rd) {
    if (auto ptexdesc = get_if<ResourceDesc::Texture>(&rd.v)) {
     // AG_DEBUG("[RELEASE TEXTURE @{}]", (const void *)ptexdesc->ptex);
      texturesInUse.erase(ptexdesc->ptex);
    } else if (auto pbufdesc = get_if<ResourceDesc::Buffer>(&rd.v)) {
      // Nothing to do for buffers
      // assignBuffer(*pbufdesc);
    }
  };

  for (auto &&p : passes) {
    //AG_DEBUG("** PASS {}", p->name);
    // create resources that should be created
    for (auto &c : p->creates) {
      auto &r = resources[c.handle];
      assignCompatibleResource(*r);
    }

    // release read-from resources that should be released on this pass
    for (auto &&read : p->reads) {
      auto &res = resources[read.handle];
      if (res->lifetimeEnd <= pass_index) {
        releaseResource(*res);
      }
    }

    ++pass_index;
  }
}

void FrameGraph::dumpGraph(const char *path) {
  std::ofstream fileOut{path};

  fmt::print(fileOut, "digraph G {{\n");

  auto writeLine = [&](auto &&... params) {
    fmt::print(fileOut, std::forward<decltype(params)>(params)...);
    fmt::print(fileOut, "\n");
  };

  auto printResourceNode = [&](const char *passName, Resource r, bool input) {
    auto &desc = getResourceDesc(r.handle);
    std::string name;
    if (!desc.name.empty())
      name = fmt::format("{}{}_{}", desc.name, r.handle, r.renameIndex);
    else
      name = fmt::format("anon{}_{}", r.handle, r.renameIndex);
    writeLine("{} [shape=record,label=\"{}|{{B: {}|E: {}}}\"];", name, name,
              desc.lifetimeBegin, desc.lifetimeEnd);
    if (input) {
      writeLine("{} -> {};", name, passName);
    } else {
      writeLine("{} -> {};", passName, name);
      // node to allocated resource
      std::string rname;
      if (auto tex = ag::get_if<ResourceDesc::Texture>(&desc.v)) {
        rname = fmt::format("tex_{}", (const void *)tex->ptex);
      } else if (auto buf = ag::get_if<ResourceDesc::Buffer>(&desc.v)) {
        rname = fmt::format("buf_{}", (const void *)buf->buf);
      }
      writeLine("{} -> {}  [style=dashed];", name, rname);
    }
  };

  writeLine("subgraph cluster0 {{");
  std::string prev;
  for (auto &&tex : textures) {
    auto thisNode = fmt::format("tex_{}", (const void *)tex.get());
    writeLine("{} [shape=box];", thisNode);
    if (!prev.empty()) 
      writeLine("{} -> {} [style=invis];", prev, thisNode);
	prev = std::move(thisNode);
  }
  for (auto &&buf : buffers) {
    auto thisNode = fmt::format("buf_{}", (const void *)buf.get());
    writeLine("{} [shape=box];", thisNode);
    if (!prev.empty()) 
      writeLine("{} -> {} [style=invis];", prev, thisNode);
	prev = std::move(thisNode);
  }
  writeLine("label=\"Resources\"");
  writeLine("}}");

  writeLine("subgraph cluster1 {{");
  int passIndex = 0;
  for (auto &&p : passes) {
    auto passName = p->name;
    if (passName.empty())
      passName = "pass_" + std::to_string(passIndex);
    writeLine("{} [shape=record,label=\"{}|{}\"]", passName, passIndex,
              passName);
    for (auto &&r : p->reads) {
      printResourceNode(passName.c_str(), r, true);
    }
    for (auto &&w : p->writes) {
      printResourceNode(passName.c_str(), w, false);
    }
    for (auto &&c : p->creates) {
      printResourceNode(passName.c_str(), c, false);
    }
    ++passIndex;
  }
  writeLine("}}");

  writeLine("}}");
}

} // namespace ag