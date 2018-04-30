#include "cLightManager.h"

#include <fstream>
#include <string>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

cLight::cLight()
{
	this->position = glm::vec3(0.0f);

	this->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	this->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	// Colour (xyz), intensity (w)
	this->specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// x = constant, y = linear, z = quadratic
	this->attenuation = glm::vec3(0.0f);
	this->attenuation.x = 0.0f;	// Constant atten
	this->attenuation.y = 1.0f;	// Linear
	this->attenuation.z = 0.0f;	// Quadratic

	this->direction = glm::vec3(0.0f);

	// x = type, y = distance cut-off, z angle1, w = angle2
	this->typeParams = glm::vec4(0.0f);
	// Set distance to infinity
	// (i.e. at this point, the light won't be calculated)
	this->typeParams.y = 1000000.0f;		// Huge number

	this->shaderlocID_position = -1;
	this->shaderlocID_diffuse = -1;
	this->shaderlocID_ambient = -1;
	this->shaderlocID_specular = -1;
	this->shaderlocID_attenuation = -1;
	this->shaderlocID_direction = -1;
	this->shaderlocID_typeParams = -1;

	this->lightIsOn = true;

	return;
}

cLight::~cLight()
{
	return;
}

cLightManager::cLightManager()
{
	return;
}

cLightManager::~cLightManager()
{
	return;
}

//Reads the number of lights, and the position of each light from a text file.
//All lights default to white, only the position is manipulated in the file
bool cLightManager::createLights()
{
	std::ifstream lightDetails("LightDetails.txt");
	std::string readLine;
	int numLights;
	if (!lightDetails.is_open())
		return 0;
	lightDetails >> readLine;
	numLights = stoi(readLine);

	this->vecLights.resize(numLights);

	for (int i = 0; i < numLights; i++)
	{
		cLight newLight;
		glm::vec3 lightPos;

		lightDetails >> readLine;
		lightPos.x = stof(readLine);
		lightDetails >> readLine;
		lightPos.y = stof(readLine);
		lightDetails >> readLine;
		lightPos.z = stof(readLine);

		newLight.position = lightPos;
		this->vecLights[i] = newLight;
	}
	return 1;
}

std::string genUniName(int index, std::string attr)
{
	std::stringstream uniName;
	uniName << "myLight[" << index << "]." << attr;
	return uniName.str();
}

void cLightManager::LoadShaderUniformLocations(int shaderID)
{
	for (int i = 0; i < this->vecLights.size(); i++)
	{
		this->vecLights[i].shaderlocID_position = glGetUniformLocation(shaderID, genUniName(i, "position").c_str());
		this->vecLights[i].shaderlocID_ambient = glGetUniformLocation(shaderID, genUniName(i, "ambient").c_str());
		this->vecLights[i].shaderlocID_specular = glGetUniformLocation(shaderID, genUniName(i, "specular").c_str());
		this->vecLights[i].shaderlocID_attenuation = glGetUniformLocation(shaderID, genUniName(i, "attenuation").c_str());
		this->vecLights[i].shaderlocID_direction = glGetUniformLocation(shaderID, genUniName(i, "direction").c_str());
		this->vecLights[i].shaderlocID_diffuse = glGetUniformLocation(shaderID, genUniName(i, "diffuse").c_str());
		this->vecLights[i].shaderlocID_typeParams = glGetUniformLocation(shaderID, genUniName(i, "typeParams").c_str());
	}
}

void cLightManager::CopyInfoToShader()
{
	for (int i = 0; i < this->vecLights.size(); i++)
	{
		
		cLight thisLight = this->vecLights[i];
		// Each light has a boolean "lightIsOn", if it's false, make the light black
		if (!thisLight.lightIsOn)
			thisLight.diffuse = glm::vec3(0.0f);

		glUniform4f(thisLight.shaderlocID_position,
			thisLight.position.x,
			thisLight.position.y,
			thisLight.position.z,
			1.0f);

		glUniform4f(thisLight.shaderlocID_diffuse,
			thisLight.diffuse.r,
			thisLight.diffuse.g,
			thisLight.diffuse.b,
			1.0f);

		glUniform4f(thisLight.shaderlocID_ambient,
			thisLight.ambient.r,
			thisLight.ambient.g,
			thisLight.ambient.b,
			1.0f);

		glUniform4f(thisLight.shaderlocID_specular,
			thisLight.specular.r,
			thisLight.specular.g,
			thisLight.specular.b,
			thisLight.specular.w);

		glUniform4f(thisLight.shaderlocID_attenuation,
			thisLight.attenuation.x,
			thisLight.attenuation.y,
			thisLight.attenuation.z,
			1.0f);

		glUniform4f(thisLight.shaderlocID_direction,
			thisLight.direction.x,
			thisLight.direction.y,
			thisLight.direction.z,
			1.0f);

		// x = type, y = distance cut-off
		// z angle1, w = angle2		
		glUniform4f(thisLight.shaderlocID_typeParams,
			thisLight.typeParams.x,
			thisLight.typeParams.y,
			thisLight.typeParams.z,
			thisLight.typeParams.w);
	}
	return;
}