// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday - Added float camDistance to be used for zoom

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
	float camDistance;
    
    // Camera Vectors
    glm::vec3 mPosition;
    glm::vec3 mLookAt;
    glm::vec3 mRight;
    glm::vec3 mUp;
	glm::vec3 mDirection; 
};
