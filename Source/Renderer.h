

#pragma once

struct GLFWwindow;

// Include GLEW - OpenGL Extension Wrangler
#include <GL/glew.h>

#include <vector>
#include <GLM/glm.hpp>


enum ShaderType
{
	SHADER_SOLID_COLOR,
	SHADER_PATH_LINES,
	SHADER_BLUE,
	SHADER_PLANET,
	SHADER_NORMAL_MAP,
	SHADER_TEXTURE,
	NUM_SHADERS
};


class Renderer
{
public:
	static void Initialize();
	static void Shutdown();

	static void BeginFrame();
	static void EndFrame();

	static GLuint LoadShaders(std::string vertex_shader_path, std::string fragment_shader_path);
	static bool   LoadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals );

	static unsigned int GetShaderProgramID() { return sShaderProgramID[sCurrentShader]; }
	static unsigned int GetCurrentShader() { return sCurrentShader; }
	static void SetShader(ShaderType type);

private:
	static GLFWwindow* spWindow;

	static std::vector<unsigned int> sShaderProgramID;
	static unsigned int sCurrentShader;

};

