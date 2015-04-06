// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday

#pragma once

#include "Camera.h"
#include "BSpline.h"

class BSplineCamera : public Camera
{
public:
    BSplineCamera(BSpline* spline, float speed);
    virtual ~BSplineCamera();

    virtual void Update(float dt);
    virtual glm::mat4 GetViewMatrix() const;

private:
    glm::vec3 mPosition;
    glm::vec3 mLookAt;
    glm::vec3 mUp;

    BSpline* mSpline;
    float mSplineParameterT;
    float mSpeed;
};