﻿//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#pragma once

#include "ParsingHelper.h"
#include <vector>

class Camera;
class Model;
class MoonModel;
class Path;
class ShipModel;
class BSpline;
class Skybox;
class ThirdPersonCamera;

class World
{
public:
	World();
	~World();
	
    static World* GetInstance();

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);
    void LoadCameras();
	void SetMoon(MoonModel m, ci_string parentModelName);

    Path* FindPath(ci_string pathName);
    BSpline* FindSpline(ci_string pathName);
    BSpline* FindSplineByIndex(unsigned int index);
    Model* FindModelByIndex(unsigned int index);
	Model* FindModelByName(ci_string name);
	ShipModel* mShip;
	ThirdPersonCamera* mShipCamera;

private:
    static World* instance;
	bool collisionApproaching;
	bool controlsOn;
	std::vector<Model*> mModel;
    std::vector<Path*> mPath;
    std::vector<BSpline*> mSpline;
	std::vector<Camera*> mCamera;
	unsigned int mCurrentCamera;
};
