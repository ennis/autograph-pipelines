#pragma once
#include <autograph/Engine/Exports.h>
#include <autograph/Core/Types.h>

namespace ag {
	struct Transform;
	struct Camera;

	class AG_ENGINE_API Arcball {
	public:
		// Apply an arcball rotation to an object
		void onMouseDown(int mouseX, int mouseY);
		void onMouseDrag(Transform& transform, const Camera& cam, int screenWidth, int screenHeight, int mouseX, int mouseY);

	private:
		auto getArcballVector(int sw, int sh, int x, int y) const -> vec3;
		int mouseDownX;
		int mouseDownY;
	};
}