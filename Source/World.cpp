//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Modified By Eric Philippona (6225497) -- Added Collision detection and spaceship loading
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
#include "ShipModel.h"
#include "Skybox.h"


#include "StaticCamera.h"
#include "MovableCamera.h"
#include "BSplineCamera.h"
#include "ThirdPersonCamera.h"

#include <GLFW/glfw3.h>
#include "EventManager.h"
#include <string.h>
#include <GL/GL.h>

using namespace std;
using namespace glm;

World* World::instance;

World::World()
{
    instance = this;
	collisionApproaching = false;
	controlsOn = true;
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
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_5 ) == GLFW_PRESS)
    {
        if (mCamera.size() > 4)
        {
            mCurrentCamera = 4;
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

	

	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_A ) == GLFW_PRESS)
		mShip->SetRotation(glm::vec3(1,0,0),50.0f);
	if(glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D ) == GLFW_PRESS)
		mShip->SetRotation(glm::vec3(1,0,0),-50.0f);

	bool collisionIsClose = false;

	std::cout << controlsOn << std::endl;

	controlsOn = true;

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);  

		//Exclude skybox from collision detection
		if ((*it)->GetScaling().x != 1000.0f && (*it)->GetScaling().x != 1.0f){

			//Get Radius of each sphere (Since all models are spheres, the scaling divided by 2 is the radius)
			float radius = (*it)->GetScaling().x / 2.0f;
			
			//Check for collision
			if (glm::distance((*it)->GetPosition(), mShip->GetPosition()) < (length(mShipCamera->GetSpeedColision()) + radius)){
				
				//Get vector from plannet to the Ship
				vec3 temp = mShip->GetPosition() - (*it)->GetPosition();

				//Turn off user controls until collision is handled
				controlsOn = false; 

				//collision handling (go over/ under plannet)
				if (mShip->GetPosition().y < 0)	{
					mShip->SetPosition(mShip->GetPosition() - glm::vec3(0.0f, radius / 200.0f, 0.0f));
				}
				else{
					mShip->SetPosition(mShip->GetPosition() + glm::vec3(0.0f, radius / 200.0f, 0.0f));
				}
			}
			if (glm::distance((*it)->GetPosition(), mShip->GetPosition()) < (1.3f * radius)){
				collisionIsClose = true;
			}

		}
	}
	collisionApproaching = collisionIsClose;

	//// Update current Camera

		//First check if controls should be on
		if (!controlsOn)
			mCamera[mCurrentCamera]->Update(0.0f);
		//Then check if collision is approaching (in which case the speed is divided by 3)
		else if (collisionApproaching)
			mCamera[mCurrentCamera]->Update(dt / 3);
		else
			mCamera[mCurrentCamera]->Update(dt);




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
			if((*it)->GetName()=="Skybox"){
				Renderer::SetShader(SHADER_TEXTURE);
			}
			else{
				Renderer::SetShader(SHADER_PLANET);
			}
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
			else if (result == "skybox")
			{
				Skybox* skybox = new Skybox();
				skybox->Load(iss);
				skybox->init();
				mModel.push_back(skybox);
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
			else if(result == "ship"){
			
				unsigned int prevShader = Renderer::GetCurrentShader();
				Renderer::SetShader(SHADER_PLANET);
				glUseProgram(Renderer::GetShaderProgramID());

				ShipModel* ship = new ShipModel();
				ship->Load(iss);
				ship->init();
				mModel.push_back(ship);
				mShip = ship;


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
	// Setup Camera----Christopher Maroday
	mCamera.push_back(new StaticCamera(vec3(5.0f, 950.0f, 20.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
    
	//MovableCamera-----Christopher Maroday
	SphereModel* movableCamera = new SphereModel();
    movableCamera->SetPosition(vec3(0.0f, 0.5f, 0.0f));
    mCamera.push_back(new MovableCamera(movableCamera));
    mModel.push_back(movableCamera);

	// BSpline Camera-----Christopher Maroday
    BSpline* spline = FindSpline("\"Path6\"");
    if (spline == nullptr)
    {
        spline = FindSplineByIndex(5);
    }
    
    if (spline != nullptr)
    {
        mCamera.push_back(new BSplineCamera(spline , 2.0f));
    }

    // Ship Character controlled with Third Person Camera

	mShipCamera = new ThirdPersonCamera(mShip);
    mCamera.push_back(mShipCamera);
	


    
    
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