

#include "PlanetModel.h"
#include "Renderer.h"
#include "texture.hpp"
#include <GL/glew.h>

const float PI = 3.141592653;

using namespace glm;

PlanetModel::PlanetModel(vec3 size) : Model()
{
	mDrawStyle = Planet;
}

PlanetModel::~PlanetModel()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteVertexArrays(1, &mVertexArrayID);
	
}

void PlanetModel::init() {

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

void PlanetModel::Update(float dt)
{
		//rotate
		//mRotationAngleInDegrees += 1.0f;
		mRotationAngleInDegrees += spin_speed;
	    if(mRotationAngleInDegrees>360.f)
		{
		//printf("angle reset\n");
		mRotationAngleInDegrees-=360;
		}
	Model::Update(dt);
}

void PlanetModel::Draw()
{
	// Draw the Vertex Buffer
	// Note this draws a unit Cube
	// The Model View Projection transforms are computed in the Vertex Shader

	glBindVertexArray(mVertexArrayID);

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
	

	/*vec3 lightSamplePositions[25];
	computeLightSources(vec3(0.0f, 0.0f, 0.0f), vec3(10.0f, 0.0f, 0.0f), 2.5f, lightSamplePositions);

	// Flatten the array for the shader
	float lightSamplePositionsFloat [75];
	for (int i = 0; i < 5; i++) {
		lightSamplePositionsFloat[3*i] = lightSamplePositions[i].x;
		lightSamplePositionsFloat[3*i+1] = lightSamplePositions[i].y;
		lightSamplePositionsFloat[3*i+2] = lightSamplePositions[i].z;
	}*/

	GLuint LightID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldLightPosition");
	glUniform3f(LightID, 0.0f, 2.5f, 0.0f);
	//glUniform3fv(Renderer::GetShaderProgramID(), 15, lightSamplePositionsFloat);

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
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Set the texture for subsequent draws
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

bool PlanetModel::ParseLine(const std::vector<ci_string> &token)
{
	if (token.empty())
	{
		return true;
	}
	else
	{
		return Model::ParseLine(token);
	}
}

void PlanetModel::computeLightSources(vec3 lightOrigin, vec3 planetOrigin, float sunRadius, vec3* outputBuf) {

	int numberOfLightsPerHorizontalAngle = 4;

	// First compute vector from light to planet
	vec3 normal = planetOrigin - lightOrigin;
	vec3 unitNormal = normalize(normal);
	vec3 scaledNormal = unitNormal * sunRadius;

	// Use scaled normal to compute the first point
	outputBuf[0] = scaledNormal + lightOrigin;

	// Now compute two other vectors to form a tangent basis
	vec3 tangent = cross(unitNormal, vec3(0,1,0));
	tangent = normalize(tangent);
	vec3 bitangent = cross(unitNormal, tangent);

	// Make sure we have a basis. Dot product should be zero
	float tmp1 = dot(tangent, unitNormal);
	float tmp2 = dot(bitangent, unitNormal);
	float tmp3 = dot(tangent, bitangent);
	assert(tmp1 < 0.001 && tmp1 > -0.001);
	assert(tmp2 < 0.001 && tmp2 > -0.001);
	assert(tmp3 < 0.001 && tmp3 > -0.001);

	int counter = 1;

	// X, Y, and Z are in tangent space
	float x_tangentspace; // X maps to tangent
	float y_tangentspace; // Y maps to unitNormal
	float z_tangentspace; // Z maps to bitangent
	
	for (int j = 0; j < 1; j++) {
		for (int i = 0; i < numberOfLightsPerHorizontalAngle; i++) {
		
			// j represents the 4 vertical angles. Let's arbitrarily choose 15, 35, 55 and 75
			// i represents the 6 horizontal angles. Space them evernly around the sphere.
			convertSphericalToCartesian(i*360/numberOfLightsPerHorizontalAngle, j*20+15, sunRadius, &x_tangentspace, &y_tangentspace, &z_tangentspace);
			
			/* Based on the theory from the view transform from class 2 slides, slide 18, the following will
			 * transform the point in tangent space into worldspace
			 */
			
			mat4 tangentToWorldtransformMatrix = mat4(vec4(tangent, 0),
													vec4(unitNormal, 0),
													vec4(bitangent, 0),
													vec4(lightOrigin, 1));

			// This step performs the [World basis matrix] * P = [Tangent basis matrix] * P'
			// Note that world basis matrix is identity therefore P = [Tangent basis matrix] * P'
			vec4 lightLocationWorldspace = tangentToWorldtransformMatrix * vec4(x_tangentspace, y_tangentspace, z_tangentspace, 1);

			outputBuf[counter] = vec3(lightLocationWorldspace.x, lightLocationWorldspace.y, lightLocationWorldspace.z);

			std::cout << "Worldspace coordinates: X: " << lightLocationWorldspace.x << ", Y: " << lightLocationWorldspace.y << " Z: " << lightLocationWorldspace.z <<std::endl;
			counter++;

		}
	}

}

void PlanetModel::convertSphericalToCartesian(float horizontalAngle, float verticalAngle, float radius, float* x, float* y, float* z) {

	*x = radius * cos(verticalAngle*PI/180) * cos(horizontalAngle*PI/180);
	*y = radius * sin(verticalAngle*PI/180);
	*z = -1 * radius * cos(verticalAngle*PI/180) * sin(horizontalAngle*PI/180);

}