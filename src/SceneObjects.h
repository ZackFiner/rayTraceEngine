#pragma once
#include "ofMain.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
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
	virtual glm::vec3 getDiffuse() const = 0;
	virtual glm::vec3 getSpec() const = 0;
	virtual glm::vec2 getUV(const glm::vec3& v) const { return glm::vec2(); }
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
	void draw() const;
	RayHit castRay(const Ray& ray) const;
	Shader* getShader();
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpec() const;
	glm::vec2 getUV(const glm::vec3& v) const;
};

class Plane : public SceneObject
{
protected:
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 c_diff, c_spec;
	Shader* shade;
public:
	Plane(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, Shader*);
	glm::vec3 getPos() const;
	glm::quat getRot() const;
	void draw() const;
	RayHit castRay(const Ray& ray) const;
	Shader* getShader();
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpec() const;
};

class FinitePlane :public Plane
{
	float roll = 0.0f;
	glm::vec2 bounds = glm::vec2(1000,1000);
	glm::vec2 textureWrap = glm::vec2(6,6);
public:
	FinitePlane(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, Shader*, float, const glm::vec2& dim);
	RayHit castRay(const Ray& ray) const;
	glm::vec2 getUV(const glm::vec3& v) const;
	void draw() const;
};