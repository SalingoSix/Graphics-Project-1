#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SceneHandler.h"
#include "cShaderManager.h"
#include "cMesh.h"
#include "cVAOMeshManager.h"
#include "cGameObject.h"
#include "cLightManager.h"

//Camera variables
float angle = 0.0f;
float lookx = 0.0f, looky = 1.0f, lookz = -1.0f;
float camPosx = 0.0f, camPosz = 5.0f;
glm::vec3 g_cameraXYZ = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 g_cameraTarget_XYZ = glm::vec3(0.0f, 1.0f, 0.0f);

//Global handlers for shader, VAOs, game objects and lights
cShaderManager* g_pShaderManager = new cShaderManager();
cVAOMeshManager* g_pVAOManager = new cVAOMeshManager();
std::vector <cGameObject*> g_vecGameObject;
cLightManager* g_pLightManager = new cLightManager();

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	//Space turns the blue guy (or whatever models was loaded second) into a wireframe and back
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		::g_vecGameObject[1]->wireFrame = !::g_vecGameObject[1]->wireFrame;
	
	//Keys 1 through 8 will turn on and off the 8 point lights on the scene
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[0].lightIsOn = !(::g_pLightManager->vecLights[0].lightIsOn);
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[1].lightIsOn = !(::g_pLightManager->vecLights[1].lightIsOn);
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[2].lightIsOn = !(::g_pLightManager->vecLights[2].lightIsOn);
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[3].lightIsOn = !(::g_pLightManager->vecLights[3].lightIsOn);
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[4].lightIsOn = !(::g_pLightManager->vecLights[4].lightIsOn);
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[5].lightIsOn = !(::g_pLightManager->vecLights[5].lightIsOn);
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[6].lightIsOn = !(::g_pLightManager->vecLights[6].lightIsOn);
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[7].lightIsOn = !(::g_pLightManager->vecLights[7].lightIsOn);

	const float CAMERASPEED = 0.03f;
	switch (key)
	{
	//MOVEMENT USING TANK CONTROLS WASD
	case GLFW_KEY_A:		// Look Left
		angle -= CAMERASPEED * 3;
		lookx = sin(angle);
		lookz = -cos(angle);
		break;
	case GLFW_KEY_D:		// Look Right
		angle += CAMERASPEED * 3;
		lookx = sin(angle);
		lookz = -cos(angle);
		break;
	case GLFW_KEY_W:		// Move Forward (relative to which way you're facing)
		camPosx += lookx * 0.1f;
		camPosz += lookz * 0.1f;
		break;
	case GLFW_KEY_S:		// Move Backward
		camPosx -= lookx * 0.1f;
		camPosz -= lookz * 0.1f;
		break;
	case GLFW_KEY_DOWN:		// Look Down (along y axis)
		if (g_cameraTarget_XYZ.y > 0.5)	//Up and down looking range is limited
			looky -= CAMERASPEED;
		break;
	case GLFW_KEY_UP:		// Look Up (along y axis)
		if (g_cameraTarget_XYZ.y < 1.5)
			looky += CAMERASPEED;
		break;
	}
}

