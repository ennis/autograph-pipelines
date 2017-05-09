#pragma once
#include <autograph/Engine/EntityManager.h>

namespace ag {
struct AG_ENGINE_API Light {};

class AG_ENGINE_API LightComponents : public ComponentManager<Light> {
public:
	void showGUI(ID id) override;
};

}