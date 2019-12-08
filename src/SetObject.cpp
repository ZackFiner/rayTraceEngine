#include "SetObject.h"
#include "Ray.h"

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
bool MeshTreeNode::intersect(const glm::vec3& pt0, const glm::vec3& pt1, const glm::vec3& pt2) {


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
 * Code is adapted from Kevin Smith's implementation in Box.cc
 */
bool MeshTreeNode::intersect(const Ray &r) {
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

bool MeshTreeNode::intersect(const glm::vec3& _origin, const glm::vec3& _bounds) {
	glm::vec3 max = origin + bounds * 0.5f;
	glm::vec3 min = origin - bounds * 0.5f;
	glm::vec3 o_max = _origin + _bounds * 0.5f;
	glm::vec3 o_min = _origin - _bounds * 0.5f;

	return ((max.x >= o_min.x && min.x <= o_max.x) ||
			(max.y >= o_min.y && min.y <= o_max.y) ||
			(max.z >= o_min.z && min.z <= o_max.z));
}