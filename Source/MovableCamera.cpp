// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday

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
}

MovableCamera::~MovableCamera()
{
}

void MovableCamera::CalculateCameraBasis()
{
	// @TODO
    // Calculate Camera Vectors (LookAt, Up, Right) from Spherical Coordinates
    // Convert from Spherical to Cartesian Coordinates to get the lookAt Vector
	
	// Direction 
	mDirection = glm::vec3(
		cos(mVerticalAngle) * sin(mHorizontalAngle), 
		sin(mVerticalAngle),
		cos(mVerticalAngle) * cos(mHorizontalAngle)
	);

	//Position
	mPosition = mTargetModel -> GetPosition() + mDirection * (-50.0f);

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
    //Initial values
	mouseSpeed = 0.005f;
	
	//Mouse
	// Get mouse position
	glfwGetCursorPos(EventManager::GetWindow(), &xpos, &ypos);
	// Reset mouse position for next frame
	glfwSetCursorPos(EventManager::GetWindow(), 1024/2, 768/2);

	
	// Compute new orientation
	if(mVerticalAngle >=-85 && mVerticalAngle <= 85)
	{
	mVerticalAngle   += mouseSpeed * float( 768/2 - ypos );
	}
	mHorizontalAngle += mouseSpeed * float(1024/2 - xpos );
	
	CalculateCameraBasis();
}

glm::mat4 MovableCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mLookAt, mUp);
}
