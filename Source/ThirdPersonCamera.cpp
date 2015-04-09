//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 28/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "ThirdPersonCamera.h"
#include "EventManager.h"
#include "World.h"
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <GLFW/glfw3.h>
#include <algorithm>


using namespace glm;


ThirdPersonCamera::ThirdPersonCamera(Model* targetModel)
    : Camera(), mTargetModel(targetModel), mHorizontalAngle(0.0f), mVerticalAngle(0.0f), mRadius(2.0f)

{
    assert(mTargetModel != nullptr);
    CalculateCameraBasis();
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::CalculateCameraBasis()
{
    float a = radians(mHorizontalAngle);
    float b = radians(mVerticalAngle);
    
    
    float Px = mRadius* cosf(b)*cosf(a);
    float Py = mRadius * sinf(b);
    float Pz = -mRadius * cosf(b)*sinf(a);
    
    mLookAt = normalize(vec3(-Px,-Py,-Pz));
    mRight = normalize(cross(vec3(0,-1,0), mLookAt));
    mUp = normalize(cross(mRight,mLookAt));

	mPosition = mTargetModel->GetPosition()+ vec3(Px,Py,Pz) - (2.0f * mRight);
}


void ThirdPersonCamera::Update(float dt)
{
    EventManager::DisableMouseCursor();
    
    
    //Compute new orientation from mouse mouvement
    mHorizontalAngle += EventManager::GetMouseMotionX() *dt ;
    mVerticalAngle +=  EventManager::GetMouseMotionY() *dt ;

    
    
    //Clamp Vertical angle between 0 and 85 degrees
    mVerticalAngle = clamp(mVerticalAngle, -50.0f, 50.0f);
    
    
    //
    if(mHorizontalAngle >180)
        mHorizontalAngle -= 360;
    else if (mHorizontalAngle <-180)
        mHorizontalAngle +=360;
    
   
    //================
    //Model Movement
    //================
        
	if (ControlsOn){
		// Press W to move Forward
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_W) == GLFW_PRESS){
			mTargetModel->SetPosition(mTargetModel->GetPosition() + mLookAt*25.0f*dt);
			speedColision = length(mLookAt)*25.0f*dt;
		}
		// Press S to move Backward
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_S) == GLFW_PRESS){
			mTargetModel->SetPosition(mTargetModel->GetPosition() - mLookAt*5.0f*dt);
			speedColision = length(mLookAt)*5.0f*dt;
		}
		// Press E to move Up
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_E) == GLFW_PRESS){
			mTargetModel->SetPosition(mTargetModel->GetPosition() + mUp*dt*5.0f);
			speedColision = length(mUp)*5.0f*dt;
		}
		// Press Q to move Down
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_Q) == GLFW_PRESS){
			mTargetModel->SetPosition(mTargetModel->GetPosition() - mUp*dt*5.0f);
			speedColision = length(mUp)*5.0f*dt;
		}
		// Press A to move Left
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_A) == GLFW_PRESS){
			mTargetModel->SetPosition(mTargetModel->GetPosition() - mRight*dt*5.0f);
			speedColision = length(mRight)*5.0f*dt;
		}
		// Press D to move Right
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D) == GLFW_PRESS){
			mTargetModel->SetPosition(mTargetModel->GetPosition() + mRight*dt*5.0f);
			speedColision = length(mRight)*5.0f*dt;

		}
	}

	
	
    // Aligns target model with the horizontal angle
    mTargetModel->SetRotation(vec3(0,1,0), mHorizontalAngle);
   

    
    
    
    CalculateCameraBasis();
}

glm::mat4 ThirdPersonCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mLookAt, mUp);
}

void ThirdPersonCamera::setControls(bool controls){

	ControlsOn = controls;
}