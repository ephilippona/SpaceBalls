

#pragma once

#include "ParsingHelper.h"
#include <vector>

class Camera;
class Model;
class Path;

class World
{
public:
	World();
	~World();
	
    static World* GetInstance();

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);
    Path* FindPath(ci_string pathName);

private:
    static World* instance;

	std::vector<Model*> mModel;
    std::vector<Path*> mPath;
	std::vector<Camera*> mCamera;
	unsigned int mCurrentCamera;
};
