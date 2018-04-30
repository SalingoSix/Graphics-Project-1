#ifndef _cMesh_HG_
#define _cMesh_HG_

static const int MAX_VERTEX_ARRAY_SIZE = 4096;
static const int MAX_INDEX_ARRAY_SIZE = 4096;

#include <string>
#include <glm/vec3.hpp>

class cTriangle
{
public:
	int vertex_ID_0;
	int vertex_ID_1;
	int vertex_ID_2;
};

class cVertex_xyz_rgb_n
{
public:
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
};

class cMesh
{
public:
	cMesh();
	~cMesh();

	std::string name;

	cVertex_xyz_rgb_n* pVertices;
	int numberOfVertices;

	cTriangle* pTriangles;
	int numberOfTriangles;

	// First 2 are the "bounding box" edges
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	// This is Max - Min for each x,y,z
	glm::vec3 maxExtentXYZ;
	float maxExtent;		// Biggest of above
							// Scale can be 1.0 div maxExtent;
	float scaleForUnitBBox;

	void CalculateExtents(void);

	void CalculateNormals(void);
};


#endif