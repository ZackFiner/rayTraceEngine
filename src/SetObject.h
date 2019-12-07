#pragma once
#include "ofMain.h"

class SceneObject;
class Light;
class Ray;

class TreeNode {
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
	std::vector<SceneObject*> objs;
	std::unordered_map<int, TreeNode*> children;
	TreeNode* parent = nullptr;
	int dir;
	bool intersect(const Ray& r); // will use another source
	bool intersect(const glm::vec3& _origin, const glm::vec3& _bounds); // intersection for another AABB
	bool intersect(const glm::vec3& pt0, const glm::vec3& pt1, const glm::vec3& pt2); // triangle AABB intersection (crucial for mesh testing), also will use another source
	

};
class SetOctree {

};

class SetObject {
public:
	SetOctree sceneObjectTree;
	std::vector<Light*> lights;

};