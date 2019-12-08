/*H******************************************************************
 * FILENAME: Mesh.cpp
 * AUTHOR: Zackary Finer
 * 
 * DESCRIPTION: 
 * this file contains the implementations of the various methods
 * contained within the Mesh class, as well as other
 * supporting classes.
 */


#include "Mesh.h"
vertexIndex::vertexIndex()
{}
vertexIndex::vertexIndex(int index):
	vert(index)
{}
vertexIndex::vertexIndex(const vertexIndex& o) :
	vert(o.vert),
	normal(o.normal),
	texCoord(o.texCoord)
{}

vertexIndex& vertexIndex::operator=(const vertexIndex& o)
{
	vert = o.vert;
	normal = o.normal;
	texCoord = o.texCoord;
	return *this;
}

/*
	Returns the vertex index as it would be represented in a .obj
	file.

	@return: std::string - A string of the .obj representation of the vertex indicie
*/
std::string vertexIndex::vertString()
{
	std::stringstream vertS;
	vertS << std::to_string(vert);
	if (texCoord != -1 || normal != -1)
	{
		if (texCoord != -1)
		{
			vertS << "/" << std::to_string(texCoord);
			if (normal != -1)
				vertS << "/" << std::to_string(normal);
		}
		else
		{
			vertS << "//" << std::to_string(normal);
		}
	}
	return vertS.str();
}

/*
	.obj constructor for mesh class.

	@param: std::string - filepath to the .obj to be loaded
*/
Mesh::Mesh(const std::string& _filePath)
	:filePath(_filePath)
{
	loadDataFromFile(filePath);
	computeTangents();
}
/*
	Default constructor for mesh.
*/
Mesh::Mesh()
	: filePath("")
{
}

