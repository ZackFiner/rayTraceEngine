#include "Light.h"
#include "SceneObjects.h"

Light::Light() {
	this->intensity = -1.0f;
}
Light::Light(const glm::vec3& pos, const glm::vec3& color, float intensity)
{
	this->pos = pos;
	this->color = color;
	this->intensity = intensity;
}
Light::Light(const Light& o)
{
	this->pos = o.pos;
	this->color = o.color;
	this->intensity = o.intensity;
}
Light& Light::operator=(const Light& o)
{
	this->pos = o.pos;
	this->color = o.color;
	this->intensity = o.intensity;
	return *this;
}
glm::vec3 Light::getIntensity(const RayHit& point, const std::vector<SceneObject*>& obj)
{
	//returns a color
	//in the case of an omni light (this one), either the color*intensity or nothing
	glm::vec3 diff = pos - point.hitPos;
	glm::vec3 lightDir = glm::normalize(diff);
	float distToLight = glm::sqrt(glm::dot(diff, diff));
	Ray lightTest = Ray(lightDir, point.hitPos + lightDir * LIGHT_EPSILON); // we offset the origin to avoid hitting the same object we're casting from
	auto testResult = lightTest.getHit(obj);
	if (!testResult.hit || testResult.hitDist > distToLight) // 
		return color * intensity;
	return glm::vec3();
	
}