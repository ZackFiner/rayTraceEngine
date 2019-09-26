#pragma once
#include "ofMain.h"
class Ray;
class RayHit;
class Shader;
class SceneObject
{
public: 
	virtual glm::vec3 getPos() = 0;
	virtual glm::quat getRot() = 0;
	virtual void draw() const = 0;
	virtual RayHit castRay(const Ray& ray) const = 0;
	virtual void setPos(const glm::vec3& newPos) = 0;
	virtual void setRot(const glm::vec3& newRot) = 0;
};

class Sphere : public SceneObject
{
	glm::vec3 pos;
	glm::vec3 c_diff, c_spec;
	Shader* shade;
public:
	glm::vec3 getPos();
	glm::quat getRot();
	void draw();
	RayHit castRay();
	void setPos();
	void setRot();
};