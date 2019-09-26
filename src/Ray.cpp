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

glm::vec3 Ray::getDir() const { return dir; }
glm::vec3 Ray::getOrig() const { return orig; }

RayHit Ray::getHit(const std::vector<SceneObject*>& objects) const
{
	RayHit finalHit;
	float clstDst = FLT_MAX;
	float sqrdDist;
	for (auto obj : objects)
	{
		RayHit cur = obj->castRay(*this);
		if (cur.hit)
		{
			if (cur.hitDist != RAY_NOHIT_DIST)
			{
				sqrdDist = cur.hitDist * cur.hitDist;
			}
			else
			{
				glm::vec3 diff = cur.hitPos - this->orig;
				sqrdDist = glm::dot(diff, diff);
			}
			
			if (sqrdDist < clstDst)
			{
				finalHit = cur;
				clstDst = sqrdDist;
			}
		}
	}

	if (finalHit.hit && finalHit.hitDist == RAY_NOHIT_DIST)
	{
		finalHit.hitDist = glm::sqrt(sqrdDist);
	}

	return finalHit; // default constructor builds a NO-HIT
}

RayCam::RayCam()
{
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->right = glm::vec3(1.0f,0.0f, 0.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->origin = glm::vec3(0.0f,0.0f,0.0f);
}
RayCam::RayCam(const glm::vec3& f, const glm::vec3& r, const glm::vec3& u, const glm::vec3& o)
{
	this->front = f;
	this->right = r;
	this->up = u;
	this->origin = o;
}
Ray RayCam::getRay(const glm::vec2& coord, const glm::vec2& dim)
{
	glm::vec2 scCoord = coord - dim / 2;
	
	float sX = 1.0f / dim.x;
	float sY = 1.0f / dim.y;
	
	glm::vec3 planePos = scCoord.x*right + scCoord.y*up + front * focalDist;
	glm::vec3 dir = glm::normalize(planePos - origin);
	
	return Ray(dir, origin);
}
void RayCam::setPos(const glm::vec3& newPos) { origin = newPos; }
void RayCam::rotate(const glm::quat& rot)
{
	front = glm::normalize(rot * front);
	right = glm::normalize(rot * right);
	up = glm::normalize(rot * up);
}