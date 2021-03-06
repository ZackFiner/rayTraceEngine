#include "Ray.h"
#include "SceneObjects.h"
#include <limits>
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
RayHit::RayHit()
{}

RayHit::RayHit(const RayHit& other)
{
	this->hit = other.hit;
	this->hitDir = other.hitDir;
	this->hitDist = other.hitDist;
	this->hitNorm = other.hitNorm;
	this->hitObject = other.hitObject;
	this->hitPos = other.hitPos;
	this->hitTriIndex = other.hitTriIndex;
	this->hitUV = other.hitUV;
	this->hitUVSet = other.hitUVSet;
	this->hitTangent = other.hitTangent;
	this->hitTangentSet = other.hitTangentSet;
}

RayHit& RayHit::operator=(const RayHit& other)
{
	this->hit = other.hit;
	this->hitDir = other.hitDir;
	this->hitDist = other.hitDist;
	this->hitNorm = other.hitNorm;
	this->hitObject = other.hitObject;
	this->hitPos = other.hitPos;
	this->hitTriIndex = other.hitTriIndex;
	this->hitUV = other.hitUV;
	this->hitUVSet = other.hitUVSet;
	this->hitTangent = other.hitTangent;
	this->hitTangentSet = other.hitTangentSet;
	return *this;
}

Ray::Ray()
{}

Ray::Ray(const Ray& other)
{
	this->dir = other.dir;
	this->orig = other.orig;
}

Ray::Ray(const glm::vec3& dir, const glm::vec3 orig)
{
	this->dir = dir;
	this->orig = orig;
}

Ray& Ray::operator=(const Ray& other)
{
	this->dir = other.dir;
	this->orig = other.orig;
	return *this;
}

glm::vec3 Ray::getDir() const { return dir; }
glm::vec3 Ray::getOrig() const { return orig; }

RayHit Ray::getHit(const std::vector<SceneObject*>& objects) const
{
	RayHit finalHit;
	float clstDst = FLT_MAX;
	float sqrdDist;
	for (auto obj : objects)
	{
		RayHit cur = obj->castRay(*this);
		if (cur.hit)
		{
			if (cur.hitDist != RAY_NOHIT_DIST)
			{
				sqrdDist = cur.hitDist * cur.hitDist;
			}
			else
			{
				glm::vec3 diff = cur.hitPos - this->orig;
				sqrdDist = glm::dot(diff, diff);
			}
			
			if (sqrdDist < clstDst)
			{
				finalHit = cur;
				clstDst = sqrdDist;
			}
		}
	}

	if (finalHit.hit && finalHit.hitDist == RAY_NOHIT_DIST)
	{
		finalHit.hitDist = glm::sqrt(clstDst);
	}

	return finalHit; // default constructor builds a NO-HIT
}

float Ray::sceneSDF(const glm::vec3& p, const std::vector<SceneObject*>& objs)  const {
	float minSDF = FLT_MAX;
	for (auto object : objs) {
		float sdist = object->sdf(p);
		if (sdist < minSDF) {
			minSDF = sdist;
		}
	}
	return minSDF;
}

SceneObject* Ray::getClosest(const glm::vec3& p, const std::vector<SceneObject*>& objs) const {
	float minSDF = FLT_MAX;
	SceneObject* closest = nullptr;
	for (auto object : objs) {
		float sdist = object->sdf(p);
		if (sdist < minSDF) {
			minSDF = sdist;
			closest = object;
		}
	}
	return closest;
}

bool Ray::rayMarch(const std::vector<SceneObject*>& objs, glm::vec3& shadePoint, RayHit* hitContainer) const {
	bool hit = false;
	bool debug_prematureexit = false;
	shadePoint = orig;
	float dist;
	for (int i = 0; i < MAX_RAY_STEPS; i++)
	{
		dist = sceneSDF(shadePoint, objs);
		if (glm::abs(dist) < DIST_THRESHOLD) { // if we're close enough to a surface
			hit = true; // we hit something, we're done
			break;
		}
		else if (glm::abs(dist) > MAX_DISTANCE) { // if we're way to far away from any surface
			debug_prematureexit = true;
			break; // don't bother testing anymore
		}
		else { // otherwise
			shadePoint = shadePoint + dir * dist; // march the ray ahead along the closest distance
		}
	}
	return hit;
}

