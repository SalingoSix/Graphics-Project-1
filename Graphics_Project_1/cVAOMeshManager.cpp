#include <glad/glad.h>		// For all the OpenGL calls
#include <GLFW/glfw3.h>		// For all the OpenGL calls

#include "cVAOMeshManager.h"
#include "cMesh.h"
//cMesh.h contains the class cVertex_xyz_rgb_n now (also cTriangle)

cVAOMeshManager::cVAOMeshManager()
{
	return;
}

cVAOMeshManager::~cVAOMeshManager()
{
	return;
}

bool cVAOMeshManager::loadMeshIntoVAO(cMesh &theMesh, int shaderID)
{
	sVAOInfo theVAOInfo;

	// Create a Vertex Array Object (VAO)
	glGenVertexArrays(1, &(theVAOInfo.VAO_ID));
	glBindVertexArray(theVAOInfo.VAO_ID);


	glGenBuffers(1, &(theVAOInfo.vertex_buffer_ID));
	glBindBuffer(GL_ARRAY_BUFFER, theVAOInfo.vertex_buffer_ID);

	// Allocate the global vertex array
	cVertex_xyz_rgb_n* pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];

	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		pVertices[index].x = theMesh.pVertices[index].x;
		pVertices[index].y = theMesh.pVertices[index].y;
		pVertices[index].z = theMesh.pVertices[index].z;

		pVertices[index].r = theMesh.pVertices[index].r;
		pVertices[index].g = theMesh.pVertices[index].g;
		pVertices[index].b = theMesh.pVertices[index].b;

		pVertices[index].nx = theMesh.pVertices[index].nx;
		pVertices[index].ny = theMesh.pVertices[index].ny;
		pVertices[index].nz = theMesh.pVertices[index].nz;
	}

	// Copy the local vertex array into the GPUs memory
	int sizeOfGlobalVertexArrayInBytes = sizeof(cVertex_xyz_rgb_n) * theMesh.numberOfVertices;
	glBufferData(GL_ARRAY_BUFFER,
		sizeOfGlobalVertexArrayInBytes,		// sizeof(vertices), 
		pVertices,
		GL_STATIC_DRAW);

	// Get rid of local vertex array
	delete[] pVertices;
	// ******************************************************************************
	// ******************************************************************************

	glGenBuffers(1, &(theVAOInfo.index_buffer_ID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theVAOInfo.index_buffer_ID); // It's now an Index Buffer

																	   // Make a temporary array that matches what will be on the GPU...
																	   // (GPU expects a 1D array of integers)

																	   // Number of indices = number of tris x 3 (3 indices per triangle)
	int numberOfIndices = theMesh.numberOfTriangles * 3;

	//unsigned int indexArray[ 10000000 ];		// CAN'T DO THIS, unfortunately
	unsigned int* indexArray = new unsigned int[numberOfIndices];

	// Copy the triangle data into this 1D array 
	int triIndex = 0;		// Index into the triangle array (from mesh)
	int indexIndex = 0;		// Index into the "vertex index" array (1D)
	for (; triIndex < theMesh.numberOfTriangles; triIndex++, indexIndex += 3)
	{
		indexArray[indexIndex + 0] = theMesh.pTriangles[triIndex].vertex_ID_0;
		indexArray[indexIndex + 1] = theMesh.pTriangles[triIndex].vertex_ID_1;
		indexArray[indexIndex + 2] = theMesh.pTriangles[triIndex].vertex_ID_2;
	}
	// note number of indices is number of triangles x 3
	int sizeOfIndexArrayInBytes = sizeof(unsigned int) * numberOfIndices;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,		// "index buffer" or "vertex index buffer"
		sizeOfIndexArrayInBytes,
		indexArray,
		GL_STATIC_DRAW);

	// Don't need local array anymore (like the on in CPU RAM)
	delete[] indexArray;

	// At this point, the vertex and index buffers are stored on GPU

	// Now set up the vertex layout (for this shader)

	GLuint vpos_location = glGetAttribLocation(shaderID, "vPos");		// program, "vPos");	// 6
	GLuint vcol_location = glGetAttribLocation(shaderID, "vCol");		// program, "vCol");	// 13
	GLuint vnorm_location = glGetAttribLocation(shaderID, "vNorm");		// program, "vCol");	// 13

																		//	vec3 vPos,		x = 0th location in this class
																		//	vec3 vCol       r = 3rd location in this class
	glEnableVertexAttribArray(vpos_location);
	const int offsetOf_x_into_cVertex = 0;	// X is 0th location in cVertex
	glVertexAttribPointer(vpos_location,
		3,				// now vec3, not vec2   
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 9,	// cVertex is 6 floats in size
							// or you could do: sizeof( cVertex ), 
							//	  (void*) (sizeof(float) * offsetOf_x_into_cVertex) );		
		(void*)offsetof(cVertex_xyz_rgb_n, x));


	glEnableVertexAttribArray(vcol_location);
	const int offsetOf_r_into_cVertex = 3;	// "r" is 3rd float into cVertex
	glVertexAttribPointer(vcol_location,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 9,		// (  sizeof(cVertex)  );
								//  (void*) (sizeof(float) * offsetOf_r_into_cVertex));
		(void*)offsetof(cVertex_xyz_rgb_n, r));

	// Now we also have normals on the vertex
	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 9,		// (  sizeof(cVertex)  );
								//  (void*) (sizeof(float) * offsetOf_r_into_cVertex));
		(void*)offsetof(cVertex_xyz_rgb_n, nx));

	// Copy the information into the VAOInfo structure
	theVAOInfo.numberOfIndices = theMesh.numberOfTriangles * 3;
	theVAOInfo.numberOfTriangles = theMesh.numberOfTriangles;
	theVAOInfo.numberOfVertices = theMesh.numberOfVertices;
	theVAOInfo.friendlyName = theMesh.name;
	theVAOInfo.shaderID = shaderID;


	theMesh.CalculateExtents();
	theVAOInfo.scaleForUnitBBox = theMesh.scaleForUnitBBox;

	// Store the VAO info by mesh name
	this->m_mapNameToVAO[theMesh.name] = theVAOInfo;

	//std::string myArray[10];
	//myArray[3] = "Michael"

	// CRITICAL 
	// Unbind the VAO first!!!!
	glBindVertexArray(0);	// 

							// Unbind (release) everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(vcol_location);
	glDisableVertexAttribArray(vpos_location);

	return true;
}

bool cVAOMeshManager::lookupVAOFromName(std::string name, sVAOInfo &theVAOInfo)
{
	// look up in map for the name of the mesh we want to draw

	//	std::map< std::string, sVAOInfo > m_mapNameToVAO;
	// "Interator" is a class that can access inside a container
	std::map< std::string, sVAOInfo >::iterator itVAO = this->m_mapNameToVAO.find(name);

	// Did I find something?
	if (itVAO == this->m_mapNameToVAO.end())
	{	// ON NO! we DIDN'T!
		return false;
	}
	// DID find what we were looking for, so 
	//	ISN'T pointing to the "end()" built-in iterator
	theVAOInfo = itVAO->second;		// Because the "second" thing is the sVAO
}