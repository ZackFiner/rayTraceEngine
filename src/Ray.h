#pragma once
#include "ofMain.h"

class SceneObject;
struct RayHit
{
	glm::vec3 hitPos, hitNorm, hitDir;
	float hitDist;
	SceneObject* hitObject;
};

class Ray
{
	glm::vec3 dir, orig;
	Ray(const glm::vec3& dir, const glm::vec3 orig);
	RayHit getHit(std::vector<SceneObject*>);
};

class RayCam
{

};