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
    
	mHorizontalAngle += EventManager::GetMouseMotionX() *dt ;
    mVerticalAngle +=  EventManager::GetMouseMotionY() *dt ;

	mVerticalAngle = clamp(mVerticalAngle, 1.65f, 4.65f);
	
	//Test values
	std::cout<<"vertical angle:  "<< mVerticalAngle <<std::endl;


	CalculateCameraBasis();
}

glm::mat4 MovableCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mLookAt, mUp);
}