int main()
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1080, 720,
		"Crappy Party",
		NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	::g_pShaderManager->setBasePath("assets//shaders//");

	if (!::g_pShaderManager->createProgramFromFile(
		"simpleShader", vertShader, fragShader))
	{
		std::cout << "Failed to create shader program. Shutting down." << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		return -1;
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;
	GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	std::string* plyDirects;
	std::string* plyNames;
	int numPlys;

	if (!readPlysToLoad(&plyDirects, &plyNames, &numPlys))
	{
		std::cout << "Couldn't find files to read. Giving up hard.";
		return -1;
	}

	for (int i = 0; i < numPlys; i++)
	{	//Load each file into a VAO object
		cMesh newMesh;
		newMesh.name = plyNames[i];
		if (!LoadPlyFileIntoMesh(plyDirects[i], newMesh))
		{
			std::cout << "Didn't load model" << std::endl;
		}
		if (!::g_pVAOManager->loadMeshIntoVAO(newMesh, shaderID))
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}

	int numEntities;
	if (!readEntityDetails(&g_vecGameObject, &numEntities))
	{
		std::cout << "Unable to find game objects for placement." << std::endl;
	}


	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	GLint uniLoc_mModel = glGetUniformLocation(currentProgID, "mModel");
	GLint uniLoc_mView = glGetUniformLocation(currentProgID, "mView");
	GLint uniLoc_mProjection = glGetUniformLocation(currentProgID, "mProjection");

	GLint uniLoc_materialDiffuse = glGetUniformLocation(currentProgID, "materialDiffuse");

	::g_pLightManager->createLights();	// There are 10 lights in the shader
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);

	glEnable(GL_DEPTH);

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glm::mat4x4 m, p;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		::g_pLightManager->CopyInfoToShader();

		unsigned int sizeOfVector = ::g_vecGameObject.size();	//*****//
		for (int index = 0; index != sizeOfVector; index++)
		{
			// Is there a game object? 
			if (::g_vecGameObject[index] == 0)	//if ( ::g_GameObjects[index] == 0 )
			{	// Nothing to draw
				continue;		// Skip all for loop code and go to next
			}

			// Was near the draw call, but we need the mesh name
			std::string meshToDraw = ::g_vecGameObject[index]->meshName;		//::g_GameObjects[index]->meshName;

			sVAOInfo VAODrawInfo;
			if (::g_pVAOManager->lookupVAOFromName(meshToDraw, VAODrawInfo) == false)
			{	// Didn't find mesh
				continue;
			}

			m = glm::mat4x4(1.0f);	

			glm::mat4 matPreRotX = glm::mat4x4(1.0f);
			matPreRotX = glm::rotate(matPreRotX, ::g_vecGameObject[index]->orientation.x,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotX;
			
			glm::mat4 matPreRotY = glm::mat4x4(1.0f);
			matPreRotY = glm::rotate(matPreRotY, ::g_vecGameObject[index]->orientation.y,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotY;
			
			glm::mat4 matPreRotZ = glm::mat4x4(1.0f);
			matPreRotZ = glm::rotate(matPreRotZ, ::g_vecGameObject[index]->orientation.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotZ;

			glm::mat4 trans = glm::mat4x4(1.0f);
			trans = glm::translate(trans,
				::g_vecGameObject[index]->position);
			m = m * trans;

			glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
			matPostRotZ = glm::rotate(matPostRotZ, ::g_vecGameObject[index]->orientation2.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPostRotZ;

			glm::mat4 matPostRotY = glm::mat4x4(1.0f);
			matPostRotY = glm::rotate(matPostRotY, ::g_vecGameObject[index]->orientation2.y,
				glm::vec3(0.0f, 1.0f, 0.0f));
			m = m * matPostRotY;


			glm::mat4 matPostRotX = glm::mat4x4(1.0f);
			matPostRotX = glm::rotate(matPostRotX, ::g_vecGameObject[index]->orientation2.x,
				glm::vec3(1.0f, 0.0f, 0.0f));
			m = m * matPostRotX;

			float finalScale = ::g_vecGameObject[index]->scale;
			glm::mat4 matScale = glm::mat4x4(1.0f);
			matScale = glm::scale(matScale,
				glm::vec3(finalScale,
						finalScale,
						finalScale));
			m = m * matScale;

			p = glm::perspective(0.6f,			// FOV
				ratio,		// Aspect ratio
				0.1f,			// Near (as big as possible)
				1000.0f);	// Far (as small as possible)

							// View or "camera" matrix
			glm::mat4 v = glm::mat4(1.0f);	// identity

			g_cameraXYZ.x = camPosx;
			g_cameraXYZ.z = camPosz;

			g_cameraTarget_XYZ.x = camPosx + lookx;
			g_cameraTarget_XYZ.y = looky;
			g_cameraTarget_XYZ.z = camPosz + lookz;

			v = glm::lookAt(g_cameraXYZ,						// "eye" or "camera" position
				g_cameraTarget_XYZ,		// "At" or "target" 
				glm::vec3(0.0f, 1.0f, 0.0f));	// "up" vector


			glUniform4f(uniLoc_materialDiffuse,
				::g_vecGameObject[index]->diffuseColour.r,
				::g_vecGameObject[index]->diffuseColour.g,
				::g_vecGameObject[index]->diffuseColour.b,
				::g_vecGameObject[index]->diffuseColour.a);


			//        glUseProgram(program);
			::g_pShaderManager->useShaderProgram("simpleShader");

			glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(m));

			glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(v));

			glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(p));

			if(::g_vecGameObject[index]->wireFrame)
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(VAODrawInfo.VAO_ID);

			glDrawElements(GL_TRIANGLES,
				VAODrawInfo.numberOfIndices,
				GL_UNSIGNED_INT,
				0);

			glBindVertexArray(0);

		}//for ( int index = 0...

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}