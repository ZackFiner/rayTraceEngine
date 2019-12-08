#include "Light.h"
#include "SceneObjects.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
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

float Light::getBlocked(const glm::vec3& point, const std::vector<SceneObject*>& obj)
{
	//returns a color
	//in the case of an omni light (this one), either the color*intensity or nothing
	glm::vec3 diff = pos - point;
	glm::vec3 lightDir = glm::normalize(diff);
	float distToLight = glm::sqrt(glm::dot(diff, diff));
	Ray lightTest = Ray(lightDir, point + lightDir * LIGHT_EPSILON); // we offset the origin to avoid hitting the same object we're casting from
	auto testResult = lightTest.getHit(obj);
	if (!testResult.hit || testResult.hitDist > distToLight) // 
		return 1.0f;
	return 0.0f;

}

void Light::drawDebug() const
{
	ofPushMatrix();
	ofTranslate(pos);
	ofDrawSphere(glm::vec3(), 0.1f);
	ofDrawAxis(10.0f);
	ofPopMatrix();
}

float AreaLight::getBlocked(const glm::vec3& p, const std::vector<SceneObject*>& obj) {
	/*
	 * Reference: 
	 * Wenzel, J., Pharr, M., Humphreys, G. (2004), Physically Based Rendering: From Theory To Implementation Retrieved from
	 * http://www.pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Sampling_Light_Sources.html
	 */

	glm::vec3 diff = pos - p;
	glm::vec3 lightDir = glm::normalize(diff);
	glm::vec3 up = glm::normalize(glm::cross(-lightDir, glm::vec3(-lightDir.z, -(-lightDir.y+-lightDir.z+-lightDir.x), -lightDir.x))); // hopefully spherical Rand != lightDir
	glm::vec3 right = glm::normalize(glm::cross(up, -lightDir));
	float intensityOnPoint = 0.0f;
	for (int i = 0; i < SAMPLES_PER_LIGHT; i++) {
		auto pt = glm::diskRand(radius); // we'll see how uniform this is
		auto point = right * pt.x + up * pt.y + pos;
		auto testDir = point - p;
		float distToLight = glm::sqrt(glm::dot(testDir, testDir));
		Ray lightTest = Ray(glm::normalize(testDir), p + testDir*LIGHT_EPSILON);
		auto testResult = lightTest.getHit(obj);
		intensityOnPoint += (!testResult.hit || testResult.hitDist > distToLight) ? 1.0f : 0.0f;
	}
	return intensityOnPoint / SAMPLES_PER_LIGHT; // returns the average intensity
}