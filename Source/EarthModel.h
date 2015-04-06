

#pragma once

#include "Model.h"
#include <GL/glew.h>

class EarthModel : public Model
{
public:
	EarthModel(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual ~EarthModel(void);

	virtual void Update(float dt);
	virtual void Draw();
	virtual void init();

	// The one with ID is the handle to the shader. The other one is the texture object itself.
	GLuint TextureNormal;
	GLuint TextureNormalID;
	GLuint TextureDiffuse;
	GLuint TextureDiffuseID;
	GLuint TextureSpecular;
	GLuint TextureSpecularID;

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);

private:

	void computeTangentBasis(
		// inputs
		std::vector<glm::vec3> & vertices,
		std::vector<glm::vec2> & uvs,
		std::vector<glm::vec3> & normals,
		// outputs
		std::vector<glm::vec3> & tangents,
		std::vector<glm::vec3> & bitangents);

	// Buffers
	unsigned int mVertexArrayID;
	unsigned int vertexbuffer;
	unsigned int uvbuffer;
	unsigned int normalbuffer;
	unsigned int tangentbuffer;
	unsigned int bitangentbuffer;
	
	// Vertex information
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

};