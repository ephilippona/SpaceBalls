

#pragma once

//#include "SphereModel.h"
#include "ParsingHelper.h"
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <vector>
#include "Model.h"
using namespace std;

enum BoxSides{
	TOP=0,
	BOTTOM,
	FRONT,
	BACK,
	LEFT,
	RIGHT
};

class Skybox : public Model
{
public:
	Skybox();
    ~Skybox(void);

	void init();
	void Draw();
	GLuint TextureID;
	void Load(ci_istringstream& iss);
	virtual void Update(float dt);
protected:
    bool ParseLine(const std::vector<ci_string> &token);

private:
    // The vertex format could be different for different types of models
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };
	// Buffers for holding the vertex data
	unsigned int vertexbuffer;
	unsigned int uvbuffer;
	unsigned int normalbuffer;

	unsigned int mVertexArrayID;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint skybox[6];
	string mTextureFileName[6];
};


