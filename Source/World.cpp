//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "World.h"
#include "Renderer.h"
#include "ParsingHelper.h"

#include "SphereModel.h"
#include "MoonModel.h"
#include "Path.h"
#include "BSpline.h"
#include "PlanetModel.h"
#include "EarthModel.h"
#include "RingModel.h"

#include "StaticCamera.h"
#include "MovableCamera.h"
#include "BSplineCamera.h"

#include <GLFW/glfw3.h>
#include "EventManager.h"
#include <string.h>

using namespace std;
using namespace glm;

World* World::instance;

World::World()
{
    instance = this;
}

World::~World()
{
	// Models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		delete *it;
	}

	mModel.clear();

    // Paths
    for (vector<Path*>::iterator it = mPath.begin(); it < mPath.end(); ++it)
	{
		delete *it;
	}
	mPath.clear();

    // Splines
    for (vector<BSpline*>::iterator it = mSpline.begin(); it < mSpline.end(); ++it)
	{
		delete *it;
	}
	mSpline.clear();

	// Camera
	for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	{
		delete *it;
	}
	mCamera.clear();
}

World* World::GetInstance()
{
    return instance;
}

void World::Update(float dt)
{
	// User Inputs
	// 0 1 2 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 1)
		{
			mCurrentCamera = 1;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 2)
		{
			mCurrentCamera = 2;
		}
	}
    else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_4 ) == GLFW_PRESS)
    {
        if (mCamera.size() > 3)
        {
            mCurrentCamera = 3;
        }
    }

	// Spacebar to change the shader
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_SOLID_COLOR);
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9 ) == GLFW_PRESS)
	{
		Renderer::SetShader(SHADER_BLUE);
	}

	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}

}

void World::Draw()
{
	Renderer::BeginFrame();
	
	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());

	glBindTexture(GL_TEXTURE_2D, 0);

	// Compute matricies which the shaders need
	mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	mat4 V = mCamera[mCurrentCamera]->GetViewMatrix();
	mat4 P = mCamera[mCurrentCamera]->GetProjectionMatrix();
	mat4 MVP;
	mat4 M;
	
	// Handles to the shader matricies
	GLuint MVPMatrixLocation;
	GLuint VPMatrixLocation;
	GLuint MV3x3Location;

	GLuint PMatrixLocation;
	GLuint VMatrixLocation;
	GLuint MMatrixLocation;
	GLuint NormalMatrixLocation;

	// Draw models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{

		//if (strcmp((*it)->GetName().c_str(), earth) == 0) {
		if ((*it)->GetDrawType() == Planet) {

			unsigned int prevShader = Renderer::GetCurrentShader();
			Renderer::SetShader(SHADER_PLANET);
			glUseProgram(Renderer::GetShaderProgramID());

			mat4 worldMatrix = (*it)->GetWorldMatrix();
			mat4 normalTransform = inverse(transpose(V * worldMatrix));
			vec4 materialCoefficients = (*it)->GetMaterialCoefficients();

		
			// Send the metricies to the shader
			//VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform"); 
			//glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

			MMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
			VMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
			PMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
			NormalMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "NormalTransform");
			GLuint materialCoefficientsLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");

			glUniformMatrix4fv(MMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
			glUniformMatrix4fv(VMatrixLocation, 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(PMatrixLocation, 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(NormalMatrixLocation, 1, GL_FALSE, &normalTransform[0][0]);
			glUniform4f(materialCoefficientsLocation, materialCoefficients.x, materialCoefficients.y, materialCoefficients.z, materialCoefficients.w);

			(*it)->Draw();

			// Restore previous shader
			Renderer::SetShader((ShaderType) prevShader);
			glUseProgram(Renderer::GetShaderProgramID());
	
		}
		else if ((*it)->GetDrawType() == Normal_Map) {
		
			unsigned int prevShader = Renderer::GetCurrentShader();
			Renderer::SetShader(SHADER_NORMAL_MAP);
			glUseProgram(Renderer::GetShaderProgramID());

			// Compute MVP matrix since the shader wants it
			M = (*it)->GetWorldMatrix();
			MVP = VP * M;

			// Comput MV3x3 matrix that the shander wants
			mat3 ModelView3x3Matrix = glm::mat3(V*M);

			// Send the MVP matrix, View matrix, Model Matrix and 3x3 MV matrix
			MVPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "MVP"); 
			VMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "V"); 
			MMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "M"); 
			MV3x3Location = glGetUniformLocation(Renderer::GetShaderProgramID(), "MV3x3"); 
						
			glUniformMatrix4fv(VMatrixLocation, 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(MMatrixLocation, 1, GL_FALSE, &M[0][0]);
			glUniformMatrix3fv(MV3x3Location, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

			// Send the world light position. Sun is at the center of the world
			GLuint LightID = glGetUniformLocation(Renderer::GetShaderProgramID(), "LightPosition_worldspace");
			glUniform3f(LightID, 0.0f, 2.5f, 0.0f);

			(*it)->Draw();

			// Restore previous shader
			Renderer::SetShader((ShaderType) prevShader);
			glUseProgram(Renderer::GetShaderProgramID());

		}
		else {

			// Since you changed the shader, resend the view projection matrix
			VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform"); 
			glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

			// Draw model
			//std::cout<<"drawing something else" << std::endl;
			(*it)->Draw();
		}
	}

	// Draw Path Lines
	
	// Set Shader for path lines
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());

	// Send the view projection constants to the shader
	
	VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	for (vector<Path*>::iterator it = mPath.begin(); it < mPath.end(); ++it)
	{
		// Draw model
		(*it)->Draw();
	}

    // Draw B-Spline Lines (using the same shader for Path Lines)
    for (vector<BSpline*>::iterator it = mSpline.begin(); it < mSpline.end(); ++it)
	{
		// Draw model
		(*it)->Draw();
	}

	// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);

	Renderer::EndFrame();

}

