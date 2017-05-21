#include <autograph/Engine/FrameGraph.h>

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

int FrameGraph::assignCompatibleResource(FrameGraph::ResourceDesc &desc,
                                         int passId) {
  int index = 0;
  if (auto ptexdesc = get_if<ResourceDesc::Texture>(&desc.v)) {
    for (int i = 0; i < textures.size(); ++i) {
      if (isTextureInUse(i))
        continue;
      auto ptex = textures[i].get();
      if (ptex->desc() == ptexdesc->desc) {
        ptexdesc->ptex = ptex;
        setTextureInUse(i);
        return;
      }
    }
    auto tex = std::make_unique<Texture>(ptexdesc->desc);
    auto ptex = tex.get();
    textures.push_back(std::move(tex));
    ptexdesc->ptex = ptex;
  } else if (auto pbufdesc = get_if<ResourceDesc::Buffer>(&desc.v)) {
    /*for (int i = 0; i < buffers.size(); ++i)
    {
            if (isBufferInUse(i)) continue;
            auto ptex = textures[i].get();
            if (ptex->desc() == ptexdesc->desc) {
                    ptexdesc->ptex = ptex;
                    setTextureInUse(i);
                    return;
            }
    }*/
    // always create another buffer, for now
    auto buf = std::make_unique<Buffer>(pbufdesc->size, pbufdesc->usage);
    auto pbuf = buf.get();
    buffers.push_back(std::move(buf));
    pbufdesc->buf = pbuf;
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
        AG_DEBUG("write rename index for {}: .{} -> .{}", r.handle,
                 r.renameIndex, r.renameIndex + 1);
        w_ren[r.handle] = r.renameIndex + 1;
      }
      auto &res = resources[r.handle];
      // update lifetime end
      // resource is read during this pass, so it must outlive it
      if (res.lifetimeEnd < pass_index) {
        res.lifetimeEnd = pass_index;
        AG_DEBUG("lifetime of {}: pass {} -> {}", r.handle, res.lifetimeBegin,
                 res.lifetimeEnd);
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
  pass_index = 0;

  for (auto &p : passes) {
    // create resources that should be created
    for (auto &c : p.creates) {
      auto &r = resources[c.handle];
      if (auto texres = ag::get_if<TextureResource>(&r.res)) {
        // create the texture
        texres->texId = getCompatibleTextureResource(texres->desc, pass_index);
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
} // namespace ag