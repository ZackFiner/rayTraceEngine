#include "RayTracer.h"

ofColor getColFromVec(const glm::vec3& col)
{
	return ofColor(255*col.x, 255*col.y, 255*col.z);
}

ofImage RayTracer::testRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim, ofImage& canvas)
{
	int width = (int)dim.x;
	int height = (int)dim.y;
	canvas.allocate(width, height, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = cam.getRay(glm::vec2(j, i), dim);
			auto Hit = PxRay.getHit(set);
			if (Hit.hit)
				pixCol = getColFromVec(Hit.hitObject->getDiffuse());
			canvas.setColor(j, i, pixCol);
		}
	}

	return ofImage();
}
ofImage RayTracer::lambertRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim, ofImage& canvas)
{
	glm::vec3 ambientCol = glm::vec3(0.1, 0.1, 0.1);
	int width = (int)dim.x;
	int height = (int)dim.y;
	canvas.allocate(width, height, ofImageType::OF_IMAGE_COLOR);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			ofColor pixCol = ofColor::black;
			auto PxRay = cam.getRay(glm::vec2(j, i), dim);
			auto Hit = PxRay.getHit(set);
			if (Hit.hit)
			{
				pixCol = getColFromVec(Hit.hitObject->getDiffuse());
				glm::vec3 col = ambientCol;
				for (auto light : lighting)
				{
					float I = light->intensity;
					glm::vec3 diff = light->pos - Hit.hitPos;
					float r2 = 1.0f/glm::dot(diff, diff);
					auto returnedI = light->getIntensity(Hit, set);
					//std::cout << returnedI << std::endl;
					col += Hit.hitObject->getDiffuse() * (I*r2) * glm::max(0.0f, glm::dot(Hit.hitNorm, glm::normalize(diff))) * glm::clamp(returnedI.x, 0.0f, 1.0f);
				}
				pixCol = getColFromVec(col);
			}
			canvas.setColor(j, i, pixCol);
		}
	}
	return ofImage();
}
ofImage RayTracer::phongRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim, ofImage& canvas)
{
	return ofImage();
}