#pragma once
#include "ofMain.h"
#define RAY_NOHIT_DIST -1.0f
#define MAX_RAY_STEPS 200
#define DIST_THRESHOLD 0.01f
#define MAX_DISTANCE 1000.0f
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
class SceneObject;
class RayHit
{
public:
	glm::vec3 hitPos, hitNorm, hitDir;
	glm::vec4 hitTangent;
	glm::vec2 hitUV;
	bool hitUVSet = false;
	bool hitTangentSet = false;
	int hitTriIndex;
	float hitDist = RAY_NOHIT_DIST;
	bool hit = false;
	SceneObject* hitObject = nullptr;

	RayHit();
	RayHit(const RayHit&);
	RayHit& operator=(const RayHit&);
};

class Ray
{
	glm::vec3 dir, orig;

	
public:
	Ray();
	Ray(const Ray&);
	Ray(const glm::vec3& dir, const glm::vec3 orig);
	glm::vec3 getDir() const;
	glm::vec3 getOrig() const;
	Ray& operator=(const Ray&);
	RayHit getHit(const std::vector<SceneObject*>&) const;

	// FOR RAYMARCHING
	float sceneSDF(const glm::vec3& p, const std::vector<SceneObject*>&) const;
	bool rayMarch(const std::vector<SceneObject*>&, glm::vec3& shadePoint) const;
	glm::vec3 getNormalRM(const glm::vec3 &p, const std::vector<SceneObject*>&) const;
	SceneObject* getClosest(const glm::vec3& p, const std::vector<SceneObject*>&) const;

};

class RayCam
{
	glm::vec3 origin;
	glm::vec3  front, up, right;
	float aspectRatio = 16/9; // = width / height
	float invAspectRatio = 9/16;
	float focalDist = 1000.0f;
	
public:
	RayCam();
	glm::quat orientation;
	RayCam(const glm::vec3& f, const glm::vec3& r, const glm::vec3& u, const glm::vec3& o);
	Ray getRay(const glm::vec2& coord, const glm::vec2& dim) const;
	Ray getRay(const glm::vec2& uv) const;
	void setPos(const glm::vec3& newPos);
	void rotate(const glm::quat& rot);
	void setFOV(float);
	void setAspectRatio(const glm::vec2& aspec);
	void setAspectRatio(float);
	void drawDebug() const;
	glm::vec3 getUp() const;
};