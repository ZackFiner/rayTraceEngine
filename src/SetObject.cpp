#include "SetObject.h"
#include "Ray.h"
#include "MeshLoader/Mesh.h"
#include "SceneObjects.h"


bool axisTest(const glm::vec3& axis, float min, float max, int count, ...) {
	va_list points;
	va_start(points, count);
	float set_min = FLT_MAX;
	float set_max = -FLT_MAX;
	for (int i = 0; i < count; i++) {
		float val;
		if ((val = glm::dot(axis, va_arg(points, glm::vec3))) > set_max)
			set_max = val;
		if (val < set_min)
			set_min = val;
	}
	va_end(points);
	return set_max >= min && set_min <= max;
}

bool boxAxisTest(const glm::vec3& axis, const glm::vec3& halfBounds, int count, ...) {
	/*
	 * Akenine-Möllser, T. (2001) Fast 3D Triangle-Box Overlap Testing, Journal of Graphics Tools,
	 * 6:1, 29-33, DOI: 10.1080/10867651.2001.10487535
	 */
	va_list points;
	va_start(points, count);
	float set_min = FLT_MAX;
	float set_max = -FLT_MAX;
	for (int i = 0; i < count; i++) {
		float val;
		if ((val = glm::dot(axis, va_arg(points, glm::vec3))) > set_max)
			set_max = val;
		if (val < set_min)
			set_min = val;
	}
	va_end(points);
	//next, project box along axis
	float radius = halfBounds.x*glm::abs(axis.x) + halfBounds.y*glm::abs(axis.y) + halfBounds.z*glm::abs(axis.z);

	return !(set_min > radius || set_max < -radius);
}

/*
 * Box Triangle intersection using a variation of Seperating Axis theorem as described in:
 *
 *      Akenine-Möllser, T. (2001) Fast 3D Triangle-Box Overlap Testing, Journal of Graphics Tools,
 *      6:1, 29-33, DOI: 10.1080/10867651.2001.10487535
 */
bool MeshTreeNode::intersect(const glm::vec3& pt0, const glm::vec3& pt1, const glm::vec3& pt2) const {


	//first move the triangle so that the box is centered at the origin
	auto vert0 = pt0 - origin;
	auto vert1 = pt1 - origin;
	auto vert2 = pt2 - origin;
	auto halfBounds = bounds * 0.5f;
	auto edge0 = pt1 - pt0;
	auto edge1 = pt2 - pt1;
	auto edge2 = pt0 - pt2;
	auto norm = glm::normalize(glm::cross(edge0, edge1));//this may need to be flipped

	//3 test along the boxes axis (1,0,0) (0,1,0) (0,0,1)

	if (!axisTest(glm::vec3(1, 0, 0), -halfBounds.x, halfBounds.x, 3, vert0, vert1, vert2)) // the bounds for this are +-halfBounds.x
		return false;
	if (!axisTest(glm::vec3(0, 1, 0), -halfBounds.y, halfBounds.y, 3, vert0, vert1, vert2)) // the bounds for this are +-halfBounds.y
		return false;
	if (!axisTest(glm::vec3(0, 0, 1), -halfBounds.z, halfBounds.z, 3, vert0, vert1, vert2)) // the bounds for this are +-halfBounds.z
		return false;

	//normal test (to determine if plane of triangle passes through the box, we use the min and max verts to perform this test.
	/*
	 * 
	 * https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	 */
	float radius = halfBounds.x * glm::abs(norm.x) + halfBounds.y * glm::abs(norm.y) + halfBounds.z * glm::abs(norm.z);
	float s = -glm::dot(vert0, norm); // distance from plane to box center
	if (!(glm::abs(s) <= radius))
		return false;

	if (!boxAxisTest(glm::cross(glm::vec3(1, 0, 0), edge0), halfBounds, 3, vert0, vert1, vert2))
		return false;
	if (!boxAxisTest(glm::cross(glm::vec3(1, 0, 0), edge1), halfBounds, 3, vert0, vert1, vert2))
		return false;
	if (!boxAxisTest(glm::cross(glm::vec3(1, 0, 0), edge2), halfBounds, 3, vert0, vert1, vert2))
		return false;

	if (!boxAxisTest(glm::cross(glm::vec3(0, 1, 0), edge0), halfBounds, 3, vert0, vert1, vert2))
		return false;
	if (!boxAxisTest(glm::cross(glm::vec3(0, 1, 0), edge1), halfBounds, 3, vert0, vert1, vert2))
		return false;
	if (!boxAxisTest(glm::cross(glm::vec3(0, 1, 0), edge2), halfBounds, 3, vert0, vert1, vert2))
		return false;

	if (!boxAxisTest(glm::cross(glm::vec3(0, 0, 1), edge0), halfBounds, 3, vert0, vert1, vert2))
		return false;
	if (!boxAxisTest(glm::cross(glm::vec3(0, 0, 1), edge1), halfBounds, 3, vert0, vert1, vert2))
		return false;
	if (!boxAxisTest(glm::cross(glm::vec3(0, 0, 1), edge2), halfBounds, 3, vert0, vert1, vert2))
		return false;

	return true; // no seperating axis could be found. triangle is intersecting

}

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 * 
 */
