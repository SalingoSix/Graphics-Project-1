#include <string>
#include <fstream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "cMesh.h"
#include "cGameObject.h"

//Places the file directories of all plys to be loaded into a string, and returns it to the main function.
//File will look like
//4							<- number of files to be loaded
//**/**/**.ply plyname1		<- relative directory for each file, and the friendly name for that ply
//**/**/**.ply plyname2
//**/**/**.ply plyname3
//**/**/**.ply plyname4
bool readPlysToLoad(std::string** myPlys, std::string** plyNames, int* length)
{
	std::ifstream plyFiles("PlysToLoad.txt");
	if (!plyFiles.is_open())
		return 0;			//If we can't load plys, send a signal to shut the whole thing down

	std::string curLine;

	plyFiles >> curLine;	//This line will determine the amount of ply files that will be loaded
	int numOfPlys = stoi(curLine);

	std::string* thePlys = new std::string[numOfPlys];
	std::string* theNames = new std::string[numOfPlys];
	
	for (int i = 0; i < numOfPlys; i++)
	{
		plyFiles >> thePlys[i];
		plyFiles >> theNames[i];
	}
		
	*length = numOfPlys;
	*myPlys = thePlys;
	*plyNames = theNames;
	return 1;
}

//For each entry in this file, a new model will be placed into the scenes based on the specifications laid out within
bool readEntityDetails(std::vector <cGameObject*>* myObjs, int* length)
{
	std::ifstream Entities("EntityDetails.txt");
	if (!Entities.is_open())
		return 0;			//If we can't load plys, send a signal to shut the whole thing down
	std::string curLine;
	Entities >> curLine;
	*length = stoi(curLine);
	for (int i = 0; i < *length; i++)
	{
		cGameObject* newObj = new cGameObject();

		Entities >> curLine;
		newObj->meshName = curLine;

		Entities >> curLine; //TRANSLATION
		Entities >> curLine;
		newObj->position.x = stof(curLine);
		Entities >> curLine;
		newObj->position.y = stof(curLine);
		Entities >> curLine;
		newObj->position.z = stof(curLine);

		Entities >> curLine; //PREROTATION
		Entities >> curLine;
		newObj->orientation.x = glm::radians(stof(curLine));
		Entities >> curLine;
		newObj->orientation.y = glm::radians(stof(curLine));
		Entities >> curLine;
		newObj->orientation.z = glm::radians(stof(curLine));

		Entities >> curLine; //POSTROTATION
		Entities >> curLine;
		newObj->orientation2.x = glm::radians(stof(curLine));
		Entities >> curLine;
		newObj->orientation2.y = glm::radians(stof(curLine));
		Entities >> curLine;
		newObj->orientation2.z = glm::radians(stof(curLine));

		Entities >> curLine; //SCALE
		Entities >> curLine;
		newObj->scale = stof(curLine);

		Entities >> curLine; //COLOUR
		Entities >> curLine;
		newObj->diffuseColour.x = stof(curLine)/255;
		Entities >> curLine;
		newObj->diffuseColour.y = stof(curLine)/255;
		Entities >> curLine;
		newObj->diffuseColour.z = stof(curLine)/255;

		myObjs->push_back(newObj);
	}
	return 1;
}

void ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}


// Takes a file name, loads a mesh
bool LoadPlyFileIntoMesh(std::string filename, cMesh &theMesh)
{
	std::ifstream plyFile(filename.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(plyFile, "vertex");
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{		
		float x, y, z, nx, ny, nz;

		plyFile >> x;
		plyFile >> y;
		plyFile >> z;
		plyFile >> nx;
		plyFile >> ny;
		plyFile >> nz;


		theMesh.pVertices[index].x = x;
		theMesh.pVertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;
		theMesh.pVertices[index].g = 1.0f;
		theMesh.pVertices[index].b = 1.0f;
		theMesh.pVertices[index].nx = nx;
		theMesh.pVertices[index].ny = ny;
		theMesh.pVertices[index].nz = nz;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		int discard = 0;
		plyFile >> discard;								
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	
	}

	return true;
}

bool LoadPlyFileIntoMeshNoNormals(std::string filename, cMesh &theMesh)
{
	std::ifstream plyFile(filename.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}

	ReadFileToToken(plyFile, "vertex");
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken(plyFile, "face");
	plyFile >> theMesh.numberOfTriangles;

	ReadFileToToken(plyFile, "end_header");

	// Allocate the appropriate sized array (+a little bit)
	theMesh.pVertices = new cVertex_xyz_rgb_n[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for (int index = 0; index < theMesh.numberOfVertices; index++)
	{
		float x, y, z;

		plyFile >> x;
		plyFile >> y;
		plyFile >> z;

		theMesh.pVertices[index].x = x;
		theMesh.pVertices[index].y = y;
		theMesh.pVertices[index].z = z;
		theMesh.pVertices[index].r = 1.0f;
		theMesh.pVertices[index].g = 1.0f;
		theMesh.pVertices[index].b = 1.0f;
	}

	// Load the triangle (or face) information, too
	for (int count = 0; count < theMesh.numberOfTriangles; count++)
	{
		int discard = 0;
		plyFile >> discard;
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;
	}

	theMesh.CalculateNormals();

	return true;
}