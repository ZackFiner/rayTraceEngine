#include "Ray.h"
#include "SceneObjects.h"
#include <limits>

RayHit::RayHit()
{}

RayHit::RayHit(const RayHit& other)
{
	this->hit = other.hit;
	this->hitDir = other.hitDir;
	this->hitDist = other.hitDist;
	this->hitNorm = other.hitNorm;
	this->hitObject = other.hitObject;
	this->hitPos = other.hitPos;
}

RayHit& RayHit::operator=(const RayHit& other)
{
	this->hit = other.hit;
	this->hitDir = other.hitDir;
	this->hitDist = other.hitDist;
	this->hitNorm = other.hitNorm;
	this->hitObject = other.hitObject;
	this->hitPos = other.hitPos;
}

Ray::Ray()
{}

Ray::Ray(const Ray& other)
{
	this->dir = other.dir;
	this->orig = other.orig;
}

Ray::Ray(const glm::vec3& dir, const glm::vec3 orig)
{
	this->dir = dir;
	this->orig = orig;
}

Ray& Ray::operator=(const Ray& other)
{
	this->dir = other.dir;
	this->orig = other.orig;
}

RayHit getHit(std::vector<SceneObject*> objects)
{
	RayHit finalHit;
	float clstDst = FLT_MAX;
	for (auto obj : objects)
	{

	}
}