#pragma once
#include "ofMain.h"
#include "Ray.h"
#include "SceneObjects.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
#define LIGHT_EPSILON 0.1f //if you're seeing spots, play with this value
#define SAMPLES_PER_LIGHT 50
class Light : public SceneObject
{
public:
	glm::vec3 pos;

	glm::vec3 color;
	float intensity;
	Light();
	Light(const glm::vec3&, const glm::vec3&,float);
	Light(const Light&);
	Light& operator=(const Light&);
	virtual glm::vec3 getIntensity(const RayHit& point, const std::vector<SceneObject*>& obj);
	virtual void drawDebug() const;

	glm::vec3 getPos() const { return pos; }
	glm::quat getRot() const { return glm::quat(); }
	virtual void draw() const { drawDebug(); }
	virtual RayHit castRay(const Ray& ray) const { return RayHit(); }
	virtual void setPos(const glm::vec3& newPos) { pos = newPos; }
	virtual void setRot(const glm::vec3& newRot) {}
	virtual glm::vec3 getDiffuse() const { return glm::vec3(); }
	virtual glm::vec3 getSpec() const { return glm::vec3(); }
	virtual void setIntensity(float inten) { intensity = inten; }
	virtual float getBlocked(const glm::vec3& p, const std::vector<SceneObject*>& obj);
	virtual float getBlockedRM(const glm::vec3& p, const std::vector<SceneObject*>& obj);
};

class AreaLight : public Light
{
	float radius = 1.0f;
public:
	AreaLight() : Light() {}
	AreaLight(const glm::vec3& _pos, const glm::vec3& _color, float _intensity, float _radius) :Light(_pos, _color, _intensity) { radius = _radius; }
	float getBlocked(const glm::vec3& p, const std::vector<SceneObject*>& obj);
	float getBlockedRM(const glm::vec3& p, const std::vector<SceneObject*>& obj);
	void setRadius(float _radius) { radius = _radius; }
	float getRadius() const { return radius; }
};