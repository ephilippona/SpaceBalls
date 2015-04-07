//
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

#include <GLM/glm.hpp>

class Path;
class BSpline;


enum DrawType {
	Standard,
	Planet,
	Normal_Map
};

class Model
{
public:
	Model();
	virtual ~Model();

	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

	void Load(ci_istringstream& iss);

	virtual glm::mat4 GetWorldMatrix() const;

	void SetPosition(glm::vec3 position);
	void SetScaling(glm::vec3 scaling);
	void SetRotation(glm::vec3 axis, float angleDegrees);

	glm::vec3 GetPosition() const		{ return mPosition; }
	glm::vec3 GetScaling() const		{ return mScaling; }
	glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
	float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
	ci_string GetName()                 { return mName; }
	unsigned int GetNumChildren()       { return mNumChildren; }
	DrawType GetDrawType()              { return mDrawStyle;}
	glm::vec4 GetMaterialCoefficients() { return mMaterialCoefficients;}

	void SetSpeed(float spd);
	void IncrementNumChild()       { mNumChildren++; }

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token) = 0;

	ci_string mName; // The model name is mainly for debugging
	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::vec3 mRotationAxis;
	float     mRotationAngleInDegrees;
	float	  spin_speed;

	// Makes the model follow a path defined by a set of waypoints
	Path* mPath;
	BSpline* mSpline;
	float mSplineParameterT;
	float mSpeed;
	bool blackHole;
	float holeFactor;
	float holeSpeed;
	glm::vec3 direction;
	unsigned int mTargetWaypoint;
	unsigned int mNumChildren;



	// For models with an object file
	std::string mObjectFileName;

	// For models with textures
	std::string mTextureFileName;
	std::string mTextureBlackHoleFileName;
	std::string mTextureNormalFileName;
	std::string mTextureDiffuseFileName;
	std::string mTextureSpecularFileName;

	// If the model needs a special shader to render
	DrawType mDrawStyle;

	// Lighting properties
	glm::vec4 mMaterialCoefficients;

};
