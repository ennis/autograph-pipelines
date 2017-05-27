#include <autograph/Engine/EntityManager.h>
#include <autograph/Engine/Plugin.h>

namespace ag {
class Cache;
struct SceneObject;

class SceneLoader : public Extension {
public:
  // returns true if the scene was actually loaded
  virtual bool loadScene(const char *path, Scene &scene, ID &rootObject,
                         Cache &cache, SceneObject *parentObject = nullptr) = 0;
};

} // namespace ag
