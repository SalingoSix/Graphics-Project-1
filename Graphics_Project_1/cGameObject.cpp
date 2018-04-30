#include "cGameObject.h"

cGameObject::cGameObject()
{
	this->scale = 1.0f;	// (not zero)
	this->position = glm::vec3(0.0f);
	this->orientation = glm::vec3(0.0f);
	this->orientation2 = glm::vec3(0.0f);
	this->wireFrame = false;

	this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);



	return;
}

cGameObject::~cGameObject()
{
	return;
}