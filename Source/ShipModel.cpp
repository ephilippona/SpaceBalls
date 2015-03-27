

#include "ShipModel.h"
#include "Renderer.h"
#include <GL/glew.h>

using namespace glm;

ShipModel::ShipModel(vec3 size) : Model()
{
    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; // Won't be used at the moment.
    bool res = Renderer::LoadOBJ("Spaceship.obj", vertices, uvs, normals);
    
    // Load it into a VBO
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
  
}

ShipModel::~ShipModel()
{
    glDeleteBuffers(1, &mVertexBufferID);
    glDeleteVertexArrays(1, &mVertexArrayID);
}


void ShipModel::Update(float dt)
{
    Model::Update(dt);
}

void ShipModel::Draw()
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
                          sizeof(Vertex), // stride
                          (void*)0        // array buffer offset
                          );
    
    // 2nd attribute buffer : vertex normal
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glVertexAttribPointer(  1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
                          );
    
    
    // 3rd attribute buffer : vertex color
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
    glVertexAttribPointer(  2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*) (2* sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
                          );
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numOfVertices);
    
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

bool ShipModel::ParseLine(const std::vector<ci_string> &token)
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