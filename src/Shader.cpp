#include "Shader.h"

ofColor Shader::lambert(	const glm::vec3 &p,
							const glm::vec3 &norm,
							const ofColor diffuse)
{
	float luminence = ambientBase;
	/*if (lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;*/
	for (auto light : lights)
	{
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		luminence += (light->intensity * invR2)*glm::max(0.0f, glm::dot(norm, glm::normalize(lightRay)))*light->getBlocked(p, set);
	}
	return diffuse * glm::min(luminence, 1.0f);

}
ofColor Shader::phong(	const glm::vec3 &p,
						const glm::vec3 &norm,
						const ofColor diffuse,
						const ofColor specular,
						float power,
						const glm::vec3 &hitDir)
{
	float luminence;
	if (lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;
	for (auto light : lights)
	{
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		glm::vec3 bisector = glm::normalize(glm::normalize(lightRay) + -hitDir);
		luminence += (light->intensity * invR2)*glm::pow(glm::max(0.0f, glm::dot(norm, bisector)), power)*light->getBlocked(p, set);
	}
	return lambert(p, norm, diffuse) + specular * glm::min(luminence, 1.0f);
}