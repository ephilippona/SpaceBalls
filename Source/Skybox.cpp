

#include "Skybox.h"
#include "Renderer.h"
#include <GL/glew.h>
#include "texture.hpp"
//#include <iostream>


Skybox::Skybox()
{ 
	mDrawStyle = Planet;
}

Skybox::~Skybox()
{
}

void Skybox::init() {
	std::string path;

	path = "../Objects_And_Textures/";
	std::string objectPath = path.append(mObjectFileName.c_str());
	
	std::string texturePath;
	// Load the textures
	for (int i = 0; i < 6; i++)
	{
		path = "../Objects_And_Textures/";
		texturePath = path.append(mTextureFileName[i].c_str());
		skybox[i] = loadBMP_custom(texturePath.c_str());
	}

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

void Skybox::Draw()
{
	// Draw the Vertex Buffer
	// Note this draws a unit Cube
	// The Model View Projection transforms are computed in the Vertex Shader
	glDisable(GL_LIGHTING); //turn off lighting, when making the skybox
	glDisable(GL_DEPTH_TEST);       //turn off depth texting
	glBindVertexArray(mVertexArrayID);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	//glBindTexture(GL_TEXTURE_2D, Texture);

	glUniform1i(TextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
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

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Draw the triangle !
	glDepthMask(0);
	for (int i=0; i < (vertices.size()/6); i++){
		glBindTexture(GL_TEXTURE_2D, skybox[i]);
		int start = i * 6;
		glDrawArrays(GL_TRIANGLES, i*6, 6);
	}
	glDepthMask(1);
	

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Set the texture for subsequent draws
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_LIGHTING);  
	glEnable(GL_DEPTH_TEST);
}

void Skybox::Update(float dt){
	
}

void Skybox::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while (std::getline(iss, line))
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
/*
* Obtain parameter from the scene file unique to the skybox
*/
bool Skybox::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty())
    {
        return true;
    }
	else if (token[0] == "textureTop") {
		assert(token.size() > 2);
		assert(token[1] == "=");
		mTextureFileName[TOP] = token[2].c_str();
	}
	else if (token[0] == "textureBottom") {
		assert(token.size() > 2);
		assert(token[1] == "=");
		mTextureFileName[BOTTOM] = token[2].c_str();
	}
	else if (token[0] == "textureFront") {
		assert(token.size() > 2);
		assert(token[1] == "=");
		mTextureFileName[FRONT] = token[2].c_str();
	}
	else if (token[0] == "textureBack") {
		assert(token.size() > 2);
		assert(token[1] == "=");
		mTextureFileName[BACK] = token[2].c_str();
	}
	else if (token[0] == "textureLeft") {
		assert(token.size() > 2);
		assert(token[1] == "=");
		mTextureFileName[LEFT] = token[2].c_str();
	}
	else if (token[0] == "textureRight") {
		assert(token.size() > 2);
		assert(token[1] == "=");
		mTextureFileName[RIGHT] = token[2].c_str();
	}
	else{
		return Model::ParseLine(token);
	}
	return true;
}