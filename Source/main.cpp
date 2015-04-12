// COMP 371 Assignment Framework

#include "Renderer.h"
#include "World.h"
#include "EventManager.h"
#include "texture.hpp"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
GLFWwindow* window;

using namespace glm;

int main(int argc, char*argv[])
{
	EventManager::Initialize();
	Renderer::Initialize();

	World world;

	if (argc > 1)
	{
		world.LoadScene(argv[1]);
	}
	else
	{
#if defined(PLATFORM_OSX)
        //world.LoadScene("Scenes/CoordinateSystem.scene");
        world.LoadScene("Scenes/PathScene.scene");
#else
        //world.LoadScene("../Scenes/CoordinateSystem.scene");
        world.LoadScene("../Scenes/PathScene.scene");
#endif
	}

	// Main Loop
	do
	{
		// Update Event Manager - Frame time / input / events processing 
		EventManager::Update();

		// Update World
		float dt = EventManager::GetFrameTime();
		world.Update(dt);

		// Draw World
		world.Draw();
	}
	while(EventManager::ExitRequested() == false);

	Renderer::Shutdown();
	EventManager::Shutdown();

	return 0;
}
