#pragma once
#include "ofMain.h"

class SceneObject;
class Light;
class Ray;


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
	bool intersect(const Ray& r); // will use another source
	bool intersect(const glm::vec3& _origin, const glm::vec3& _bounds); // intersection for another AABB
	bool intersect(const glm::vec3& pt0, const glm::vec3& pt1, const glm::vec3& pt2); // triangle AABB intersection (crucial for mesh testing), also will use another source
	
	~MeshTreeNode() {
		for (int i = 0; i < 8; i++)
			if (children[i] != nullptr)
				delete children[i];
	}
};
class Mesh;
class MeshOctree {
private:
	std::vector<MeshTreeNode*> subdivide(const glm::vec3& _origin, const glm::vec3& _bounds);
	void siftDown(MeshTreeNode* node, int depth, int bottom); // subdivides the region with stuff in it
	void getMeshBounds(Mesh& m);
public:
	Mesh* mesh;
	MeshTreeNode* root;
	glm::vec3 origin, bounds;
	int depth;

	MeshOctree(Mesh& m, int _depth);
	RayHit castRay(const Ray& r);

};

class SetObject {
public:
	SetOctree sceneObjectTree;
	std::vector<Light*> lights;

};