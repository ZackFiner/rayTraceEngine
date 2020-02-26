#include "SceneObjects.h"
#include "Ray.h"
#include "SetObject.h"

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
	return glm::vec2((glm::atan(local.x / local.z)+PI)/(PI*2), -(glm::acos(local.y / r))/PI);
}

float Sphere::sdf(const glm::vec3& p) const {
	// sphere signed distance is easy, we just find the distance between our test point and the origin of the sphere, and subtract the radius
	return glm::distance(p, getPos()) - radius;
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

float Plane::sdf(const glm::vec3& p) const {
	// signed distance for a plane is also easy, we use the dot product of the normal and our localized sample point to calculate it
	// however, this will mean that points on the other side of the plane will be considered 'inside' the plane
	return glm::dot(norm, p-getPos());
}

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

float FinitePlane::sdf(const glm::vec3& p) const {
	// finite plane distance is a little more complicated

	// similarly to the intersection test for finite plane, we need to transform our test point as if the plane was the world origin and axis aligned
	auto local = p - this->getPos(); // center the point to the plane origin
	// next we rotate our point as if the plane normal is aligned to the world axis
	auto flatHit = local * glm::angleAxis(glm::acos(glm::dot(this->norm, glm::vec3(0.0f, 1.0f, 0.0f))), glm::cross(this->norm, glm::vec3(0.0f, 1.0f, 0.0f)));
	flatHit = glm::rotate(flatHit, -glm::radians(roll), glm::vec3(0.0f, 1.0f, 0.0f)); // finally, undoe any affect caused by roll
	// now, flat hit is a localized point to the finite plane: the finite plane is axis aligned and has a normal of 0,1,0 to the point
	
	if (glm::abs(flatHit.x) < bounds.x*0.5f && glm::abs(flatHit.z) < bounds.y*0.5f) { // if the point is above the finite plane (and not next to it)
		return flatHit.y; // since we're in bounds, we just use the dot product, but since we're localized and the normal is 0,1,0, then we just return the y value
	}
	else { // otherwise, if we're outside the bounds
		// use the formula for minimum distance between a point and axis aligned rectangle
		float max_x = glm::max(bounds.x*-0.5f - flatHit.x, 0.0f, flatHit.x - bounds.x*0.5f);
		float max_y = glm::max(bounds.y*-0.5f - flatHit.z, 0.0f, flatHit.z - bounds.y*0.5f);
		max_x = glm::abs(max_x) - bounds.x*0.5f;
		max_y = glm::abs(max_y) - bounds.y*0.5f;
		return glm::sqrt(max_x*max_x + max_y*max_y + flatHit.y * flatHit.y);
	}
}

MandleBulb::MandleBulb(const glm::vec3& p, const glm::vec3& diff, const glm::vec3& spec, float r, Shader* s = nullptr) :
	Sphere(p, diff, spec, r, s)
{

}

/*
 * 
 * Christensen, M. (September 20, 2011). Distance Estimated 3D Fractals (V): The Mandelbulb & Different DE Approximations [Blog Post]. Retrieved from
 * http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/
 *
 */
float MandleBulb::sdf(const glm::vec3& p) const {
	unsigned int Iterations = 150;
	double Bailout = 10.0;
	double Power = 8.0;
	glm::vec3 localPos = (p - getPos())*0.1;
	glm::vec3 z = localPos;
	double dr = 1.0;
	double r = 0.0;
	for (int i = 0; i < Iterations; i++) {
		r = glm::length(z);
		if (r > Bailout) break;// if we've diverged, break out

		// convert to polar coordinates
		double theta = glm::acos(z.z / r);
		double phi = glm::atan(z.y, z.x);
		dr = glm::pow(r, Power - 1.0f)*Power*dr + 1.0f;

		double zr = glm::pow(r, Power);
		theta = theta * Power;
		phi = phi * Power;

		// convert back to cartesian coordinates
		z = zr * glm::vec3(glm::sin(theta)*glm::cos(phi), glm::sin(phi)*glm::sin(theta), glm::cos(theta));
		z += localPos;
	}
	return 0.5f * glm::log(r)*r / dr;
}

Torus::Torus(const glm::vec3& _pos, const glm::vec2& _t) {
	pos = _pos;
	t = _t;
}

glm::vec3 Torus::getPos() const { return pos; }
glm::quat Torus::getRot() const { return rot; }
void Torus::draw() const {
	ofPushMatrix();
	ofNoFill();
	ofSetColor(ofColor::red);
	ofTranslate(getPos());
	ofDrawSphere(t.x);
	ofSetColor(ofColor::white);
	ofFill();
	ofPopMatrix();
}

RayHit Torus::castRay(const Ray& ray) const {
	return RayHit();
}

void Torus::setPos(const glm::vec3& newPos) {
	pos = newPos;
}

void Torus::setRot(const glm::vec3& newRot) {
	rot = glm::quat(newRot);
}

glm::vec3 Torus::getDiffuse() const {
	return glm::vec3(0.5f, 0.5f, 0.5f);
}

glm::vec3 Torus::getSpec() const {
	return glm::vec3(1.0f);
}
/*
 * Below is the implementation of the Torus sdf. I've used the implementation provided Inigo Quilez.
 *
 * Quiliz, I. (2008). 3D SDF functions. Retrieved from https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */
float Torus::sdf(const glm::vec3& p) const {
	auto localizedP = (p - getPos()) * glm::inverse(getRot());
	/*
	 * Notice:
	 * (1) glm::length(glm::vec2(localizedP.x, localizedP.z)) - t.x
	 * the inner vector is composed of the x any z component of the sample point: it represent the point in 2d space if we were
	 * looking directly down (or up) at the torus in orthographic projection. By taking the length of this and then subtracting the t.x,
	 * this will give the sdf the behavior that there is a inner area (the middle of our torus) that is not filled, because if x and z are 0, this
	 * will make the sdf become -t.x
	 *
	 * Next, if we re-introduce the sample points y component, and put those two quantities in a vector, we now have a vector that represents
	 * the horizontal closest distance to the surface, w/ the respective y (height) component
	 * (2) glm::vec2((1), localizedP.y)
	 *
	 * Now, to finish our sdf, we take the length of this (2) new vector, and subtract our t.y (outer radius)
	 */
	glm::vec2 q = glm::vec2(glm::length(glm::vec2(localizedP.x, localizedP.z)) - t.x, localizedP.y);
	return glm::length(q) - t.y;
}

/*
Below, we use the phong-shading method of generating surface normals for a triangle 
Marschner, S., & Shirley, P. (2016). Fundamentals of Computer Graphics, Fourth Edition. A. K. Peters, Ltd..

*/
glm::vec3 MeshTriangle::getSurfaceNormal(const glm::vec2& baryPos) const {
	//first, we ensure that all verticies have valid normals, otherwise we just return the surface normal

	if (owner->indicies[ind0].normal == -1 || owner->indicies[ind1].normal == -1 || owner->indicies[ind2].normal == -1) {
		/*if (faceNormCalc)
			return faceNormal;
		
		faceNormCalc = true;*/
		auto vert0 = owner->verts[owner->indicies[ind0].vert-1];
		auto vert1 = owner->verts[owner->indicies[ind1].vert-1];
		auto vert2 = owner->verts[owner->indicies[ind2].vert-1];
		auto faceNormal0 = glm::normalize(glm::cross(vert1 - vert0, vert2 - vert0));
		return faceNormal0;

	}

	auto norm0 = owner->normals[owner->indicies[ind0].normal-1];
	auto norm1 = owner->normals[owner->indicies[ind1].normal-1];
	auto norm2 = owner->normals[owner->indicies[ind2].normal-1];
	//below we mix the normals: you can see that when we're at bary centric point [0,0] (closest to vert0), this will equal norm0
	//and when we're at [0,1] we return norm2, etc.
	//it generates a weighted bisector for the triangle surface

	//I've tried to find a citation for this: this is all i located https://stackoverflow.com/questions/38717963/best-way-to-interpolate-triangle-surface-using-3-positions-and-normals-for-ray-t
	auto mixed_norm = glm::normalize(norm0*(1 - (baryPos.x + baryPos.y)) + norm1 * baryPos.x + norm2 * baryPos.y);
	return mixed_norm;
}

glm::vec4 MeshTriangle::getSurfaceTangent(const glm::vec2& baryPos) const {
	//first, we ensure that all verticies have valid normals, otherwise we just return the surface normal

	if (parent->getTexture(BUMP_MAP) == nullptr || owner->indicies[ind0].tangent == -1 || owner->indicies[ind1].tangent == -1 || owner->indicies[ind2].tangent == -1) {
		return glm::vec4();
	}

	auto norm0 = glm::vec3(owner->tangents[owner->indicies[ind0].tangent]);
	auto norm1 = glm::vec3(owner->tangents[owner->indicies[ind1].tangent]);
	auto norm2 = glm::vec3(owner->tangents[owner->indicies[ind2].tangent]);
	//below we mix the normals: you can see that when we're at bary centric point [0,0] (closest to vert0), this will equal norm0
	//and when we're at [0,1] we return norm2, etc.
	//it generates a weighted bisector for the triangle surface

	//I've tried to find a citation for this: this is all i located https://stackoverflow.com/questions/38717963/best-way-to-interpolate-triangle-surface-using-3-positions-and-normals-for-ray-t
	auto mixed_norm = glm::normalize(norm0*(1 - (baryPos.x + baryPos.y)) + norm1 * baryPos.x + norm2 * baryPos.y);
	return glm::vec4(mixed_norm, owner->tangents[owner->indicies[ind0].tangent].w);
}

glm::vec2 MeshTriangle::getUVPos(const glm::vec2& baryPos) const {
	if (owner->indicies[ind0].texCoord == -1 || owner->indicies[ind1].texCoord == -1 || owner->indicies[ind2].texCoord == -1)
		return glm::vec2();

	auto tC0 = owner->texCoords[owner->indicies[ind0].texCoord-1];
	auto tC1 = owner->texCoords[owner->indicies[ind1].texCoord-1];
	auto tC2 = owner->texCoords[owner->indicies[ind2].texCoord-1];
	//this is a relatively simple operation, we just remap our berycentric points to fall within the bounds of the uvw triangle
	auto uv = (tC1 - tC0)*baryPos.x + (tC2 - tC0)*baryPos.y + tC0;
	return uv;
}

RayHit MeshTriangle::castRay(const Ray& ray) const {
	glm::vec3 baryPos;
	if (glm::intersectRayTriangle(ray.getOrig(), ray.getDir(),
		owner->verts[owner->indicies[ind0].vert-1],
		owner->verts[owner->indicies[ind1].vert-1],
		owner->verts[owner->indicies[ind2].vert-1],
		baryPos))
	{
		auto vert0 = owner->verts[owner->indicies[ind0].vert-1];
		auto vert1 = owner->verts[owner->indicies[ind1].vert-1];
		auto vert2 = owner->verts[owner->indicies[ind2].vert-1];

		RayHit hit;
		hit.hit = true;
		hit.hitDir = ray.getDir();
		hit.hitDist = baryPos.z;
		glm::vec2 baryS = glm::vec2(baryPos.x, baryPos.y);
		hit.hitUV = getUVPos(baryS);
		hit.hitUVSet = true;

		hit.hitObject = (SceneObject*)this;
		hit.hitNorm = getSurfaceNormal(baryS);
		hit.hitPos = baryS.x * (vert1 - vert0) + baryS.y * (vert2 - vert0) + vert0;
		if (parent->getTexture(BUMP_MAP) != nullptr &&
			owner->indicies[ind0].tangent != -1 &&
			owner->indicies[ind1].tangent != -1 &&
			owner->indicies[ind2].tangent != -1)
		{ // we need to perturb the normal 
			hit.hitTangent = getSurfaceTangent(baryS);
			hit.hitTangentSet = true;
		}
		return hit;
	}
	return RayHit();
}

void MeshTriangle::draw() const {
	ofNoFill();
	auto vert0 = owner->verts[owner->indicies[ind0].vert-1];
	auto vert1 = owner->verts[owner->indicies[ind1].vert-1];
	auto vert2 = owner->verts[owner->indicies[ind2].vert-1];
	auto baryPos = 0.25f * (vert1 - vert0) + 0.25f * (vert2 - vert0) + vert0;
	ofSetColor(ofColor::white);
	ofDrawTriangle(vert0, vert1, vert2);
	ofSetColor(ofColor::red);
	
	auto surfaceNorm = getSurfaceNormal(glm::vec2(0.25f, 0.25f));
	
	ofDrawLine(surfaceNorm*5.0 +baryPos, baryPos);

	auto tangent = getSurfaceTangent(glm::vec2(0.25f, 0.25f));

	ofSetColor(ofColor::green);
	ofDrawLine(glm::vec3(tangent.x, tangent.y, tangent.z)*5.0 + baryPos, baryPos);
	 
	ofSetColor(ofColor::blue);
	ofDrawLine(glm::normalize(glm::cross(glm::vec3(tangent), surfaceNorm))*tangent.w*5.0 + baryPos, baryPos);
	
	ofSetColor(ofColor::white);
}

ofImage* MeshTriangle::getTexture(TextureType t) const {
	return parent->getTexture(t);
}

MeshObject::MeshObject(std::string filepath, const glm::vec3& _pos) :
	mesh_data(filepath)
{
	queryTree = new MeshOctree(mesh_data, MESH_TREE_DEPTH, this);
	pos = _pos;
}

glm::vec3 MeshObject::getPos() const {
	return pos;
}

glm::quat MeshObject::getRot() const {
	return rot;
}

void MeshObject::draw() const {
	ofPushMatrix();
	ofTranslate(pos);
	auto r = glm::eulerAngles(rot);
	ofRotateX(glm::degrees(r.x));
	ofRotateY(glm::degrees(r.y));
	ofRotateZ(glm::degrees(r.z));

	mesh_data.draw();
	//queryTree->draw();
	queryTree->drawTriangles();
	
	ofNoFill();
	ofPopMatrix();
}

RayHit MeshObject::castRay(const Ray& ray) const {
	// We need to localize the ray to the mesh's space
	Ray testRayAdj = Ray(ray.getDir()*glm::inverse(rot) , (ray.getOrig() - pos)*glm::inverse(rot));
	RayHit result = queryTree->castRay(testRayAdj);

	if (result.hit)
	{
		// we need to adjust for the transformation we did to query the octree
		result.hitPos = result.hitPos*rot + pos;
		result.hitNorm = result.hitNorm*rot;
		result.hitDir = result.hitDir*rot;
		result.hitTangent = glm::vec4(glm::vec3(result.hitTangent)*rot, result.hitTangent.w);
	}
	return result;
}

void MeshObject::setPos(const glm::vec3& newPos) {
	pos = newPos;
}

void MeshObject::setRot(const glm::vec3& newRot) {
	rot = glm::toQuat(glm::eulerAngleXYZ(newRot.x, newRot.y, newRot.z));
}

glm::vec3 MeshObject::getDiffuse() const { return glm::vec3(1.0f, 0.0f, 0.0f); }
glm::vec3 MeshObject::getSpec() const { return glm::vec3(1.0f); }
MeshObject::~MeshObject()
{
	delete queryTree;
}