bool MeshTreeNode::intersect(const Ray &r) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	
	/*Code below patches things*/
	float t0 = 0.0f;
	float t1 = FLT_MAX;
	int sign[3] = {};
	glm::vec3 parameters[2] = { origin - bounds * 0.5f, origin + bounds * 0.5f };
	glm::vec3 inv_direction = glm::vec3(1.0f / r.getDir().x, 1.0f / r.getDir().y, 1.0f / r.getDir().z);
	sign[0] = (1.0f / r.getDir().x) < 0.0f;
	sign[1] = (1.0f / r.getDir().y) < 0.0f;
	sign[2] = (1.0f / r.getDir().z) < 0.0f;

	tmin = (parameters[sign[0]].x - r.getOrig().x) * inv_direction.x;
	tmax = (parameters[1 - sign[0]].x - r.getOrig().x) * inv_direction.x;
	tymin = (parameters[sign[1]].y - r.getOrig().y) * inv_direction.y;
	tymax = (parameters[1 - sign[1]].y - r.getOrig().y) * inv_direction.y;
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (parameters[sign[2]].z - r.getOrig().z) * inv_direction.z;
	tzmax = (parameters[1 - sign[2]].z - r.getOrig().z) * inv_direction.z;
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	return ((tmin < t1) && (tmax > t0));
}

bool MeshTreeNode::intersect(const glm::vec3& _origin, const glm::vec3& _bounds) const {
	glm::vec3 max = origin + bounds * 0.5f;
	glm::vec3 min = origin - bounds * 0.5f;
	glm::vec3 o_max = _origin + _bounds * 0.5f;
	glm::vec3 o_min = _origin - _bounds * 0.5f;

	return ((max.x >= o_min.x && min.x <= o_max.x) ||
			(max.y >= o_min.y && min.y <= o_max.y) ||
			(max.z >= o_min.z && min.z <= o_max.z));
}

std::vector<MeshTreeNode*> MeshOctree::subdivide(const glm::vec3& _origin, const glm::vec3& _bounds) {
	glm::vec3 subBounds = _bounds * 0.5f;
	glm::vec3 halfOffset = subBounds * 0.5f;
	std::vector<MeshTreeNode*> return_values;
	return_values.push_back(new MeshTreeNode(_origin + glm::vec3(-halfOffset.x, halfOffset.y, halfOffset.z), subBounds, MeshTreeNode::NW_UP));
	return_values.push_back(new MeshTreeNode(_origin + halfOffset, subBounds, MeshTreeNode::NE_UP));
	return_values.push_back(new MeshTreeNode(_origin + glm::vec3(halfOffset.x, halfOffset.y, -halfOffset.z), subBounds, MeshTreeNode::SE_UP));
	return_values.push_back(new MeshTreeNode(_origin + glm::vec3(-halfOffset.x, halfOffset.y, -halfOffset.z), subBounds, MeshTreeNode::SW_UP));
	return_values.push_back(new MeshTreeNode(_origin + glm::vec3(-halfOffset.x, -halfOffset.y, halfOffset.z), subBounds, MeshTreeNode::NW_DW));
	return_values.push_back(new MeshTreeNode(_origin + glm::vec3(halfOffset.x, -halfOffset.y, halfOffset.z), subBounds, MeshTreeNode::NE_DW));
	return_values.push_back(new MeshTreeNode(_origin + glm::vec3(halfOffset.x, -halfOffset.y, -halfOffset.z), subBounds, MeshTreeNode::SE_DW));
	return_values.push_back(new MeshTreeNode(_origin - halfOffset, subBounds, MeshTreeNode::SW_DW));
	return return_values;
}

void MeshOctree::getMeshBounds(Mesh& m) {
	glm::vec3 max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	glm::vec3 min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	for (int i = 0; i < m.verts.size(); i++) {
		if (max.x < m.verts[i].x)
			max.x = m.verts[i].x;
		if (min.x > m.verts[i].x)
			min.x = m.verts[i].x;

		if (max.y < m.verts[i].y)
			max.y = m.verts[i].y;
		if (min.y > m.verts[i].y)
			min.y = m.verts[i].y;

		if (max.z < m.verts[i].z)
			max.z = m.verts[i].z;
		if (min.z > m.verts[i].z)
			min.z = m.verts[i].z;
	}
	bounds = max - min;
	origin = (max + min)*0.5f;
	bounds *= 1.1f;//this is for padding
}

