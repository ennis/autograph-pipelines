#pragma once
#include <autograph/engine/ResourcePool.h>
#include <autograph/support/IDTable.h>
#include <memory>
#include <vector>

namespace ag {

enum class RenderLayer 
{
  Deferred,
  ForwardOpaque,
  ForwardTranslucent
};

struct RenderableObject {
  ID id;
  RenderLayer layer{Deferred};
  bool castsShadows{true};
  bool receivesShadows{true};
  ag::SmallVector<gl::Texture*, 8> textures;
};

class RenderableScene : public SceneBase<RenderableObject> {
};

}