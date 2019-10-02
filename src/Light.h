#pragma once
#include "ofMain.h"
#include "Ray.h"
#define LIGHT_EPSILON 0.1f //if you're seeing spots, play with this value
class Light
{
public:
	glm::vec3 pos;
	glm::vec3 color;
	float intensity;
	Light();
	Light(const glm::vec3&, const glm::vec3&,float);
	Light(const Light&);
	Light& operator=(const Light&);
	glm::vec3 getIntensity(const RayHit& point, const std::vector<SceneObject*>& obj);
	void drawDebug() const;
};