void World::LoadScene(const char * scene_path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if(input.fail() )
	{	 
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while( std::getline( input, item, '[' ) )   
	{
        ci_istringstream iss( item );

		ci_string result;
		if( std::getline( iss, result, ']') )
		{

            if( result == "sphere" )
            {
                SphereModel* sphere = new SphereModel();
                sphere->Load(iss);
                mModel.push_back(sphere);
            }
            else if( result == "path" )
			{
				Path* path = new Path();
				path->Load(iss);
                mPath.push_back(path);
			}
            else if( result == "spline" )
			{
				BSpline* path = new BSpline();
				path->Load(iss);
                mSpline.push_back(path);
			}
			else if( result == "moon" )
			{
				MoonModel* moon = new MoonModel();
				moon->Load(iss);
				moon->SetParent(FindModelByName(moon->GetParentName()));
				moon->init();
				mModel.push_back(moon);
			}
			else if( result == "ring" )
			{
				RingModel* ring = new RingModel();
				ring->Load(iss);
				ring->SetParent(FindModelByName(ring->GetParentName()));
				mModel.push_back(ring);
			}
			else if (result == "planet") {
				
				unsigned int prevShader = Renderer::GetCurrentShader();
				Renderer::SetShader(SHADER_PLANET);
				glUseProgram(Renderer::GetShaderProgramID());

				PlanetModel* planet = new PlanetModel();
				planet->Load(iss);
				planet->init();
				mModel.push_back(planet);

				// Restore previous shader
				Renderer::SetShader((ShaderType) prevShader);
				glUseProgram(Renderer::GetShaderProgramID());

			}
			else if (result == "earth") {

				unsigned int prevShader = Renderer::GetCurrentShader();
				Renderer::SetShader(SHADER_NORMAL_MAP);
				glUseProgram(Renderer::GetShaderProgramID());

				EarthModel* earth = new EarthModel();
				earth->Load(iss);
				earth->init();
				mModel.push_back(earth);

				// Restore previous shader
				Renderer::SetShader((ShaderType) prevShader);
				glUseProgram(Renderer::GetShaderProgramID());

			}
			else if ( result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
	    }
	}
	input.close();

	// Set PATH vertex buffers
	for (vector<Path*>::iterator it = mPath.begin(); it < mPath.end(); ++it)
	{
		// Draw model
		(*it)->CreateVertexBuffer();
	}

    // Set B-SPLINE vertex buffers
    for (vector<BSpline*>::iterator it = mSpline.begin(); it < mSpline.end(); ++it)
	{
		// Draw model
		(*it)->CreateVertexBuffer();
	}
    
    LoadCameras();
}
void World::LoadCameras()
{      
	// Setup Camera
	mCamera.push_back(new StaticCamera(vec3(5.0f, 90.0f, 20.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
    
	//MovableCamera
	SphereModel* character = new SphereModel();
    character->SetPosition(vec3(0.0f, 0.5f, 0.0f));
    mCamera.push_back(new MovableCamera(character));
    mModel.push_back(character);


     // BSpline Camera
    BSpline* spline = FindSpline("\"Path3\"");
    if (spline == nullptr)
    {
        spline = FindSplineByIndex(6);
    }
    
    if (spline != nullptr)
    {
        mCamera.push_back(new BSplineCamera(spline , 2.0f));
    }
    
    mCurrentCamera = 0;
}

Path* World::FindPath(ci_string pathName)
{
    for(std::vector<Path*>::iterator it = mPath.begin(); it < mPath.end(); ++it)
    {
        if((*it)->GetName() == pathName)
        {
            return *it;
        }
    }
    return nullptr;
}

BSpline* World::FindSpline(ci_string pathName)
{
    for(std::vector<BSpline*>::iterator it = mSpline.begin(); it < mSpline.end(); ++it)
    {
        if((*it)->GetName() == pathName)
        {
            return *it;
        }
    }
    return nullptr;
}

BSpline* World::FindSplineByIndex(unsigned int index)
{
    return mSpline.size() > 0 ? mSpline[index % mSpline.size()] : nullptr;
}

Model* World::FindModelByIndex(unsigned int index)
{
    return mModel.size() > 0 ? mModel[index % mModel.size()] : nullptr;
}

Model* World::FindModelByName(ci_string name)
{
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		if(!strcmp((*it)->GetName().c_str(),name.c_str()))
			return (*it);
	}
	return nullptr;
}