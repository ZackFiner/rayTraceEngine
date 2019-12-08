#pragma once
#include "ofMain.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */
class Ray;
class RayHit;
class Shader;
class SceneObject
{
protected:
	ofImage* tex = nullptr;
public: 
	virtual glm::vec3 getPos() const = 0;
	virtual glm::quat getRot() const = 0;
	virtual void draw() const = 0;
	virtual RayHit castRay(const Ray& ray) const = 0;
	virtual void setPos(const glm::vec3& newPos) = 0;
	virtual void setRot(const glm::vec3& newRot) = 0;
	virtual glm::vec3 getDiffuse() const = 0;
	virtual glm::vec3 getSpec() const = 0;
	virtual glm::vec2 getUV(const glm::vec3& v) const { return glm::vec2(); }
	virtual glm::vec2 getUV(const glm::vec2& uv) const { return glm::vec2(); }
	virtual void setTexture(ofImage* tex) { this->tex = tex; }
	virtual ofImage* getTexture() const { return tex; }
	virtual glm::mat3 getTBN(const glm::vec2& uv) const { return glm::mat3(); }
};

class Sphere : public SceneObject
{
	glm::vec3 pos;
	glm::vec3 c_diff, c_spec;
	Shader* shade;
	float radius;
public:
	Sphere(const glm::vec3&, const glm::vec3&, const glm::vec3&, float, Shader*);
	glm::vec3 getPos() const;
	glm::quat getRot() const;
	void draw() const;
	RayHit castRay(const Ray& ray) const;
	Shader* getShader();
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpec() const;
	glm::vec2 getUV(const glm::vec3& v) const;
};

class Plane : public SceneObject
{
protected:
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 c_diff, c_spec;
	Shader* shade;
public:
	Plane(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, Shader*);
	glm::vec3 getPos() const;
	glm::quat getRot() const;
	void draw() const;
	RayHit castRay(const Ray& ray) const;
	Shader* getShader();
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpec() const;
};

class FinitePlane :public Plane
{
	float roll = 0.0f;
	glm::vec2 bounds = glm::vec2(1000,1000);
	glm::vec2 textureWrap = glm::vec2(10,10);
public:
	FinitePlane(const glm::vec3&, const glm::vec3&, const glm::vec3&, const glm::vec3&, Shader*, float, const glm::vec2& dim);
	RayHit castRay(const Ray& ray) const;
	glm::vec2 getUV(const glm::vec3& v) const;
	void setTextureWrap(const glm::vec2& v) { textureWrap = v; }
	void draw() const;
};

class Mesh;
class MeshTriangle : public SceneObject
{
private:
	glm::vec3 faceNormal;
	bool faceNormCalc = false;
	glm::vec3 getSurfaceNormal(const glm::vec2& baryPos) const;
	glm::vec2 getUVPos(const glm::vec2& baryPos) const;
public:
	MeshTriangle(int _i0, int _i1, int _i2, Mesh* parent) { ind0 = _i0; ind1 = _i1; ind2 = _i2; owner = parent; }
	Mesh* owner;
	int ind0, ind1, ind2;
	RayHit castRay(const Ray& ray) const;
	glm::vec2 getUV(const glm::vec3& v) const { return glm::vec2(); }
	void draw() const;
	glm::mat3 getTNB() const;
};