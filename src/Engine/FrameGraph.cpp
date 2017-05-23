#include <autograph/Engine/FrameGraph.h>
#include <set>

namespace ag {
using ag::get_if;

FrameGraph::Resource FrameGraph::addResourceDesc(const ResourceDesc &rd) {
  resources.push_back(std::make_unique<ResourceDesc>(rd));
  int handle = (int)(resources.size() - 1);
  AG_DEBUG("addResource {}.{}", handle, 0);
  return Resource{handle, 0};
}

const FrameGraph::ResourceDesc &FrameGraph::getResourceDesc(int handle) const {
  return *resources[handle];
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

  AG_DEBUG("** BEGIN CONCURRENT RESOURCE USAGE DETECTION");
  int pass_index = 0;
  for (auto &p : passes) {
    AG_DEBUG("** PASS {}", p.name);
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
        AG_DEBUG("write rename index for {}: .{} -> .{}", r.handle,
                 r.renameIndex, r.renameIndex + 1);
        w_ren[r.handle] = r.renameIndex + 1;
      }
      auto &res = resources[r.handle];
      // update lifetime end
      // resource is read during this pass, so it must outlive it
      if (res->lifetimeEnd < pass_index) {
        res->lifetimeEnd = pass_index;
        AG_DEBUG("lifetime of {}: pass {} -> {}", r.handle, res->lifetimeBegin,
                 res->lifetimeEnd);
      }
    }

    for (auto &w : p.writes) {
      auto &res = resources[w.handle];
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
        // the next pass cannot use this rename for either read or write
        // operations
        AG_DEBUG("read rename index for {}: .{} -> .{}", w.handle,
                 w.renameIndex, w.renameIndex + 1);
        AG_DEBUG("write rename index for {}: .{} -> .{}", w.handle,
                 w.renameIndex, w.renameIndex + 1);
        r_ren[w.handle] = w.renameIndex + 1;
        w_ren[w.handle] = w.renameIndex + 1;
      }
    }
    ++pass_index;
  }

  //////////////////////////////////////////////
  // transient resource allocation
  AG_DEBUG("** BEGIN RESOURCE ALLOCATION");
  pass_index = 0;
  std::set<Texture *> texturesInUse;
  auto assignTexture = [&](ResourceDesc::Texture &texdesc) {
    for (auto &tex : textures) {
      if (texturesInUse.count(tex.get()))
        continue;
      auto ptex = tex.get();
      if (ptex->desc() == texdesc.desc) {
        AG_DEBUG("[REUSING TEXTURE @{} {}x{}x{} {}]", (const void *)ptex,
                 texdesc.desc.width, texdesc.desc.height, texdesc.desc.depth,
                 getImageFormatInfo(texdesc.desc.fmt).name);
        texdesc.ptex = ptex;
        texturesInUse.insert(ptex);
        return;
      }
    }
    auto tex = std::make_unique<Texture>(texdesc.desc);
    auto ptex = tex.get();
    textures.push_back(std::move(tex));
    texturesInUse.insert(ptex);
    AG_DEBUG("[creating new texture @{} {}x{}x{} {}]", (const void *)ptex, texdesc.desc.width,
             texdesc.desc.height, texdesc.desc.depth,
             getImageFormatInfo(texdesc.desc.fmt).name);
    texdesc.ptex = ptex;
  };

  auto assignBuffer = [&](ResourceDesc::Buffer &bufdesc) {
    auto buf = std::make_unique<Buffer>(bufdesc.size, bufdesc.usage);
    auto pbuf = buf.get();
    buffers.push_back(std::move(buf));
    AG_DEBUG("[creating new buffer @{} of size {}]", (const void *)pbuf, bufdesc.size);
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
      AG_DEBUG("[RELEASE TEXTURE @{}]", (const void *)ptexdesc->ptex);
      texturesInUse.erase(ptexdesc->ptex);
    } else if (auto pbufdesc = get_if<ResourceDesc::Buffer>(&rd.v)) {
      // Nothing to do for buffers
      // assignBuffer(*pbufdesc);
    }
  };

  for (auto &p : passes) {
    AG_DEBUG("** PASS {}", p.name);
    // create resources that should be created
    for (auto &c : p.creates) {
      auto &r = resources[c.handle];
      assignCompatibleResource(*r);
    }

    // release read-from resources that should be released on this pass
    for (auto &read : p.reads) {
      auto &res = resources[read.handle];
      if (res->lifetimeEnd <= pass_index) {
        releaseResource(*res);
      }
    }

    ++pass_index;
  }
}
} // namespace ag