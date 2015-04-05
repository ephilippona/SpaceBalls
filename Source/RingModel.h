

#pragma once

#include "Model.h"

class RingModel : public Model
{
public:
	RingModel(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
    virtual ~RingModel(void);

    virtual void Update(float dt);
    virtual void Draw();
    
protected:
    virtual bool ParseLine(const std::vector<ci_string> &token);

private:

	virtual void convertPolarToCartesian(float radius, float angle, float* x, float* z);

    // The vertex format could be different for different types of models
    unsigned int mVertexArrayID;

	unsigned int mVertexBufferID;
	unsigned int mNormalBufferID;
	unsigned int mColourBufferID;

};