/*
	Loads mesh data from a .obj file. Right now, this implementation will populate 3 seperate arrays:
	texCoords
	normals
	verts

	Of these, only 1 is used in the current application: verts. I have taken the time to parse
	the other 2 data points as they will prove to be important later on in this course.

	Indicies are represented as an integer tripple since the datapoints we have loaded in are in
	seperate arrays. However, we will really only use the first (the vertex index) of these integers.
	
	Ideally, we would merge these different arrays into one array of vertex objects, where each vertex
	contained a normal, texture coordinate, and ofcourse it's position, and just use integer indexes
	to refer to these vertexes. I have implemented this in a seperate project using hash tables
	and a bit more processing at mesh load.

	@param: std::string - filePath to the .obj that we want to load
*/
void Mesh::loadDataFromFile(const std::string& filePath)
{
	/*
	Things to consider:
	what do we do if a vertex doesn't have a normal? should we re-calculate it ourselves?
	*/
	struct face {
		std::vector<std::string> verts;
	};
	std::ifstream target(filePath);
	std::string line;
	/*std::unordered_map<std::string, int> vertMapper;*/
	std::vector<face> faces;
	int vert_c = 0;
	while (std::getline(target, line))
	{
		auto tokens = parseUtilities::split(line, " ", true);
		if (tokens.size() > 0)
		{
			if (tokens[0] == "v") // parse a point
			{
				if (tokens.size() >= 4)
				{
					verts.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
				}
			}
			else if (tokens[0] == "vt") // parse a texture coordinate
			{
				if (tokens.size() >= 3)
				{
					texCoords.push_back(glm::vec2(std::stof(tokens[1]), std::stof(tokens[2])));
				}
			}
			else if (tokens[0] == "vn") // parse a vertex normal
			{
				if (tokens.size() >= 4)
				{
					normals.push_back(glm::vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
				}
			}
			else if (tokens[0] == "f") // parse a face, we will assume that these are located after the point/texCoord/Normal definitions, and are tri
			{
				face cur_face;
				// for the indicies, they are formatted as pt/txC/nrm, or just pt, if a vertex has
				for (int i = 1; i < 4; i++)
				{
					const auto& token = tokens[i];
					auto subTokens = parseUtilities::split(token, "/", false);
					int type = 0;
					vertexIndex vI;
					for (auto subToken : subTokens)
					{
						switch (type)
						{
						case 0:
							vI.vert = std::stoi(subToken);
							break;
						case 1:
							vI.texCoord = std::stoi(subToken);
							break;
						case 2:
							vI.normal = std::stoi(subToken);
							break;
						}
						type++;
					}
					/*
					auto vertS = vI.vertString();
					cur_face.verts.push_back(vertS);

					if (vertMapper.find(vertS) == vertMapper.end())
					{
						vertMapper.try_emplace(vertS, vert_c++);
					}*/
					indicies.push_back(vI); //add the vertex index to our indicies
				}
				faces.push_back(cur_face);
			}
		}
	}
	/*
	You may have been wondering why i did all this nonsense with faces and vertMapper when they aren't used
	anywhere else in the code. The awnser is i'm not sure if the current method of loading vertex data is the best.
	When you pass vertex data to your gpu's pipeline, it is usually done by specifying an array of integers for indicies
	and a big floating point array for vertex data. now, this vertex data array is essentially a flattened array of verticies.
	a given verticy in this array could contain points, normals, and texture coordinates. the way in which this data is encoded/interpreted
	depends on how you configure your vertex attribute pointer, but all the data is represented as a flattened floating point array never the less.

	As such, in the current allocation scheme, if we wanted to flatten the vertex data, we would need to perform an operation similar
	to what was done with the vert mapper and faces data structures here: generate a unique set of verticies, and re-map our indicies
	to point to these unique vertices.
	
	Once this operation has been performed, flattening the data is relatively trivial, and still memory efficient.
	*/
	/*
	faces;
	vertMapper;*/
}

/*
	Draws the mesh in wireframe. This is done through the use of the ofDrawLine method, by drawing
	the 3 edges of the triangle. As such, this implementation draws duplicate edges.
*/
void Mesh::draw()
{
	//right now, we're only going to use the vertex part of our indicies, since ofDrawTriangle only takes 3 points
	//ofDisableDepthTest();
	for (int i = 0; i < indicies.size(); i += 3)
	{
		ofSetColor(ofColor::black);
		ofNoFill();
		ofDrawTriangle(verts[indicies[i].vert - 1], verts[indicies[i + 1].vert - 1], verts[indicies[i + 2].vert - 1]);
		//ofSetColor(ofColor::black);
		//ofDrawLine(verts[indicies[i].vert - 1], verts[indicies[i + 1].vert - 1]);
		//ofDrawLine(verts[indicies[i].vert - 1], verts[indicies[i + 2].vert - 1]);
		//ofDrawLine(verts[indicies[i + 1].vert - 1], verts[indicies[i + 2].vert - 1]);
	}
}

/*
	Returns the number of verticies in the mesh.

	@return: int - representing the number of verticies in the mesh.
*/
int Mesh::vertCount()
{
	return verts.size();
}

/*
	Returns the number of triangles in the mesh. This is calculated by dividing the count
	of the indicies array by 3 (since 3 indicies represents a triangle.

	@return: int - representing the number of triangles in the mesh
*/
int Mesh::triCount()
{
	return indicies.size() / 3;
}

/*
	Returns the size in bytes of the mesh data this object contains.
	This is calculated by getting the size of the vectors and multiplying
	them by the size of their contained data.

	@return: long - representing the size in bytes of the mesh data
*/
long Mesh::getByteSize()
{
	return sizeof(std::vector<glm::vec3>)+verts.size()*sizeof(glm::vec3)
		+ sizeof(std::vector<glm::vec3>) + normals.size() * sizeof(glm::vec3)
		+ sizeof(std::vector<vertexIndex>) + indicies.size() * sizeof(vertexIndex)
		+ sizeof(std::vector<glm::vec2>) + texCoords.size() * sizeof(glm::vec2)
		+ sizeof(Mesh);
}

/*
	Returns the file name, or file path of the .obj file that was loaded into
	this mesh data structure.

	@return: std::string - representing the filepath
*/
std::string& Mesh::fileName()
{
	return filePath;
}

/*
	Adds a vertex to the mesh, this vertex should be passed as a glm::vec3
	and represent a point in 3d space.

	@param vert: glm::vec3 - the vertex that will be added to the mesh
*/
void Mesh::addVert(const glm::vec3 & vert)
{
	verts.push_back(vert);
}

/*
	Adds a vertex indicie to the mesh.

	@param index: int - the index to be added to the mesh
*/
void Mesh::addIndex(int index)
{
	indicies.push_back(vertexIndex(index));
}


/*
 * This method generates a Tangent, Bitangent, and normals for each vertex in our mesh.
 * This is crucial to advanced texture rendering techniques like normal and bump mapping.
 * The algorithm and implementation below is based on the following sources:
 *
 *		Lengyel, E. (2019) Foundations of Game Engine Development Volume 2: Rendering.
 *		Lincoln, California: Terathon Software LLC.
 *
 *		http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
 */
void Mesh::computeTangents() {
	/*
		Right now, this assumes that every vertex indicie has at least a normal and texture coordinate
	*/
	std::unordered_map<string, vertexIndex*> uniqueIndexes;
	std::vector<glm::vec3> bitangents;
	int triangleCount = indicies.size() / 3;
	int tangentCount = 0;
	int bitangentCount = 0;
	for (int k = 0; k < triangleCount; k+=3) {
		auto ind0 = indicies[k];
		auto ind1 = indicies[k+1];
		auto ind2 = indicies[k+2];


		auto vert0 = verts[ind0.vert];
		auto vert1 = verts[ind1.vert];
		auto vert2 = verts[ind2.vert];
		auto tC0 = texCoords[ind0.texCoord];
		auto tC1 = texCoords[ind1.texCoord];
		auto tC2 = texCoords[ind2.texCoord];

		glm::vec3 e1 = vert1 - vert0;
		glm::vec3 e2 = vert2 - vert0;
		glm::vec2 tD1 = tC1 - tC0;
		glm::vec2 tD2 = tC2 - tC0;

		float r = 1.0f / (tD1.x*tD2.y - tD2.x*tD1.y);
		glm::vec3 t = (e1*tD2.y - e2 * tD1.y)*r;
		glm::vec3 b = (e2*tD1.x - e1 * tD2.x)*r;

		// it is here we arrive at an issue: the way our mesh is indexed is weird:
		// the article and sample code assumes that an index corresponds to specific
		// seperate texcoord and normal: our mesh isn't layed out this way.

		// As such, we need a way to deduce unique verticies (same pos, normal and texcoord)
		// so we can know when to average an already existing tangent and bitangent rather than
		// allocate a new one
		if (uniqueIndexes.find(ind0.vertString()) == uniqueIndexes.end()) {//if this vert has already been processed
			indicies[k].tangent = uniqueIndexes[ind0.vertString()]->tangent;
			indicies[k].bitangent = uniqueIndexes[ind0.vertString()]->bitangent;
			// we assign our tangent and bitangent on this 
			tangents[indicies[k].tangent] += t;
			bitangents[indicies[k].bitangent] += b;
			//then we average our tangent and bitangent
		} else {
			indicies[k].tangent = tangentCount;
			indicies[k].bitangent = tangentCount;
			tangents.push_back(glm::vec4(t, 0.0f));
			bitangents.push_back(b);
			tangentCount++;

			uniqueIndexes.insert_or_assign(ind0.vertString(), &indicies[k]);
		}
		if (uniqueIndexes.find(ind1.vertString()) == uniqueIndexes.end()) {//if this vert has already been processed
			indicies[k+1].tangent = uniqueIndexes[ind1.vertString()]->tangent;
			indicies[k+1].bitangent = uniqueIndexes[ind1.vertString()]->bitangent;
			// we assign our tangent and bitangent on this 
			tangents[indicies[k+1].tangent] += t;
			bitangents[indicies[k+1].bitangent] += b;
			//then we average our tangent and bitangent
		} else {
			indicies[k+1].tangent = tangentCount;
			indicies[k+1].bitangent = tangentCount;
			tangents.push_back(glm::vec4(t, 0.0f));
			bitangents.push_back(b);
			tangentCount++;

			uniqueIndexes.insert_or_assign(ind1.vertString(), &indicies[k+1]);
		}
		if (uniqueIndexes.find(ind2.vertString()) == uniqueIndexes.end()) {//if this vert has already been processed
			indicies[k+2].tangent = uniqueIndexes[ind2.vertString()]->tangent;
			indicies[k+2].bitangent = uniqueIndexes[ind2.vertString()]->bitangent;
			// we assign our tangent and bitangent on this 
			tangents[indicies[k+2].tangent] += t;
			bitangents[indicies[k+2].bitangent] += b;
			//then we average our tangent and bitangent
		} else {
			indicies[k+2].tangent = tangentCount;
			indicies[k+2].bitangent = tangentCount;
			tangents.push_back(glm::vec4(t, 0.0f));
			bitangents.push_back(b);
			tangentCount++;

			uniqueIndexes.insert_or_assign(ind2.vertString(), &indicies[k+2]);
		}

	}
	// finally, we "orthonormalize each tangent and calculate the handedness"
	for (int i = 0; i < indicies.size(); i++) {

		//if we haven't already processed this vertex
		if (uniqueIndexes[indicies[i].vertString()] != nullptr) {
			auto t = tangents[indicies[i].tangent];
			auto b = bitangents[indicies[i].bitangent];
			auto n = normals[indicies[i].normal];
			
			indicies[i].bitangent = -1; //we will be de-allocating bitangents at the end of this, so we set this to -1
			auto t_3 = glm::vec3(t.x, t.y, t.z);
			// third component is used to determine whether our tangent is left or right handed
			tangents[indicies[i].tangent] = glm::vec4(glm::normalize(t_3 - glm::dot(t_3, n)*n),
											(glm::dot(glm::cross(t_3, b), n)> 0.0f) ? 1.0f : -1.0f); 

			uniqueIndexes[indicies[i].vertString()] = nullptr;
		}
	}
	//bitangents have now been calculated.
}