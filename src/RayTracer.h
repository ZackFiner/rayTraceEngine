#pragma once
#include "ofMain.h"
#include "Light.h"
#include "SceneObjects.h"
#include "Ray.h"

namespace RayTracer {

	ofImage testRayTracer(const RayCam&, const std::vector<SceneObject*>&, const std::vector<Light*>&, const glm::vec2&);
	ofImage lambertRayTracer(const RayCam&, const std::vector<SceneObject*>&, const std::vector<Light*>&, const glm::vec2&);
	ofImage phongRayTracer(const RayCam&, const std::vector<SceneObject*>&, const std::vector<Light*>&, const glm::vec2&);
}