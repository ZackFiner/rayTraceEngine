#include "SceneObjects.h"
#include "Ray.h"
#include "MeshLoader/Mesh.h"

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
glm::vec2 Sphere::getUV(const glm::vec3& v) const {
	//map using sphereical coords
	//Formulas can be found on http://mathworld.wolfram.com/SphericalCoordinates.html
	auto local = v - pos;
	float r = glm::length(local);
	return glm::vec2((glm::atan(local.x / local.z)+PI)/(PI*2), (glm::acos(local.y / r))/PI);
}
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
	// NOTE: what i've noticed is that when rays are cast in direction that is close to being in alignment with
	// the normal, it still registers a hit.
	if (_return.hit = glm::intersectRayPlane(ray.getOrig()-pos, ray.getDir(), glm::vec3(0,0,0), this->norm, dist) && dist > 0)
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
		auto local = hit.hitPos - this->getPos();
		auto flatHit = local*glm::angleAxis(glm::acos(glm::dot(this->norm, glm::vec3(0.0f, 1.0f, 0.0f))), glm::cross(this->norm, glm::vec3(0.0f, 1.0f, 0.0f)));
		flatHit = glm::rotate(flatHit, -glm::radians(roll), glm::vec3(0.0f, 1.0f, 0.0f)); // adjust for roll
		//flatHit = flatHit - this->getPos(); // we center it at origin

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
	ofTranslate(this->getPos());
	ofRotateX(glm::degrees(eul.x));
	ofRotateY(glm::degrees(eul.y));
	ofRotateZ(glm::degrees(eul.z));

	ofSetColor(ofColor(c_diff.x * 255, c_diff.y * 255, c_diff.z * 255));
	plane.drawWireframe();
	ofPopMatrix();
}

glm::vec2 FinitePlane::getUV(const glm::vec3& v) const
{
	glm::vec2 uv;
	if (glm::normalize(this->norm) != glm::vec3(0.0f, 1.0f, 0.0f) || roll != 0.0f)
	{
		auto local = v - this->getPos();
		//auto transMat = local * glm::angleAxis(-glm::radians(this->roll), this->norm);
		auto transMat = local * glm::angleAxis(glm::acos(glm::dot(this->norm, glm::vec3(0.0f, 1.0f, 0.0f))), glm::cross(this->norm, glm::vec3(0.0f, 1.0f, 0.0f)));
		transMat = transMat * glm::angleAxis(-glm::radians(roll), glm::vec3(0.0f, 1.0f, 0.0f));
		transMat = transMat + bounds/2;
		uv = glm::vec2(transMat.x / (bounds.x / textureWrap.x), transMat.z / (bounds.y / textureWrap.y));
	}
	else
	{
		auto transMat = v - pos + bounds/2;
		uv = glm::vec2(v.x / (bounds.x / textureWrap.x), v.z / (bounds.y / textureWrap.y));
	}
	return uv;
}

glm::vec3 MeshTriangle::getSurfaceNormal(const glm::vec2& baryPos) const {
	//first, we ensure that all verticies have valid normals, otherwise we just return the surface normal

	if (owner->indicies[ind0].normal == -1 || owner->indicies[ind1].normal == -1 || owner->indicies[ind2].normal == -1) {
		/*if (faceNormCalc)
			return faceNormal;
		
		faceNormCalc = true;*/
		auto vert0 = owner->verts[owner->indicies[ind0].vert];
		auto vert1 = owner->verts[owner->indicies[ind1].vert];
		auto vert2 = owner->verts[owner->indicies[ind2].vert];
		auto faceNormal0 = glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
		return faceNormal0;

	}

	auto norm0 = owner->normals[owner->indicies[ind0].normal];
	auto norm1 = owner->normals[owner->indicies[ind1].normal];
	auto norm2 = owner->normals[owner->indicies[ind2].normal];
	//below we mix the normals: you can see that when we're at bary centric point [0,0] (closest to vert0), this will equal norm0
	//and when we're at [0,1] we return norm2, etc.
	//it generates a weighted bisector for the triangle surface

	//I've tried to find a citation for this: this is all i located https://stackoverflow.com/questions/38717963/best-way-to-interpolate-triangle-surface-using-3-positions-and-normals-for-ray-t
	return glm::normalize(norm0*(1 - (baryPos.x + baryPos.y)) + norm1 * baryPos.x + norm2 * baryPos.y);
}

glm::vec2 MeshTriangle::getUVPos(const glm::vec2& baryPos) const {
	if (owner->indicies[ind0].texCoord == -1 || owner->indicies[ind1].texCoord == -1 || owner->indicies[ind2].texCoord == -1)
		return glm::vec2();

	auto tC0 = owner->texCoords[owner->indicies[ind0].texCoord];
	auto tC1 = owner->texCoords[owner->indicies[ind1].texCoord];
	auto tC2 = owner->texCoords[owner->indicies[ind2].texCoord];
	//this is a relatively simple operation, we just remap our berycentric points to fall within the bounds of the uvw triangle
	return glm::vec2((tC1 - tC0)*baryPos.x, (tC2 - tC0)*baryPos.y) + tC0;
}

RayHit MeshTriangle::castRay(const Ray& ray) const {
	glm::vec3 baryPos;
	if (glm::intersectRayTriangle(ray.getOrig(), ray.getDir(),
		owner->verts[owner->indicies[ind0].vert],
		owner->verts[owner->indicies[ind1].vert],
		owner->verts[owner->indicies[ind2].vert],
		baryPos)) // NOTE: if this isn't working, flip the order of the indicies as they verts should be counter clockwise
	{
		auto vert0 = owner->verts[owner->indicies[ind0].vert];
		auto vert1 = owner->verts[owner->indicies[ind1].vert];
		auto vert2 = owner->verts[owner->indicies[ind2].vert];

		RayHit hit;
		hit.hit = true;
		hit.hitDir = ray.getDir();
		hit.hitDist = baryPos.z;
		glm::vec2 baryS = glm::vec2(baryPos.x, baryPos.y);
		hit.hitUV = getUVPos(baryS);

		hit.hitObject = (SceneObject*)this;
		hit.hitNorm = getSurfaceNormal(baryS);
		hit.hitPos = baryS.x * (vert1 - vert0) + baryS.y * (vert2 - vert0);
		return hit;
	}
	return RayHit();
}

void MeshTriangle::draw() const {
	auto vert0 = owner->verts[owner->indicies[ind0].vert];
	auto vert1 = owner->verts[owner->indicies[ind1].vert];
	auto vert2 = owner->verts[owner->indicies[ind2].vert];
	ofDrawTriangle(vert0, vert1, vert2);
}


glm::mat3 getTBN() const {

}