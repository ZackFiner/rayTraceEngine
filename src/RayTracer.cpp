#include "RayTracer.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
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
			auto PxRay = cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			auto Hit = PxRay.getHit(set);
			if (Hit.hit)
				pixCol = getColFromVec(Hit.hitObject->getDiffuse());
			canvas.setColor(j, i, pixCol);
		}
	}

	return canvas;
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
			auto PxRay = cam.getRay(glm::vec2(((float)j+0.5f)/dim.x, ((float)i+0.5f)/dim.y));
			//std::cout << PxRay.getDir() << std::endl;
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
				pixCol = getColFromVec(glm::clamp(col, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f))); // need to clamp so our values to explode
			}
			canvas.setColor(j, i, pixCol);
		}
	}
	return canvas;
}

ofImage RayTracer::phongRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim, ofImage& canvas)
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
			auto PxRay = cam.getRay(glm::vec2(((float)j + 0.5f) / dim.x, ((float)i + 0.5f) / dim.y));
			//std::cout << PxRay.getDir() << std::endl;
			auto Hit = PxRay.getHit(set);
			if (Hit.hit)
			{
				pixCol = getColFromVec(Hit.hitObject->getDiffuse());
				glm::vec3 col = ambientCol;
				for (auto light : lighting)
				{
					float I = light->intensity;
					glm::vec3 diff = light->pos - Hit.hitPos;
					float r2 = 1.0f / glm::dot(diff, diff);
					auto returnedI = light->getIntensity(Hit, set);
					glm::vec3 bisector = glm::normalize(glm::normalize(diff) + -Hit.hitDir); //we add the negative of hitDir because it needs to be pointing towards the view
					//std::cout << returnedI << std::endl;
					col += (Hit.hitObject->getDiffuse() * (I*r2) * glm::max(0.0f, glm::dot(Hit.hitNorm, glm::normalize(diff))) +
						   Hit.hitObject->getSpec() * (I*r2) * glm::pow(glm::max(0.0f, glm::dot(Hit.hitNorm, bisector)), 50.0f))* glm::clamp(returnedI.x, 0.0f, 1.0f);
				}
				pixCol = getColFromVec(glm::clamp(col, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))); // need to clamp so our values to explode
			}
			canvas.setColor(j, i, pixCol);
		}
	}
	return canvas;
}
