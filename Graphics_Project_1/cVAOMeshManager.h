#ifndef _cVAOMeshManager_HG_
#define _cVAOMeshManager_HG_

#include <string>
#include <map>

class cMesh;

struct sVAOInfo
{
	sVAOInfo() : VAO_ID(0), shaderID(-1),
		vertex_buffer_ID(0), index_buffer_ID(0),
		numberOfVertices(0), numberOfTriangles(0),
		numberOfIndices(0),
		scaleForUnitBBox(1.0f)
	{ }

	std::string friendlyName;

	unsigned int VAO_ID;	

	unsigned int numberOfVertices;
	unsigned int numberOfTriangles;
	unsigned int numberOfIndices;


	int shaderID;			
	unsigned int vertex_buffer_ID;		// GLuint
	unsigned int index_buffer_ID;

	float scaleForUnitBBox;
};

// This will handle: 
// - taking a cMesh and loading into the various buffers
// - take a mesh "name" and returning the desired VAO info 
//   (for rendering)
class cVAOMeshManager
{
public:
	cVAOMeshManager();
	~cVAOMeshManager();
	// Take name from mesh for lookup (for rendering)
	bool loadMeshIntoVAO(cMesh &theMesh, int shaderID);

	// During rendering (aka 'drawing'), get the info we need.
	bool lookupVAOFromName(std::string name, sVAOInfo &theVAOInfo);

	std::map<int, std::string> mapNumberToName;

private:
	// Loop up from name to sVAOInfo
	// 1st is what I'm indexing by (i.e. type)
	// 2nd is what I'm actually storing (the type)
	std::map< std::string, sVAOInfo > m_mapNameToVAO;

	//	sVAOInfo theVAOS[10];
};


#endif
