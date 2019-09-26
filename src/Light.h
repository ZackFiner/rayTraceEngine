#pragma once
#include "ofMain.h"
#include "Ray.h"

class Light
{
	glm::vec3 pos;
	glm::vec3 color;
	float intesnity;
public:
	glm::vec3 getIntensity(const RayHit& point);
};