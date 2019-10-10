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
	glm::vec3 getIntensity(const RayHit& point, const std::vector<SceneObject*>& obj);
	void drawDebug() const;

	glm::vec3 getPos() const { return pos; }
	glm::quat getRot() const { return glm::quat(); }
	void draw() const { drawDebug(); }
	RayHit castRay(const Ray& ray) const { return RayHit(); }
	void setPos(const glm::vec3& newPos) { pos = newPos; }
	void setRot(const glm::vec3& newRot) {}
	glm::vec3 getDiffuse() const { return glm::vec3(); }
	glm::vec3 getSpec() const { return glm::vec3(); }
	float getBlocked(const glm::vec3& p, const std::vector<SceneObject*>& obj);
};