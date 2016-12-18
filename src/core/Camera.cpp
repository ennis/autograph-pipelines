#include <autograph/Camera.h>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

namespace ag
{
	vec3 getArcballVector(int screenWidth, int screenHeight, int x, int y)
	{
		vec3 P = vec3{ 1.0f*x / (float)screenWidth * 2 - 1.0f,
			1.0f*y / (float)screenHeight * 2 - 1.0f,
			0.0f };
		P.y = -P.y;
		float OP_squared = P.x * P.x + P.y * P.y;
		if (OP_squared <= 1)
			P.z = sqrt(1.0f - OP_squared);
		else
			P = glm::normalize(P);
		return P;
	}

	void TrackballCameraControl::cursorMove(ivec2 cursor, RotationMode rotationMode)
	{
		if (rotationMode == RotationMode::Scene) {
			if (cursor != lastCur) {
					glm::vec3 va = getArcballVector(screenWidth, screenHeight, lastCur.x, lastCur.y);
					glm::vec3 vb = getArcballVector(screenWidth, screenHeight, cursor.x, cursor.y);
					float angle = std::acos(std::min(1.0f, glm::dot(va, vb)));
					glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
					glm::mat3 camera2object = glm::inverse(glm::mat3(viewMat) * glm::mat3(modelMatrix));
					glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
					modelMatrix = glm::rotate(modelMatrix, angle, axis_in_object_coord);
					lastCur = cursor;
			}
		}
		else {
			lastCur = cursor;
		}
	}

	void TrackballCameraControl::scrollWheel(double delta, ScrollMode scrollMode)
	{
		
	}

	void TrackballCameraControl::updatePanVectors() 
	{
		auto invLookAt = glm::inverse(getLookAt());
		wCamRight = glm::vec3(invLookAt * glm::vec4(CamRight, 0.0f));
		wCamUp = glm::vec3(invLookAt * glm::vec4(-CamUp, 0.0f));
		wCamFront = glm::vec3(invLookAt * glm::vec4(CamFront, 0.0f));
	}
}