/*
 * We can generate a very good approxmiation of the surface normal by simply testing how much the surface changes
 * an arbitaray distance (eps - or epsilon) along each world axis from our starting point, and using these to build a normal vector.
 * Essentially, we are approximating the gradient of the surface at this point using our SDF
 * (Finally, something from Calculus III comes in handy).
 */
glm::vec3 Ray::getNormalRM(const glm::vec3& p, const std::vector<SceneObject*>& objs) const {
	float eps = 0.001f;
	float dp = sceneSDF(p, objs);
	glm::vec3 norm = glm::vec3( dp - sceneSDF(glm::vec3(p.x - eps, p.y, p.z), objs),
								dp - sceneSDF(glm::vec3(p.x, p.y - eps, p.z), objs),
								dp - sceneSDF(glm::vec3(p.x, p.y, p.z - eps), objs));
	return glm::normalize(norm);

}

RayCam::RayCam()
{
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->right = glm::vec3(1.0f,0.0f, 0.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->origin = glm::vec3(0.0f,0.0f,0.0f);
}
RayCam::RayCam(const glm::vec3& f, const glm::vec3& r, const glm::vec3& u, const glm::vec3& o)
{
	this->front = f;
	this->right = r;
	this->up = u;
	this->origin = o;
}
Ray RayCam::getRay(const glm::vec2& coord, const glm::vec2& dim) const
{
	glm::vec2 scCoord = coord - dim / 2;
	
	float sX = 1.0f / dim.x;
	float sY = 1.0f / dim.y;
	
	glm::vec3 planePos = -scCoord.x*(orientation*right) + -scCoord.y*(orientation*up) + (orientation * front) * focalDist;
	glm::vec3 dir = glm::normalize(planePos - origin);
	
	return Ray(dir, origin);
}
Ray RayCam::getRay(const glm::vec2& uv) const
{
	/*
	UV coordinates are useful, as they allow use to define a coordinate system
	for some image without needing to know it's dimensions or aspect ratio.
	
	We will assume that these uvs are in Normalized device coordinates, that is
	the top lefthand corner of the image will be passed as (0,0)
	and the bottom righthand corner will be (1,1). thus the domain
	of u and v are both [0, 1]

	this makes the opposite length of the triangle = 1

	*/
	/*
	we need to adjust our UVs to be in our view plane's coordinate space.
	The edges of our plane are at -1 and 1 respectively
	*/
	glm::vec2 planeSpace = (uv - glm::vec2(0.5f, 0.5f))*2;
	planeSpace.x = planeSpace.x*aspectRatio;//next, we adjust for the aspect ratio
	glm::vec3 planePos = -planeSpace.x*(orientation*right) + -planeSpace.y*(orientation*up) + (orientation * front) * focalDist;
	glm::vec3 dir = glm::normalize(planePos);

	return Ray(dir, origin);
}

void RayCam::setFOV(float ang)
{
	//we assume that the view plane is 2 units wide
	//we also assume this angle is in degrees

	float halfAngRad = glm::radians(ang*0.5f);
	/*
		next, we simply need to update our focal distance s.t. the 
		angle made between a line to the top edge of our plane (a hypotenus)
		and the line to the center of the view plane (an adjacent edge)
		is equal to half the angle passed.

		the opposite edge of our view plane is assumed to be constant:
		1.0f
		now, let's apply a bit of trig. Tan(a) = opp/adj
		we need to solve for adj, given an angl and an opp of 1.0f
		Tan(a)*adj = 1.0f
		adj = 1.0f*cot(a)
		adj = cot(a)
	*/
	focalDist = glm::cot(halfAngRad);
}

void RayCam::setAspectRatio(const glm::vec2& aspec)
{
	aspectRatio = aspec.x / aspec.y;
	invAspectRatio = aspec.y / aspec.x;
}

void RayCam::setAspectRatio(float ratio)
{
	aspectRatio = ratio;
	invAspectRatio = 1.0f/ratio;
}
void RayCam::setPos(const glm::vec3& newPos) { origin = newPos; }
void RayCam::rotate(const glm::quat& rot)
{
	//front = glm::normalize(rot * front);
	//right = glm::normalize(rot * right);
	//up = glm::normalize(rot * up);
	orientation = rot*orientation;
}
glm::vec3 RayCam::getUp() const { return up*orientation; }
void RayCam::drawDebug() const
{

}