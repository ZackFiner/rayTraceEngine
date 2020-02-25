#include "Shader.h"
#include "SetObject.h"
#include "SceneObjects.h"
#include "Light.h"
#include "RayTracer.h"


ofColor Shaders::getMapColorValue(TextureType t, RayHit Hit) {
	ofColor diff;
	if (Hit.hitObject->getTexture(t) != nullptr) {
		diff = Shaders::sampleTexture(Hit.hitUVSet ? Hit.hitUV : Hit.hitObject->getUV(Hit.hitPos), Hit.hitObject->getTexture(t), true);
	}
	else {
		switch (t) {
		case DIFFUSE_MAP:
			diff = getColFromVec(Hit.hitObject->getDiffuse());
			break;
		case BUMP_MAP: 
			diff = ofColor(127.5, 127.5, 127.5);
			break;
		case SPECULAR_MAP:
			diff = getColFromVec(Hit.hitObject->getSpec());
		}
	}
	return diff;
}
ofColor Shaders::sampleTexture(const glm::vec2& uv, ofImage * tex, bool wrap)
{
	if (!wrap && (uv.x >= 1 || uv.y >= 1 || uv.x < 0 || uv.y < 0))
		return ofColor::black;

	int imgX = (int)(uv.x * tex->getWidth()) % ((int)tex->getWidth());
	int imgY = (int)(tex->getHeight() - uv.y * tex->getHeight()) % ((int)tex->getHeight());
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
	return Shaders::lambert(set, p, norm, diffuse) + (ofColor::white * (specular.getLightness()/255) * glm::min(luminence, 1.0f));
}

ofColor Shaders::phongRM(SetObject* set, const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power, const glm::vec3 &hitDir)
{
	float luminence;
	if (set->lights.size() == 0)
		luminence = 1.0f;
	else
		luminence = 0.0f;

	float phongLum = luminence;
	float lambertLum = luminence;
	for (auto light : set->lights) {
		glm::vec3 lightRay = light->getPos() - p;
		float invR2 = 1.0f / glm::dot(lightRay, lightRay);
		glm::vec3 bisector = glm::normalize(glm::normalize(lightRay) + -hitDir);
		float lightEffect = light->getBlockedRM(p, set->objects);
		lambertLum += (light->intensity * invR2)*glm::max(0.0f, glm::dot(norm, glm::normalize(lightRay)))*lightEffect;
		phongLum += (light->intensity * invR2)*glm::pow(glm::max(0.0f, glm::dot(norm, bisector)), power)*lightEffect;
	}
	return (diffuse * glm::min(lambertLum, 1.0f)) + (ofColor::white * glm::min(phongLum, 1.0f));
}

ofColor Shaders::castRayRec_Phong(SetObject* set, const Ray& ray, int depth, float phongPower)
{
	ofColor pixCol = ofColor::black;
	auto Hit = ray.getHit(set->objects);
	if (Hit.hit)
	{
		ofColor diff = getMapColorValue(DIFFUSE_MAP, Hit);
		ofColor spec = getMapColorValue(SPECULAR_MAP, Hit);
		/*
			* Below, we calculate the adjusted normal using the techniques described in the book
			* below.
			*		Lengyel, E. (2019) Foundations of Game Engine Development Volume 2: Rendering.
			*		Lincoln, California: Terathon Software LLC.
			*/
		glm::vec3 norm = Hit.hitNorm;
		if (Hit.hitObject->getTexture(BUMP_MAP) != nullptr && Hit.hitTangentSet) {
			ofColor bump = getMapColorValue(BUMP_MAP, Hit); // r and g are what we're interested in
			glm::vec3 tangent = glm::vec3(Hit.hitTangent);
			glm::vec3 bitangent = glm::normalize(glm::cross(tangent, Hit.hitNorm))*Hit.hitTangent.w;
			glm::vec3 normal = Hit.hitNorm;
			auto normAdj = (glm::vec3(bump.r, bump.g, bump.b) - glm::vec3(127)) / 127;
			norm = glm::normalize(normAdj*glm::mat3(tangent, bitangent, normal));

		}
		pixCol = Shaders::phong(set, Hit.hitPos, norm, diff, spec, phongPower, Hit.hitDir);
		if (depth < REFLECTION_MAX_DEPTH) {
			//recurse
			auto r = glm::normalize(2 * (glm::dot(norm, -Hit.hitDir))*norm + Hit.hitDir);
			Ray recurseTest = Ray(r, Hit.hitPos+r*LIGHT_EPSILON);
			ofColor result = castRayRec_Phong(set, recurseTest, depth + 1, phongPower)*0.3*(spec.getLightness()/255);
			pixCol = pixCol + result;
		}
	}
	return pixCol;
			
}

