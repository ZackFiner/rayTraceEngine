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
public:
	Ray();
	Ray(const Ray&);
	Ray(const glm::vec3& dir, const glm::vec3 orig);
	Ray& operator=(const Ray&);
	RayHit getHit(std::vector<SceneObject*>);
};

class RayCam
{
	glm::vec3 origin;
	glm::vec3  front, left, right;
	float focalDist;
public:
	RayCam(const glm::vec3& f, const glm::vec3& r, const glm::vec3& u, const glm::vec3& o);
	Ray getRay(const glm::vec2& coord, const glm::vec2& dim);
	void setPos(const glm::vec3& newPos);
	void rotate(const glm::quat& rot);
};