// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday - Implemented mouse controls, vertical angle clamping, and zoom feature

#include "MovableCamera.h"
#include "EventManager.h"
#include "World.h"
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>


using namespace glm;


MovableCamera::MovableCamera(Model* targetModel)
    : Camera(), mTargetModel(targetModel), mHorizontalAngle(0.0f), mVerticalAngle(0.0f), mRadius(10.0f)

{
    assert(mTargetModel != nullptr);
    CalculateCameraBasis();

	//Variable used for zoom
	camDistance = -350.0f;
}

MovableCamera::~MovableCamera()
{
}

void MovableCamera::CalculateCameraBasis()
{
	
	// Direction 
	mDirection = glm::vec3(
		cos(mVerticalAngle) * sin(mHorizontalAngle), 
		sin(mVerticalAngle),
		cos(mVerticalAngle) * cos(mHorizontalAngle)
	);

	//Zoom In
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_UP ) == GLFW_PRESS && (camDistance <= -104.0f)) 
	{
		camDistance = camDistance + 0.6f;
	}

	//Zoom out
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_DOWN ) == GLFW_PRESS && (camDistance>= -1000.0f)) 
	{
		camDistance = camDistance - 0.6f;
	}
	
	//Default Zoom
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D ) == GLFW_PRESS ) 
	{
		camDistance = -350.0f;
	}

	//Position
	mPosition = mTargetModel -> GetPosition() + mDirection * camDistance;

	// LookAt 
	mLookAt = mDirection;

	// Right vector
	mRight = glm::cross(mLookAt, vec3(0,1,0)); 
	
	// Up vector
	mUp = glm::cross( mRight, mLookAt ); 

}

void MovableCamera::Update(float dt)
{
    EventManager::DisableMouseCursor();

    //Define Angles
	mHorizontalAngle += EventManager::GetMouseMotionX() *dt ;
    mVerticalAngle +=  EventManager::GetMouseMotionY() *dt ;

	//Clamp
	mVerticalAngle = clamp(mVerticalAngle, 1.65f, 4.65f);
	
	//Test values
	std::cout<<"Vertical Angle:  "<< mVerticalAngle <<"                        ";
	std::cout<<"Camera Distance:  "<< camDistance <<std::endl;

	//Function Call
	CalculateCameraBasis();
}

glm::mat4 MovableCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mLookAt, mUp);
}
