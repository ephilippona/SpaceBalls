//Written By Daniel Montalvo - initial implementation including rotation around parent
//Modified By Philip Eloy - Self rotation, texturing, draw

#pragma once

#include "Model.h"
//#include "SphereModel.h"
#include <GL/glew.h>

class MoonModel : public Model
{
public:
	MoonModel(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~MoonModel(void);

	virtual void init();

    virtual void Update(float dt);
    virtual void Draw();
	void SetParent(Model* m);
	ci_string GetParentName()  { return mParentName; }
    void SetParentDistance(float d);

	GLuint Texture;
	GLuint TextureID;

protected:
    virtual bool ParseLine(const std::vector<ci_string> &token);

private:
    // The vertex format could be different for different types of models
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

	// The vertex data
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

    unsigned int mVertexArrayID;

	// Buffers for holding the vertex data
	unsigned int vertexbuffer;
	unsigned int uvbuffer;
	unsigned int normalbuffer;

	ci_string mParentName;
	Model* mParent;
	float mParentDistance;
	float initialDisperse;
	float increment;
};


