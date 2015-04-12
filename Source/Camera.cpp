// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday - Minor change to perspective to make entire solar system visible

#include "Camera.h"
#include <GLM/gtx/transform.hpp>

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
	
	glm::mat4 viewProjection = GetProjectionMatrix()*GetViewMatrix();
	return viewProjection;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(65.0f, 4.0f / 3.0f, 0.1f, 2500.0f);
}
