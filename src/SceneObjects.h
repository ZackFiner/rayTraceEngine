#pragma once
#include "ofMain.h"
#include "MeshLoader/Mesh.h"
/*H*********************************************************
 *
 * AUTHOR: Zackary Finer
 *
 *
 *
 */

enum TextureType {
	DIFFUSE_MAP = 0,
	SPECULAR_MAP = 1,
	BUMP_MAP = 2
};

class Ray;
class RayHit;
class Shader;
class SceneObject
{
protected:
	ofImage* tex[3] = { nullptr, nullptr, nullptr };
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
	virtual void setTexture(ofImage* tex, TextureType t) { this->tex[t] = tex; }
	virtual ofImage* getTexture(TextureType t) const { return tex[t]; }
	virtual glm::mat3 getTBN(const glm::vec2& baryCoord) const { return glm::mat3(); }

	// RAY MARCHING FUNCTIONS BELOW
	virtual float sdf(const glm::vec3& p) const { return glm::distance(p, getPos()) - 1.0f; /*Default is that of a Sphere with radius 1.0f*/ };


	virtual ~SceneObject() {};
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

	float sdf(const glm::vec3& p) const;
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

	float sdf(const glm::vec3& p) const;
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

	float sdf(const glm::vec3& p) const;
};

class Mesh;



#define MESH_TREE_DEPTH 7

class MeshOctree;
class MeshObject : public SceneObject {
private:

	Mesh mesh_data;
	glm::vec3 pos;
	glm::quat rot;
	glm::mat4 inv_transform;
	MeshOctree* queryTree;
public:
	MeshObject(std::string filepath, const glm::vec3& _pos);
	glm::vec3 getPos() const;
	glm::quat getRot() const;
	void draw() const;
	RayHit castRay(const Ray& ray) const;
	void setPos(const glm::vec3& newPos);
	void setRot(const glm::vec3& newRot);
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpec() const;
	~MeshObject();
};
class MeshTriangle : public SceneObject
{
private:
	glm::vec3 faceNormal;
	bool faceNormCalc = false;
	glm::vec3 getSurfaceNormal(const glm::vec2& baryPos) const;
	glm::vec4 getSurfaceTangent(const glm::vec2& baryPos) const;
	glm::vec2 getUVPos(const glm::vec2& baryPos) const;
	MeshObject* parent = nullptr;
public:
	MeshTriangle(int _i0, int _i1, int _i2, Mesh* _owner, MeshObject* _parent) { ind0 = _i0; ind1 = _i1; ind2 = _i2; owner = _owner; parent = _parent; }
	Mesh* owner;
	int ind0, ind1, ind2;
	glm::vec3 getPos() const { return glm::vec3(); };
	glm::quat getRot() const { return glm::quat(); };
	void setPos(const glm::vec3& newPos) {};
	void setRot(const glm::vec3& newRot) {};
	glm::vec3 getDiffuse() const { return glm::vec3(1.0f, 0.0f, 0.0f); }
	glm::vec3 getSpec() const { return glm::vec3(1.0f); }
	ofImage* getTexture(TextureType t) const;
	RayHit castRay(const Ray& ray) const;
	glm::vec2 getUV(const glm::vec3& v) const { return glm::vec2(); }
	void draw() const;
};