#include <autograph/engine/EntityManager.h>
#include <autograph/engine/Plugin.h>

namespace ag {
class ResourcePool;
struct SceneObject;

class SceneLoader : public Extension {
public:
  // returns true if the scene was actually loaded
  virtual bool loadScene(const char *path, Scene &scene, ID &rootObject,
                         ResourcePool &pool,
                         SceneObject *parentObject = nullptr) = 0;
};

} // namespace ag
