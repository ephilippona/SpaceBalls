

#include "MoonModel.h"
#include "Renderer.h"
#include <GL/glew.h>
#include "texture.hpp"
//#include <iostream>
using namespace glm;
//using namespace std;

MoonModel::MoonModel(vec3 size) : Model()
{ 

	mDrawStyle = Planet;
	increment = 0.0f;

}

MoonModel::~MoonModel()
{

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer); //Texture buffer
    glDeleteVertexArrays(1, &mVertexArrayID);
}

void MoonModel::init() {

	std::string path;

	path = "../Objects_And_Textures/";
	std::string texturePath = path.append(mTextureFileName.c_str());

	path = "../Objects_And_Textures/";
	std::string objectPath = path.append(mObjectFileName.c_str());

	// Load the texture
	Texture = loadBMP_custom(texturePath.c_str());

	// Get a handle for our "myTextureSampler" uniform
	TextureID = glGetUniformLocation(Renderer::GetShaderProgramID(), "myTextureSampler");

	// Read our .obj file
	bool res = Renderer::LoadOBJ(objectPath.c_str(), vertices, uvs, normals);

	glGenVertexArrays(1, &mVertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

}

void MoonModel::Update(float dt)
{
		//rotate
		mRotationAngleInDegrees += spin_speed;
	    if(mRotationAngleInDegrees>360.f)
		{
		//printf("angle reset\n");
			//mRotationAngleInDegrees-=350;
			mRotationAngleInDegrees=1;
		}
		if(mRotationAngleInDegrees<0.f)
		{
			mRotationAngleInDegrees=359;
		}

	// idea to rotate: glm::vec3( radius from parent * glm::cos(speed*incrementedVariable), 1, radius from parent *glm::sin(speed*incrementedVariable))
	//We limit overflow and keep the increment between -pi,pi
	if(increment>3.1416){
		increment=mod(increment, 3.1416f);
		increment-=3.1416;
	}
	increment+=mSpeed;
	mPosition = mParent->GetPosition()-(glm::vec3(mParentDistance*(glm::cos(increment+initialDisperse)),0,mParentDistance*(glm::sin(increment+initialDisperse))));
    //Model::Update(dt);
}

/*
*	Sets the parent reference and initializes the position
*/
void MoonModel::SetParent(Model* m)
{
    mParent = m;
	mParent->IncrementNumChild();
	// TODO revise - Symmetrical type of dispersing the init position of the moons. Support up to 4-5 moons. 
	switch (mParent->GetNumChildren())
	{
		case 1:{
			initialDisperse = -3.1416;
			mPosition = mParent->GetPosition()+glm::vec3(mParentDistance,0.0f,0.0f);
		}
		break;
		case 2:{
			initialDisperse = -1.047197551;
			mPosition = mParent->GetPosition()+glm::vec3(-mParentDistance,0.0f,0.0f);
		}
		break;
		case 3:{
			initialDisperse = 1.047197551;
			mPosition = mParent->GetPosition()+glm::vec3(0.0f,0.0f,mParentDistance);
		}
		break;
		case 4:{
			initialDisperse = 3.1416;
			mPosition = mParent->GetPosition()+glm::vec3(0.0f,0.0f,-mParentDistance);
		}
		break;
		default:{
			initialDisperse = 0;
			mPosition = mParent->GetPosition()+glm::vec3(mParentDistance,0.0f,mParentDistance);
		}
	}
}

void MoonModel::Draw()
{
    // Draw the Vertex Buffer
    // Note this draws a unit Cube
    // The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVertexArrayID);

    GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");

	// TODO -- Use the parent * local for hierarchical transforms
	// mat4 worldMatrix = mParent->GetWorldMatrix()*GetWorldMatrix();
	
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
    
    // 1st attribute buffer : vertex Positions
    glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(  0,              // attribute. No particular reason for 0, but must match the layout in the shader.
                            3,              // size
                            GL_FLOAT,       // type
                            GL_FALSE,       // normalized?
                            0,//sizeof(Vertex), // stride
                            (void*)0        // array buffer offset
                        );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : vertex normal
    glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(  2,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            (void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
                        );

    // Draw the triangles !
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

	// Set the texture for subsequent draws
	glBindTexture(GL_TEXTURE_2D, 0);
}
/*
* Obtain parameter from the scene file unique to a moon
*/
bool MoonModel::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty())
    {
        return true;
    }
	else if (token[0] == "parent")
	{
		assert(token.size() > 2);
		assert(token[1] == "=");
		mParentName = token[2];
	}
	else if (token[0] == "parentDistance")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

            float radius = static_cast<float>(atof(token[2].c_str()));
            SetParentDistance(radius);
		}
    else
    {
        return Model::ParseLine(token);
    }
	return true;
}
/*
* Sets the distance from the parent planet and this moon
*/
void MoonModel::SetParentDistance(float r)
{
    mParentDistance = r;
}