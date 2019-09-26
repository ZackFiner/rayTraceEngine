#pragma once
#include "ofMain.h"
class Ray;
class RayHit;
class Shader;
class SceneObject
{
public: 
	virtual glm::vec3 getPos() const = 0;
	virtual glm::quat getRot() const = 0;
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
	float radius;
public:
	Sphere(const glm::vec3&, const glm::vec3&, const glm::vec3&, float, Shader*);
	glm::vec3 getPos() const;
	glm::quat getRot() const;
	void draw();
	RayHit castRay(const Ray& ray) const;
	Shader* getShader();
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
};

class Plane : public SceneObject
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 c_diff, c_spec;
	Shader* shade;
public:
	Plane(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, Shader*);
	glm::vec3 getPos() const;
	glm::quat getRot() const;
	void draw();
	RayHit castRay(const Ray& ray) const;
	Shader* getShader();
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
};