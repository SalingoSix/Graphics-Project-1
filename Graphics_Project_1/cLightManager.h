#ifndef _CLIGHTMANAGER_HG_
#define _CLIGHTMANAGER_HG_

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <vector>

class cLight
{
public:
	cLight();
	~cLight();
	glm::vec3 position;
	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec4 specular;		// Colour (xyz), intensity (w)
	glm::vec3 attenuation;	// x = constant, y = linear, z = quadratic
	glm::vec3 direction;
	glm::vec4 typeParams;	// x = type, y = distance cut-off
							// z angle1, w = angle2
	int shaderlocID_position;
	int shaderlocID_diffuse;
	int shaderlocID_ambient;
	int shaderlocID_specular;
	int shaderlocID_attenuation;
	int shaderlocID_direction;
	int shaderlocID_typeParams;

	bool lightIsOn;
};

class cLightManager
{
public:
	cLightManager();
	~cLightManager();

	bool createLights();

	void LoadShaderUniformLocations(int shaderID);
	void CopyInfoToShader();

	std::vector<cLight> vecLights;
};


#endif