ofColor Shaders::castRayRec_RM_Phong(SetObject* set, const Ray& ray, int depth, float phongPower)
{
	// we'll add a bit of ambient occlusion later
	ofColor pixCol = ofColor::black;
	glm::vec3 shadePoint;
	if (ray.rayMarch(set->objects, shadePoint)) { // if we have actually hit some object
		SceneObject* closest = ray.getClosest(shadePoint, set->objects); // get our scene object
		glm::vec3 norm = ray.getNormalRM(shadePoint, set->objects); // get our surface normal
		auto diff = getColFromVec(closest->getDiffuse());
		auto spec = getColFromVec(closest->getSpec());
		pixCol = Shaders::phongRM(set, shadePoint, norm, diff, spec, phongPower, ray.getDir());
		if (depth < REFLECTION_MAX_DEPTH) {
			//recurse
			auto r = glm::normalize(2 * (glm::dot(norm, -ray.getDir()))*norm + ray.getDir());
			Ray recurseTest = Ray(r, shadePoint + r * LIGHT_EPSILON);
			ofColor result = castRayRec_RM_Phong(set, recurseTest, depth + 1, phongPower)*0.3*(spec.getLightness() / 255);
			pixCol = pixCol + result;
		}
	}
	return pixCol;
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
				ofColor diff = getMapColorValue(DIFFUSE_MAP, Hit);
				pixCol += Shaders::lambert(set, Hit.hitPos, Hit.hitNorm, diff);
			}
			img.setColor(j, i, pixCol);
		}
	}
}
void Shaders::renderPhongImage(SetObject* set, glm::vec2 dim, ofImage& img, float phongPower)
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
			/*auto Hit = PxRay.getHit(set->objects);
			if (Hit.hit)
			{
				ofColor diff = getMapColorValue(DIFFUSE_MAP, Hit);
				ofColor spec = getMapColorValue(SPECULAR_MAP, Hit);

				glm::vec3 norm = Hit.hitNorm;
				if (Hit.hitObject->getTexture(BUMP_MAP) != nullptr && Hit.hitTangentSet) {
					ofColor bump = getMapColorValue(BUMP_MAP, Hit); // r and g are what we're interested in
					glm::vec3 tangent = glm::vec3(Hit.hitTangent);
					glm::vec3 bitangent = glm::normalize(glm::cross(tangent, Hit.hitNorm))*Hit.hitTangent.w;
					glm::vec3 normal = Hit.hitNorm;
					auto normAdj = (glm::vec3(bump.r, bump.g, bump.b)-glm::vec3(127)) / 127;
					norm = glm::normalize(normAdj*glm::mat3(tangent, bitangent, normal));

				}
				pixCol = Shaders::phong(set, Hit.hitPos, norm, diff, spec, phongPower, Hit.hitDir);
			}*/
			pixCol = Shaders::castRayRec_Phong(set, PxRay, 0, phongPower);
			img.setColor(j, i, pixCol);
		}
	}
}


void Shaders::renderPhongSubRegion(SetObject* set, glm::vec2 dim, glm::vec2 subRegion_start, glm::vec2 subRegion_dim, ofImage& img, float phongPower)
{
	int startX = (int)(subRegion_start.x);
	int startY = (int)(subRegion_start.y);
	int width = (int)subRegion_dim.x + startX;
	int height = (int)subRegion_dim.y + startY;
	
	for (int i = startY; i < height; i++)
	{
		for (int j = startX; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = set->cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			pixCol = Shaders::castRayRec_Phong(set, PxRay, 0, phongPower);
			img.setColor(j, i, pixCol);
		}
	}
}

void Shaders::renderPhongRMSubRegion(SetObject* set, glm::vec2 dim, glm::vec2 subRegion_start, glm::vec2 subRegion_dim, ofImage& img, float phongPower)
{
	int startX = (int)(subRegion_start.x);
	int startY = (int)(subRegion_start.y);
	int width = (int)subRegion_dim.x + startX;
	int height = (int)subRegion_dim.y + startY;
	
	for (int i = startY; i < height; i++)
	{
		for (int j = startX; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = set->cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			pixCol = Shaders::castRayRec_RM_Phong(set, PxRay, 0, phongPower);
			img.setColor(j, i, pixCol);
		}
	}
}