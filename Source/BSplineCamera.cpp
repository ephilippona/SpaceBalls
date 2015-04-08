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
	deltapos = 25.0f;

}

BSplineCamera::~BSplineCamera()
{
}

void BSplineCamera::Update(float dt)
{
	
	//Speed Control
	//-------------------------------------------------------------------------------------------------------------------------------
    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_RIGHT ) == GLFW_PRESS && (mSpeed<=4.0f))//Increase Camera Speed
	{
		mSpeed = mSpeed + 0.01f;
		if(mSpeed>4.0f)
		{
			mSpeed=4.0f;
		}
	}

	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_LEFT ) == GLFW_PRESS && (mSpeed>0.0f))//Decrease Camera Speed
	{
		mSpeed = mSpeed - 0.01f;
		if(mSpeed<0.0f)
		{
			mSpeed=0.0f;
		}
	}

	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_S ) == GLFW_PRESS)//Stop Camera
	{
		mSpeed = 0.0f;
	}
	//--------------------------------------------------------------------------------------------------------------------------------
	
	//Position and Zoom Control
	//---------------------------------------------------------------------------------------------------------------------------------
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_UP ) == GLFW_PRESS && (deltapos>=9.0f)) //Zoom In
	{
		deltapos = deltapos - 0.05f;
	}

	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_DOWN ) == GLFW_PRESS && (deltapos<=28.0f)) //Zoom out
	{
		deltapos = deltapos + 0.05f;
	}

	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D ) == GLFW_PRESS ) //Default Zoom
	{
		deltapos = 25.0f;
	}
	//----------------------------------------------------------------------------------------------------------------------------------


	//Testing values
	std::cout << "mSpeed = "<<mSpeed<<"                     ";
	std::cout << "DeltaPos = "<<deltapos<<std::endl;

	mSplineParameterT = mSplineParameterT + (0.3 * mSpeed * dt);
	mPosition = mSpline->GetPosition(mSplineParameterT) + deltapos;

	mLookAt = glm::normalize(mSpline->GetTangent(mSplineParameterT));

	glm::vec3 right = glm::normalize(glm::cross(mLookAt, vec3(0,1,0)));
	mUp = glm::normalize(glm::cross(right, mLookAt));
	
}

glm::mat4 BSplineCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mLookAt, mUp);
}
