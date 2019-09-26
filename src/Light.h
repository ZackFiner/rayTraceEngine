#pragma once
#include "ofMain.h"
#include "Ray.h"
#define LIGHT_EPSILON 0.0001f
class Light
{
	glm::vec3 pos;
	glm::vec3 color;
	float intensity;
public:
	Light();
	Light(const glm::vec3&, const glm::vec3&,float);
	Light(const Light&);
	Light& operator=(const Light&);
	glm::vec3 getIntensity(const RayHit& point, const std::vector<SceneObject*>& obj);
};