MeshOctree::MeshOctree(Mesh& m, int _depth, MeshObject* _parent) {
	parent = _parent;
	getMeshBounds(m);
	mesh = &m;
	root = new MeshTreeNode(origin, bounds);
	depth = _depth;
	//Right now, this class is responsible for creating the triangle operations
	for (int i = 0; i < m.indicies.size(); i += 3) {
		debugCollection.push_back(MeshTriangle(i, i + 1, i + 2, &m, parent));
		root->objs.push_back(MeshTriangle(i, i + 1, i + 2, &m, parent));
	}

	siftDown(root, 0, depth);
}

void MeshOctree::siftDown(MeshTreeNode* node, int depth, int bottom) {
	// below is a fairly standard implementation of an octree subdivision algorithm

	if (depth >= bottom || node==nullptr || node->objs.size() <= 0) //the second should never happen but w/e
		return;

	auto possible_children = subdivide(node->origin, node->bounds);
	std::vector<MeshTriangle> overflowObj; // we use this to hold all triangles which cannot be subdivided further
	for (auto tri : node->objs) {
		int matched_region = -1; // i don't have a "triangle inside" algorithm, so we'll just count whenever we find only one match
	
		auto pt0 = mesh->verts[mesh->indicies[tri.ind0].vert-1];
		auto pt1 = mesh->verts[mesh->indicies[tri.ind1].vert-1];
		auto pt2 = mesh->verts[mesh->indicies[tri.ind2].vert-1];
		for (auto child : possible_children) {
			if (child->intersect(pt0, pt1, pt2)) {
				/*if (matched_region != -1) {
					overflowObj.push_back(tri);
					matched_region = 8;
					break; //stop the search
				}
				else {
					matched_region = child->dir;
				}
				// the code above doesn't create duplicate objects in child nodes,
				// but instead stores them in the minimum spanning parent node
				// i found this technique to be inadequate
				*/
				if (node->children[child->dir] == nullptr)
				{ // allocate a new region
					possible_children[child->dir]->parent = node;
					node->children[child->dir] = possible_children[child->dir];
					node->children[child->dir]->objs.push_back(tri); // we add the triangle
				}
				else {
					node->children[child->dir]->objs.push_back(tri);
				}
			}
		}

		/*
		if (matched_region == 8)
			continue; // we've already evaluated the triangle, continue
		if (matched_region == -1)
			continue; //handle this, this is when the triangle didn't fit in anywhere
		
		if (node->children[matched_region] == nullptr)
		{ // allocate a new region
			possible_children[matched_region]->parent = node;
			node->children[matched_region] = possible_children[matched_region];
			node->children[matched_region]->objs.push_back(tri); // we add the triangle
		}
		else {
			node->children[matched_region]->objs.push_back(tri);
		}
		// the code above doesn't create duplicate objects in child nodes,
		// but instead stores them in the minimum spanning parent node
		// i found this technique to be inadequate
		*/

	}
	
	node->objs = overflowObj; // this is roughly equivalent to clearing the vector

	for (auto child : possible_children)
		if (child->objs.size() <= 0)
			delete child; //cleanup

	for (int i = 0; i < 8; i++)
	{
		siftDown(node->children[i], depth + 1, bottom);
	}
}

RayHit MeshOctree::castRay(const Ray& r) const {
	if (!root->intersect(r))
		return  RayHit();
	std::unordered_map<SceneObject*, int> checked_set;
	std::stack<MeshTreeNode*> unsearchedNodes;
	std::vector<SceneObject*> candidates;
	unsearchedNodes.push(root);
	while (!unsearchedNodes.empty()) {
		MeshTreeNode* current = unsearchedNodes.top();
		unsearchedNodes.pop();

		if (current->objs.size() > 0) //we've definately hit this node, so add all the objects in it
			for (auto& tri : current->objs)
				if (checked_set.find(&tri) == checked_set.end()) {
					checked_set.insert({&tri, 0});
					candidates.push_back((SceneObject*)(&tri));
				}

		for (int i = 0; i < 8; i++) {
			if (current->children[i] != nullptr && current->children[i]->intersect(r))
				unsearchedNodes.push(current->children[i]);
		}
	}
	// WE MUST ENSURE THAT TRIANGLES ARE NOT DE-ALLOCATED BEFORE THE OCTREE IS & CANDIDATES ARE ACTUAL TRIANGLE ADDRESSES
	Ray testRay = r;
	return testRay.getHit(candidates);
}

void MeshOctree::draw() const {
	draw(root);
}

void MeshOctree::draw(MeshTreeNode* node) const {
	ofNoFill();
	ofSetColor(ofColor::blue);
	ofDrawBox(node->origin, node->bounds.x, node->bounds.y, node->bounds.z);
	for (int i = 0; i < 8; i++) {
		if (node->children[i] != nullptr)
			draw(node->children[i]);
	}
}

void MeshOctree::drawTriangles() const {
	for (auto& tri : debugCollection)
		tri.draw();
}