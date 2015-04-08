

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
	return glm::perspective(55.0f, 4.0f / 3.0f, 0.1f, 2000.0f);
}
