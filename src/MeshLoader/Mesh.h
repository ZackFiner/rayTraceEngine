/*H******************************************************************
 * FILENAME: Mesh.h
 * AUTHOR: Zackary Finer
 *
 * DESCRIPTION:
 * header file for the Mesh class. this class is used to load
 * vertex/mesh data from a .obj file, and draw it on screen.
 */
#pragma once
#include "ofMain.h"
#include "parseUtilities.h"
#include <unordered_map>
class vertex
{
public:
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 tex;
};

class vertexIndex
{
public:
	vertexIndex();
	vertexIndex(int);
	vertexIndex(const vertexIndex&);
	vertexIndex& operator=(const vertexIndex&);
	int vert = -1, normal= -1, texCoord =-1, tangent = -1, bitangent= -1;
	std::string vertString();
};

/*
 * The Mesh class implemented below utilizes algorithms and techniques for calculating
 * and storing tangent and bitangent information as described in:
 *
 *		Lengyel, E. (2019) Foundations of Game Engine Development Volume 2: Rendering.
 *		Lincoln, California: Terathon Software LLC.
 *
 *		http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
 */
class Mesh
{
private:

	std::string filePath;
	// indicies represent triangles, i've flattened it in this case (instead of an array of tripples, just an array with every 3 elements being a tripple)
	
	void loadDataFromFile(const std::string&);
public:
	std::vector<glm::vec3> verts; // VERTEX ARRAY
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec4> tangents;
	
	void computeTangents(); // THIS NEEDS TESTING! TEST ME.
	std::vector<vertexIndex> indicies; // INDICIES ARRAY
	int vertCount();
	int triCount();
	Mesh(const std::string&);
	Mesh();
	void draw();
	std::string& fileName();
	long getByteSize();
	void addVert(const glm::vec3&);
	void addIndex(int);
	//void ofDraw();
};