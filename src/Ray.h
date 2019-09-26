#pragma once
#include "ofMain.h"

class SceneObject;
class RayHit
{
public:
	glm::vec3 hitPos, hitNorm, hitDir;
	float hitDist;
	SceneObject* hitObject;
	RayHit();
	RayHit(const RayHit&);
	RayHit& operator=(const RayHit&);
};

class Ray
{
	glm::vec3 dir, orig;
	Ray(const glm::vec3& dir, const glm::vec3 orig);
	RayHit getHit(std::vector<SceneObject*>);
	Ray();
	Ray(const Ray&);
	Ray& operator=(const Ray&);
};

class RayCam
{
private:
};