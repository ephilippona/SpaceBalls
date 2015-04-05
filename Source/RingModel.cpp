

#include "RingModel.h"
#include "Renderer.h"
#include <GL/glew.h>

using namespace glm;

const float PI = 3.14;
const int NUMBER_OF_VERTICES = 606;

RingModel::RingModel(vec3 size) : Model()
{
	mDiffferentShader = false;

	vec3 vertexBuffer[NUMBER_OF_VERTICES];
	vec3 normalBuffer[NUMBER_OF_VERTICES];
	vec3 colourBuffer[NUMBER_OF_VERTICES];

	float z;
	float x;

	for (int i = 0; i < NUMBER_OF_VERTICES; i++) {

		if (i < 202) {
			
			if (i % 2 == 0) {
				convertPolarToCartesian(1.0f, 3.6*i/2, &x, &z);
			}
			else {
				convertPolarToCartesian(0.8f, 3.6*i/2, &x, &z);
			}
			vertexBuffer[i] = vec3(x,0.0f,z);
			normalBuffer[i] = vec3(0.0f,1.0f,0.0f);
			colourBuffer[i] = vec3(0.76f, 0.54f, 0.367f);

		}
		else if (i < 404) {
			
			if (i % 2 == 0) {
				convertPolarToCartesian(0.8f, 3.6*(i-202)/2, &x, &z);
			}
			else {
				convertPolarToCartesian(0.75f, 3.6*(i-202)/2, &x, &z);
			}
			vertexBuffer[i] = vec3(x,0.0f,z);
			normalBuffer[i] = vec3(0.0f,1.0f,0.0f);
			colourBuffer[i] = vec3(0.0f, 0.0f, 0.0f);

		}
		else if (i < 606) {
			
			if (i % 2 == 0) {
				convertPolarToCartesian(0.75f, 3.6*(i-404)/2, &x, &z);
			}
			else {
				convertPolarToCartesian(0.6f, 3.6*(i-404)/2, &x, &z);
			}
			vertexBuffer[i] = vec3(x,0.0f,z);
			normalBuffer[i] = vec3(0.0f,1.0f,0.0f);
			colourBuffer[i] = vec3(0.76f, 0.54f, 0.367f);
		}
		else {
			std::cout << "Something went horribly wrong!" <<std::endl;
		}

	}

    glGenVertexArrays(1, &mVertexArrayID);
    
	glGenBuffers(1, &mVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), &vertexBuffer[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mNormalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBufferID);
	glBufferData(GL_ARRAY_BUFFER, NUMBER_OF_VERTICES * sizeof(glm::vec3), normalBuffer, GL_STATIC_DRAW);

	glGenBuffers(1, &mColourBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mColourBufferID);
	glBufferData(GL_ARRAY_BUFFER, NUMBER_OF_VERTICES * sizeof(glm::vec3), &colourBuffer[0], GL_STATIC_DRAW);

	// TODO - add parent
	mPosition = vec3(6,3,6);
	mScaling = vec3(4,4,4);
	mRotationAxis = vec3(0,1,0);
	mRotationAngleInDegrees = 0;
	mName = "Rings";

}

RingModel::~RingModel()
{
    glDeleteBuffers(1, &mVertexBufferID);
	glDeleteBuffers(1, &mNormalBufferID);
	glDeleteBuffers(1, &mColourBufferID);
    glDeleteVertexArrays(1, &mVertexArrayID);
}


void RingModel::Update(float dt)
{
    Model::Update(dt);
}

void RingModel::Draw()
{
    // Draw the Vertex Buffer
    // Note this draws a unit Cube
    // The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVertexArrayID);

    GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform"); 
    glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);
    
    // 1st attribute buffer : vertex Positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glVertexAttribPointer(  0,              // attribute. No particular reason for 0, but must match the layout in the shader.
                            3,              // size
                            GL_FLOAT,       // type
                            GL_FALSE,       // normalized?
                            0, // stride
                            (void*)0        // array buffer offset
                        );

    // 2nd attribute buffer : vertex normal
    glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBufferID);
    glVertexAttribPointer(  1,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            (void*)0    // Normal is Offseted by vec3 (see class Vertex)
                        );


    // 3rd attribute buffer : vertex color
    glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, mColourBufferID);
    glVertexAttribPointer(  2,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            (void*)0 // Color is Offseted by 2 vec3 (see class Vertex)
                        );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLE_STRIP, 0, NUMBER_OF_VERTICES);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

bool RingModel::ParseLine(const std::vector<ci_string> &token)
{
    return true;
}

void RingModel::convertPolarToCartesian(float radius, float angle, float* x, float* z) {

	*x = cos(angle*PI/180) * radius;
	*z = sin(angle*PI/180) * radius;

}