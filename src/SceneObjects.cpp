#include "SceneObjects.h"
#include "Ray.h"

/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
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
void Sphere::draw() const
{
	ofPushMatrix();
	ofTranslate(pos);
	ofNoFill();
	ofSetColor(ofColor(c_diff.x*255, c_diff.y*255, c_diff.z*255));
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

void Plane::draw() const {
	auto plane = ofPlanePrimitive();
	plane.set(10000,10000);
	ofPushMatrix();
	ofTranslate(this->getPos());
	auto rot = glm::angleAxis(glm::acos(glm::dot(this->norm, glm::vec3(0.0f, 0.0f, 1.0f))), glm::cross(this->norm, glm::vec3(0.0f, 0.0f, 1.0f)));
	auto eul = glm::eulerAngles(rot);
	ofRotateX(glm::degrees(eul.x));
	ofRotateY(glm::degrees(eul.y));
	ofRotateZ(glm::degrees(eul.z));
	ofSetColor(ofColor(c_diff.x * 255, c_diff.y * 255, c_diff.z * 255));
	plane.drawWireframe();
	ofPopMatrix();
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


FinitePlane::FinitePlane(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& diffuse, const glm::vec3& spec, Shader* shade, float roll, const glm::vec2& bounds)
	:Plane(pos,norm,diffuse,spec,shade)
{
	this->roll = roll;
	this->bounds = bounds;
}

RayHit FinitePlane::castRay(const Ray& ray) const
{
	RayHit hit = Plane::castRay(ray);//first we use our super class

	/*
		Next, we simply check to see if the hit point is in the bounderies of
		the plane we've defined
	*/
	if (hit.hit) {
		auto flatHit = hit.hitPos*glm::angleAxis(glm::acos(glm::dot(this->norm, glm::vec3(0.0f, 1.0f, 0.0f))), glm::cross(this->norm, glm::vec3(0.0f, 1.0f, 0.0f)));
		flatHit = glm::rotate(flatHit, -glm::radians(roll), glm::vec3(0.0f, 1.0f, 0.0f)); // adjust for roll
		flatHit = flatHit - this->getPos(); // we center it at origin

		if (glm::abs(flatHit.x) <= bounds.x / 2 && glm::abs(flatHit.z) <= bounds.y / 2)
			return hit;
	}
	return RayHit();//otherwise, we say we didn't hit the plane.
}

void FinitePlane::draw() const {
	auto plane = ofPlanePrimitive();
	plane.set(bounds.x, bounds.y);
	ofPushMatrix();

	auto rot = glm::angleAxis(glm::acos(glm::dot(this->norm, glm::vec3(0.0f, 0.0f, 1.0f))), glm::cross(this->norm, glm::vec3(0.0f, 0.0f, 1.0f)));
	auto eul = glm::eulerAngles(rot);
	ofRotateX(glm::degrees(eul.x));
	ofRotateY(glm::degrees(eul.y));
	ofRotateZ(glm::degrees(eul.z));
	ofTranslate(this->getPos());

	ofSetColor(ofColor(c_diff.x * 255, c_diff.y * 255, c_diff.z * 255));
	plane.drawWireframe();
	ofPopMatrix();
}