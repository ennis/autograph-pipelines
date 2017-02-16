#include <autograph/engine/Scene.h>

namespace ag {
namespace SceneUtils {

struct Renderable : public Component<Renderable>
{
    const gl::Texture* albedo;
};

Entity* load(const char* id, EntityList& scene, ResourcePool& resourcePool);

}
}
