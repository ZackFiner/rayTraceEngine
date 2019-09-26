#pragma once
#include "ofMain.h"
class Ray;
class RayHit;
class SceneObject
{
public: 
	virtual glm::vec3 getPos() = 0;
	virtual glm::quat getRot() = 0;
	virtual void draw() const = 0;
	virtual RayHit castRay(const Ray& ray) const = 0;
	virtual void setPos(const glm::vec3& newPos) = 0;
	virtual void setPos(const glm::vec3& newRot) = 0;
};

