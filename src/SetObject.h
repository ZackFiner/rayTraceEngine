#pragma once
#include "ofMain.h"
#include "Ray.h"
class SceneObject;
class Light;
/*
 * Author: Zackary Finer
 */

class MeshTriangle;
class MeshTreeNode {
public :
	static enum CHILD_DIR {
		NW_UP = 0,
		NE_UP = 1,
		SE_UP = 2,
		SW_UP = 3,
		NW_DW = 4,
		NE_DW = 5,
		SE_DW = 6,
		SW_DW = 7
	};
	glm::vec3 origin, bounds;
	MeshTreeNode(const glm::vec3& _origin, const glm::vec3& _bounds, int _dir=-1) { origin = _origin; bounds = _bounds; dir = _dir; }
	std::vector<MeshTriangle> objs;
	MeshTreeNode* children[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	MeshTreeNode* parent = nullptr;
	int dir;
	bool intersect(const Ray& r) const; // will use another source
	bool intersect(const glm::vec3& _origin, const glm::vec3& _bounds) const; // intersection for another AABB
	bool intersect(const glm::vec3& pt0, const glm::vec3& pt1, const glm::vec3& pt2) const; // triangle AABB intersection (crucial for mesh testing), also will use another source
	
	~MeshTreeNode() {
		for (int i = 0; i < 8; i++)
			if (children[i] != nullptr)
				delete children[i];
	}
};
class Mesh;
class MeshObject;
class MeshOctree {
private:
	std::vector<MeshTreeNode*> subdivide(const glm::vec3& _origin, const glm::vec3& _bounds);
	void siftDown(MeshTreeNode* node, int depth, int bottom); // subdivides the region with stuff in it
	void getMeshBounds(Mesh& m);
	std::vector<MeshTriangle> debugCollection;
	MeshObject* parent;
public:
	Mesh* mesh;
	MeshTreeNode* root;
	glm::vec3 origin, bounds;
	int depth;
	void draw() const;
	void drawTriangles() const;
	void draw(MeshTreeNode* node) const;
	MeshOctree(Mesh& m, int _depth, MeshObject* _parent);
	RayHit castRay(const Ray& r) const;

};

class SetObject {
public:
	SetObject() {}
	std::vector<SceneObject*> objects;
	std::vector<Light*> lights;
	std::vector<ofImage*> textures;
	RayCam cam;
	float ambientBase = 0.1f;
};