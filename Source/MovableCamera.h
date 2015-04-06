// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday

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
	float mouseSpeed;
	double xpos;
	double ypos;
	
};
