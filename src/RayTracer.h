#pragma once
#include "ofMain.h"
#include "Light.h"
#include "SceneObjects.h"
#include "Ray.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
ofColor getColFromVec(const glm::vec3& col);
namespace RayTracer {

	ofImage testRayTracer(const RayCam&, const std::vector<SceneObject*>&, const std::vector<Light*>&, const glm::vec2&, ofImage&);
	ofImage lambertRayTracer(const RayCam&, const std::vector<SceneObject*>&, const std::vector<Light*>&, const glm::vec2&, ofImage&);
	ofImage phongRayTracer(const RayCam&, const std::vector<SceneObject*>&, const std::vector<Light*>&, const glm::vec2&, ofImage&);
}