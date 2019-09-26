#include "SceneObjects.h"
#include "Ray.h"
Sphere::Sphere(const glm::vec3& origin, const glm::vec3& diffuse, const glm::vec3& specular, float radius, Shader* shader)
{
	this->pos = origin;
	this->c_diff = diffuse;
	this->c_spec = specular;
	this->radius = radius;
	this->shade = shader;
}
glm::vec3 Sphere::getPos() const { return pos; }
glm::quat Sphere::getRot() const { return glm::quat(); }
void Sphere::draw()
{
	ofPushMatrix();
	ofTranslate(pos);
	ofNoFill();
	ofSetColor(ofColor(c_diff.x, c_diff.y, c_diff.z));
	ofDrawSphere(radius);
	ofFill();
	ofPopMatrix();
}
RayHit Sphere::castRay(const Ray& ray) const
{
	glm::vec3 norm, hit;
	RayHit _return = RayHit();
	if (_return.hit = glm::intersectRaySphere(ray.getOrig(), ray.getDir(), pos, radius, hit, norm))
	{
		_return.hitDir = ray.getDir();
		_return.hitPos = hit;
		_return.hitNorm = norm;
		_return.hitObject = (SceneObject*)this;
	}
	return _return;
}
Shader* Sphere::getShader()
{
	return shade;
}
void Sphere::setPos(const glm::vec3& newPos)
{
	pos = newPos;
}
void Sphere::setRot(const glm::vec3& newRot) {}
glm::vec3 Sphere::getDiffuse() const { return c_diff; }
glm::vec3 Sphere::getSpec() const { return c_spec; }
Plane::Plane(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& diffuse, const glm::vec3& spec, Shader* shade)
{
	this->pos = pos;
	this->norm = norm;
	this->c_diff = diffuse;
	this->c_spec = spec;
	this->shade = shade;
}
glm::vec3 Plane::getPos() const { return pos; }
glm::quat Plane::getRot() const { return glm::quat(); }

void Plane::draw()
{
	//TODO: implement me
}
RayHit Plane::castRay(const Ray& ray) const
{
	RayHit _return = RayHit();
	float dist;
	if (_return.hit = glm::intersectRayPlane(ray.getOrig(), ray.getDir(), pos, this->norm, dist))
	{
		_return.hitDir = ray.getDir();
		_return.hitPos = dist*ray.getDir()+ray.getOrig();
		_return.hitNorm = this->norm;
		_return.hitObject = (SceneObject*)this;
		_return.hitDist = dist;
	}
	return _return;
}
Shader* Plane::getShader() { return shade; }
void Plane::setPos(const glm::vec3& newPos) { pos = newPos; }
void Plane::setRot(const glm::vec3& newRot) {}
glm::vec3 Plane::getDiffuse() const { return c_diff; }
glm::vec3 Plane::getSpec() const { return c_spec; }
