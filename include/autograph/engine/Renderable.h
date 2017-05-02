#pragma once
#include <autograph/Engine/EntityManager.h>
#include <autograph/Gfx/Texture.h>

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
  Texture *albedo = nullptr;
  Texture *normals = nullptr;
  Texture *roughness = nullptr;
  Texture *metallic = nullptr;
  Texture *ambientOcclusion = nullptr;
  Texture *lightmap = nullptr;
};

class AG_ENGINE_API RenderableComponents : public ComponentManager<StdMaterial>
{
public:
	void showGUI(ID id) override;
};

}