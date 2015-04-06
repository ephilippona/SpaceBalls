//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "Model.h"
#include "Path.h"
#include "BSpline.h"
#include "World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <GLM/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "EventManager.h"
#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;

Model::Model() : mName("UNNAMED"), mPosition(0.0f, 0.0f, 0.0f), mScaling(1.0f, 1.0f, 1.0f), mRotationAxis(0.0f, 1.0f, 0.0f), mRotationAngleInDegrees(0.0f), mPath(nullptr), mSpeed(0.0f), mTargetWaypoint(1), mSpline(nullptr), mSplineParameterT(0.0f), mNumChildren(0)
{
	blackHole = false;
	mDrawStyle = Standard;
	
	// Set some default lighting -- to override from scene file
	mMaterialCoefficients = vec4(0.25, 0.1, 0.6, 8.0);
}

Model::~Model()
{
}

void Model::Update(float dt)
{
    if(mPath != nullptr)
    {
		vec3 target = mPath->GetWaypoint(mTargetWaypoint);
		vec3 directionToTarget = target - mPosition;
		float distanceToTarget = length(directionToTarget);

		// Normalize direction and update direction
		directionToTarget = normalize(directionToTarget);
		float distance = mSpeed*dt;
		mPosition += distance * directionToTarget;

		// Update waypoint
		if (distance > distanceToTarget)
		{
			++mTargetWaypoint;
		}
    }
	else if (mSpline)
	{
		if (blackHole)
		{
			vec3 target = vec3(0, 0, 0);
			direction = target - mSpline->GetPosition(mSplineParameterT);
			float distanceToTarget = length(direction);
			float distance = mSpeed*dt;

			if (length(direction) > 2)
			{
				// Normalize direction and update direction
				direction = normalize(direction);
				mSplineParameterT = mSplineParameterT + dt*mSpeed;
				SetPosition(mSpline->GetPosition(mSplineParameterT) + 0.5f*mSplineParameterT*direction);
			}
			else
			{
				SetPosition(vec3(0, 0, 0));
			}
		}
		else
		{
			mSplineParameterT = mSplineParameterT + dt*mSpeed;
			SetPosition(mSpline->GetPosition(mSplineParameterT));
		}

		// Press Spacebar to activate black hole
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			blackHole = true;
		}
		// Press B to deactivate black hole
		if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_B) == GLFW_PRESS)
		{
			blackHole = false;
		}
	}
}

void Model::Draw()
{
}


void Model::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while(std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		istream_iterator<ci_string, char, ci_char_traits> end;
		vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stdout, "Error loading scene file... token:  %s!", line);
			getchar();
			exit(-1);
		}
	}
}

bool Model::ParseLine(const std::vector<ci_string> &token)
{
		if (token.empty() == false)
	{
		if (token[0].empty() == false && token[0][0] == '#')
		{
			return true;
		}
		else if (token[0] == "name")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			mName = token[2];	
		}
		else if (token[0] == "position")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mPosition.x = static_cast<float>(atof(token[2].c_str()));
			mPosition.y = static_cast<float>(atof(token[3].c_str()));
			mPosition.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "rotation")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mRotationAxis.x = static_cast<float>(atof(token[2].c_str()));
			mRotationAxis.y = static_cast<float>(atof(token[3].c_str()));
			mRotationAxis.z = static_cast<float>(atof(token[4].c_str()));
			mRotationAngleInDegrees = static_cast<float>(atof(token[5].c_str()));

			glm::normalize(mRotationAxis);
		}
		else if (token[0] == "scaling")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mScaling.x = static_cast<float>(atof(token[2].c_str()));
			mScaling.y = static_cast<float>(atof(token[3].c_str()));
			mScaling.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "lighting")
		{
			assert(token.size() > 5);
			assert(token[1] == "=");

			mMaterialCoefficients.x = static_cast<float>(atof(token[2].c_str()));
			mMaterialCoefficients.y = static_cast<float>(atof(token[3].c_str()));
			mMaterialCoefficients.z = static_cast<float>(atof(token[4].c_str()));
			mMaterialCoefficients.w = static_cast<float>(atof(token[5].c_str()));
		}
        else if (token[0] == "pathspeed")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

            float speed = static_cast<float>(atof(token[2].c_str()));
            SetSpeed(speed);
		}
        else if (token[0] == "boundpath")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			ci_string pathName = token[2];
            World* w = World::GetInstance();
            mPath = w->FindPath(pathName);

			if (mPath == nullptr)
			{
				mSpline = w->FindSpline(pathName);
			}
			if (mPath != nullptr)
			{
				mPosition = mPath->GetWaypoint(0);
			}
			else if (mSpline)
			{
				mPosition = mSpline->GetPosition(mSplineParameterT);
			}
		}
		else if (token[0] == "object") {
			assert(token.size() > 2);
			assert(token[1] == "=");
			mObjectFileName = token[2].c_str();
		}
		else if (token[0] == "texture") {
			assert(token.size() > 2);
			assert(token[1] == "=");
			mTextureFileName = token[2].c_str();
		}
		else if (token[0] == "texture_n") {
			assert(token.size() > 2);
			assert(token[1] == "=");
			mTextureNormalFileName = token[2].c_str();
		}
		else if (token[0] == "texture_d") {
			assert(token.size() > 2);
			assert(token[1] == "=");
			mTextureDiffuseFileName = token[2].c_str();
		}
		else if (token[0] == "texture_s") {
			assert(token.size() > 2);
			assert(token[1] == "=");
			mTextureSpecularFileName = token[2].c_str();
		}
		else
		{
			return false;
		}
	}

	return true;
}

glm::mat4 Model::GetWorldMatrix() const
{
	mat4 worldMatrix(1.0f);

	mat4 t = glm::translate(mat4(1.0f), mPosition);
	mat4 r = glm::rotate(mat4(1.0f), mRotationAngleInDegrees, mRotationAxis);
	mat4 s = glm::scale(mat4(1.0f), mScaling);
	worldMatrix = t * r * s;

	return worldMatrix;
}

void Model::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void Model::SetScaling(glm::vec3 scaling)
{
	mScaling = scaling;
}

void Model::SetRotation(glm::vec3 axis, float angleDegrees)
{
	mRotationAxis = normalize(axis);
	mRotationAngleInDegrees = angleDegrees;
}

void Model::SetSpeed(float spd)
{
    mSpeed = spd;
}