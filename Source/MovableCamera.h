//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 28/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"
#include "BSpline.h"

class MovableCamera : public Camera
{
public:
    MovableCamera(Model* targetModel);
    virtual ~MovableCamera();

    virtual void Update(float dt);
    virtual glm::mat4 GetViewMatrix() const;

private:
    virtual void CalculateCameraBasis();
    Model* mTargetModel;
    
    // Cartesian Coordinates
    float mHorizontalAngle;
    float mVerticalAngle;
    float mRadius;
    
    // Camera Vectors
    glm::vec3 mPosition;
    glm::vec3 mLookAt;
    glm::vec3 mRight;
    glm::vec3 mUp;

    // @TODO - Maybe you should add more camera attributes (Speed, etc)
	glm::vec3 mDirection; 
	float speed;
	float acceleration;
	float mouseSpeed;
	float verticalFOV;
	float horizontalFOV;
	static double lastTime;
	double currentTime;
	float deltaTime;
	double xpos;
	double ypos;
};
