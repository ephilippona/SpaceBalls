// Written by Nicolas Bergeron - Initial implementation
// Modified by Christopher Maroday


#include "BSplineCamera.h"
#include "EventManager.h"
#include "World.h"
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>


using namespace glm;

BSplineCamera::BSplineCamera(BSpline* spline, float speed)
    : Camera(), mSpline(spline), mSpeed(speed), mSplineParameterT(0.0f)
{
    assert(spline != nullptr);
    mPosition = mSpline->GetPosition(mSplineParameterT);
}

BSplineCamera::~BSplineCamera()
{
}

void BSplineCamera::Update(float dt)
{
    // @TODO - Using the BSpline class, update the position on the spline
    //         Set the mLookAt vector pointing in front (tangent on spline)
    //         And mUp vector being as up as possible
	mSplineParameterT = mSplineParameterT + (0.3 * mSpeed * dt);
	mPosition = mSpline->GetPosition(mSplineParameterT)+25.0f;

	mLookAt = glm::normalize(mSpline->GetTangent(mSplineParameterT));

	glm::vec3 right = glm::normalize(glm::cross(mLookAt, vec3(0,1,0)));
	mUp = glm::normalize(glm::cross(right, mLookAt));
	
}

glm::mat4 BSplineCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mLookAt, mUp);
}
