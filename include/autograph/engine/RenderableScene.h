#pragma once
#include <autograph/engine/EntityManager.h>
#include <autograph/gl/Texture.h>

namespace ag {

enum RenderLayer {
  RL_Deferred = 0,
  RL_ForwardOpaque = 1,
  RL_ForwardTranslucent = 2,
  RL_Custom = 3,
};

enum class StdTexture {
  Albedo,
  NormalMap,
  Roughness,
  Metallic,
  AmbientOcclusion,
  Lightmap
};

struct StdMaterial 
{
  gl::Texture *albedo = nullptr;
  gl::Texture *normals = nullptr;
  gl::Texture *roughness = nullptr;
  gl::Texture *metallic = nullptr;
  gl::Texture *ambientOcclusion = nullptr;
  gl::Texture *lightmap = nullptr;
};

class RenderableScene : public ComponentManager<StdMaterial> 
{
};

}