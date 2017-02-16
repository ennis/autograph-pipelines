// Shared ImGui utilities
#pragma once
#include <autograph/gl/All.h>
#include <autograph/Camera.h>
#include <autograph/engine/Scene.h>

namespace ag 
{
namespace gui 
{
	void beginFrame();
	void sceneEditor(const Camera &cam, EntityList &scene, ID rootEntityID);
	void endFrame();
}
}