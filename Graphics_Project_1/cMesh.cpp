#include "cMesh.h"

#include <glm/glm.hpp>		// cross product (I hope)

cMesh::cMesh()
{
	this->numberOfVertices = 0;
	this->numberOfTriangles = 0;
	//
	this->maxExtent = 0.0f;
	this->scaleForUnitBBox = 1.0f;
	return;
}


cMesh::~cMesh()
{

	return;
}

void cMesh::CalculateExtents(void)
{
	// Assume 1st vertex is both max and min
	this->minXYZ.x = this->pVertices[0].x;
	this->minXYZ.y = this->pVertices[0].y;
	this->minXYZ.z = this->pVertices[0].z;
	this->maxXYZ.x = this->pVertices[0].x;
	this->maxXYZ.y = this->pVertices[0].y;
	this->maxXYZ.z = this->pVertices[0].z;

	for (int index = 0; index != this->numberOfVertices; index++)
	{
		if (this->pVertices[index].x < this->minXYZ.x)
		{
			this->minXYZ.x = this->pVertices[index].x;
		}
		if (this->pVertices[index].x > this->maxXYZ.x)
		{
			this->maxXYZ.x = this->pVertices[index].x;
		}
		// y...
		if (this->pVertices[index].y < this->minXYZ.y)
		{
			this->minXYZ.y = this->pVertices[index].y;
		}
		if (this->pVertices[index].y > this->maxXYZ.y)
		{
			this->maxXYZ.y = this->pVertices[index].y;
		}
		// z...
		if (this->pVertices[index].z < this->minXYZ.z)
		{
			this->minXYZ.z = this->pVertices[index].z;
		}
		if (this->pVertices[index].z > this->maxXYZ.z)
		{
			this->maxXYZ.z = this->pVertices[index].z;
		}

	}//for ( int index...

	this->maxExtentXYZ.x = this->maxXYZ.x - this->minXYZ.x;
	this->maxExtentXYZ.y = this->maxXYZ.y - this->minXYZ.y;
	this->maxExtentXYZ.z = this->maxXYZ.z - this->minXYZ.z;

	this->maxExtent = this->maxExtentXYZ.x;
	if (this->maxExtent < this->maxExtentXYZ.y)
	{	// Y is bigger
		this->maxExtent = this->maxExtentXYZ.y;
	}
	if (this->maxExtent < this->maxExtentXYZ.z)
	{	// Z is bigger
		this->maxExtent = this->maxExtentXYZ.z;
	}
	//
	this->scaleForUnitBBox = 1.0f / this->maxExtent;

	return;
}

void cMesh::CalculateNormals(void)
{
	// Go through each triangle...
	// ...calculate normal per vertex (3 vertices)
	// ...ADD this normal to the corresponding vertex

	// 2nd pass, 
	// go through all the vertices and "normalize" them
	for (int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++)
	{
		this->pVertices[vertIndex].nx = 0.0f;
		this->pVertices[vertIndex].ny = 0.0f;
		this->pVertices[vertIndex].nz = 0.0f;
	}

	for (int triIndex = 0; triIndex != this->numberOfTriangles; triIndex++)
	{
		cTriangle curTri = this->pTriangles[triIndex];

		// Calculate normal for each vertex
		glm::vec3 vertA = glm::vec3(this->pVertices[curTri.vertex_ID_0].x,
			this->pVertices[curTri.vertex_ID_0].y,
			this->pVertices[curTri.vertex_ID_0].z);

		glm::vec3 vertB = glm::vec3(this->pVertices[curTri.vertex_ID_1].x,
			this->pVertices[curTri.vertex_ID_1].y,
			this->pVertices[curTri.vertex_ID_1].z);

		glm::vec3 vertC = glm::vec3(this->pVertices[curTri.vertex_ID_2].x,
			this->pVertices[curTri.vertex_ID_2].y,
			this->pVertices[curTri.vertex_ID_2].z);

		// Cross of A-B and A-C (normal at vertex A)
		glm::vec3 normVec0 = glm::normalize(glm::cross(vertB - vertA, vertC - vertA));

		// Cross of B-A and B-C (normal at vertex B)
		glm::vec3 normVec1 = glm::normalize(glm::cross(vertA - vertB, vertC - vertB));

		// Cross of C-A and C-B (normal at vertex C)
		glm::vec3 normVec2 = glm::normalize(glm::cross(vertA - vertC, vertB - vertC));

		// Add the values to the current normals (vert A)
		this->pVertices[curTri.vertex_ID_0].nx += normVec0.x;
		this->pVertices[curTri.vertex_ID_0].ny += normVec0.y;
		this->pVertices[curTri.vertex_ID_0].nz += normVec0.z;

		// Add the values to the current normals (vert B)
		this->pVertices[curTri.vertex_ID_1].nx += normVec1.x;
		this->pVertices[curTri.vertex_ID_1].ny += normVec1.y;
		this->pVertices[curTri.vertex_ID_1].nz += normVec1.z;

		// Add the values to the current normals (vert C)
		this->pVertices[curTri.vertex_ID_2].nx += normVec2.x;
		this->pVertices[curTri.vertex_ID_2].ny += normVec2.y;
		this->pVertices[curTri.vertex_ID_2].nz += normVec2.z;
	}//

	 // 2nd pass: normalize the normals
	for (int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++)
	{
		glm::vec3 norm = glm::vec3(this->pVertices[vertIndex].nx,
			this->pVertices[vertIndex].ny,
			this->pVertices[vertIndex].nz);
		// It's value DIV length
		glm::normalize(norm);

		this->pVertices[vertIndex].nx = norm.x;
		this->pVertices[vertIndex].ny = norm.y;
		this->pVertices[vertIndex].nz = norm.z;
	}

	return;
}