#include "Shader.h"
#include "SetObject.h"
#include "SceneObjects.h"
#include "Light.h"
#include "RayTracer.h"
//ofColor Shaders::lambert(	const glm::vec3 &p,
//							const glm::vec3 &norm,
//							const ofColor diffuse)
//{
//	float luminence = ambientBase;
//	/*if (lights.size() == 0)
//		luminence = 1.0f;
//	else
//		luminence = 0.0f;*/
//	for (auto light : lights)
//	{
//		glm::vec3 lightRay = light->getPos() - p;
//		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
//		luminence += (light->intensity * invR2)*glm::max(0.0f, glm::dot(norm, glm::normalize(lightRay)))*light->getBlocked(p, set);
//	}
//	return diffuse * glm::min(luminence, 1.0f);
//
//}
//ofColor Shaders::phong(	const glm::vec3 &p,
//						const glm::vec3 &norm,
//						const ofColor diffuse,
//						const ofColor specular,
//						float power,
//						const glm::vec3 &hitDir)
//{
//	float luminence;
//	if (lights.size() == 0)
//		luminence = 1.0f;
//	else
//		luminence = 0.0f;
//	for (auto light : lights)
//	{
//		glm::vec3 lightRay = light->getPos() - p;
//		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
//		glm::vec3 bisector = glm::normalize(glm::normalize(lightRay) + -hitDir);
//		luminence += (light->intensity * invR2)*glm::pow(glm::max(0.0f, glm::dot(norm, bisector)), power)*light->getBlocked(p, set);
//	}
//	return lambert(p, norm, diffuse) + specular * glm::min(luminence, 1.0f);
//}

ofColor Shaders::sampleTexture(const glm::vec2& uv, ofImage * tex, bool wrap)
{
	if (!wrap && (uv.x >= 1 || uv.y >= 1 || uv.x < 0 || uv.y < 0))
		return ofColor::black;

	int imgX = (int)(uv.x * tex->getWidth()) % ((int)tex->getWidth());
	int imgY = (int)(uv.y * tex->getHeight()) % ((int)tex->getHeight());
	if (imgX < 0) imgX += tex->getWidth();
	if (imgY < 0) imgY += tex->getHeight();
	return tex->getColor(imgX, imgY);
}
ofColor Shaders::lambert(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse)
{
	float luminence = set->ambientBase;
	
	/*if (lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;*/
	for (auto light : set->lights)
	{
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		luminence += (light->intensity * invR2)*glm::max(0.0f, glm::dot(norm, glm::normalize(lightRay)))*light->getBlocked(p, set->objects);
	}
	return diffuse * glm::min(luminence, 1.0f);

}
ofColor Shaders::phong(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir)
{
	float luminence;
	if (set->lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;
	for (auto light : set->lights)
	{
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		glm::vec3 bisector = glm::normalize(glm::normalize(lightRay) + -hitDir);
		luminence += (light->intensity * invR2)*glm::pow(glm::max(0.0f, glm::dot(norm, bisector)), power)*light->getBlocked(p, set->objects);
	}
	return Shaders::lambert(set, p, norm, diffuse) + specular * glm::min(luminence, 1.0f);
}

void Shaders::renderLambertImage(SetObject* set, glm::vec2 dim, ofImage& img)
{
	int width = (int)dim.x;
	int height = (int)dim.y;
	img.allocate(width, height, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = set->cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			auto Hit = PxRay.getHit(set->objects);
			if (Hit.hit)
			{
				ofColor diff = Hit.hitObject->getTexture() != nullptr ? (Shaders::sampleTexture(Hit.hitObject->getUV(Hit.hitPos), Hit.hitObject->getTexture(), true)) : (getColFromVec(Hit.hitObject->getDiffuse()));
				pixCol += Shaders::lambert(set, Hit.hitPos, Hit.hitNorm, diff);
			}
			img.setColor(j, i, pixCol);
		}
	}
}
void Shaders::renderPhongImage(SetObject* set, glm::vec2 dim, ofImage& img)
{
	int width = (int)dim.x;
	int height = (int)dim.y;

	float phongPower = 150.0f;

	img.allocate(width, height, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = set->cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			auto Hit = PxRay.getHit(set->objects);
			if (Hit.hit)
			{
				ofColor diff = Hit.hitObject->getTexture() != nullptr ? (Shaders::sampleTexture(Hit.hitObject->getUV(Hit.hitPos), Hit.hitObject->getTexture(), true)) : (getColFromVec(Hit.hitObject->getDiffuse()));
				pixCol = Shaders::phong(set, Hit.hitPos, Hit.hitNorm, diff, getColFromVec(Hit.hitObject->getSpec()), phongPower, Hit.hitDir);
			}
			img.setColor(j, i, pixCol);
		}
	}
}
