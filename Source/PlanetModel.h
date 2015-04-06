

#pragma once

#include "Model.h"
#include <GL/glew.h>

class PlanetModel : public Model
{
public:
	PlanetModel(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual ~PlanetModel(void);

	virtual void Update(float dt);
	virtual void Draw();
	virtual void init();

	GLuint Texture;
	GLuint TextureID;

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);

private:
	// The vertex format could be different for different types of models
	void computeLightSources(glm::vec3 lightOrigin, glm::vec3 planetOrigin, float sunRadius, glm::vec3* outputBuf);
	void convertSphericalToCartesian(float horizontalAngle, float verticalAngle, float radius, float* x, float* y, float* z);

	unsigned int mVertexArrayID;

	// Buffers for vertex data
	unsigned int vertexbuffer;
	unsigned int uvbuffer;
	unsigned int normalbuffer;
	
	// Vertex data
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

};