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