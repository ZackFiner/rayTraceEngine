#include "RayTracer.h"

ofColor getColFromVec(const glm::vec3& col)
{
	return ofColor(col.x, col.y, col.z);
}

ofImage RayTracer::testRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim)
{
	int width = (int)dim.x;
	int height = (int)dim.y;
	ofImage canvas = ofImage();
	canvas.resize(width, height);
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
ofImage RayTracer::lambertRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim)
{
	return ofImage();
}
ofImage RayTracer::phongRayTracer(const RayCam& cam, const std::vector<SceneObject*>& set, const std::vector<Light*>& lighting, const glm::vec2& dim)
{
	return ofImage();
}