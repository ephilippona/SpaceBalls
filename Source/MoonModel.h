

#pragma once

#include "Model.h"
#include "SphereModel.h"

class MoonModel : public Model
{
public:
	MoonModel(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~MoonModel(void);

    virtual void Update(float dt);
    virtual void Draw();
	void SetParent(Model* m);
	ci_string GetParentName()  { return mParentName; }
    void SetParentDistance(float d);
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

    unsigned int mVertexArrayID;
    unsigned int mVertexBufferID;
    unsigned int numOfVertices;

	ci_string mParentName;
	Model* mParent;
	float mParentDistance;
};


