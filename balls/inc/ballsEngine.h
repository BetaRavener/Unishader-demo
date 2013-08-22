/*
* UniShader-Demo - Showcase programs for UniShader Interface
* Copyright (c) 2011-2013 Ivan Sevcik - ivan-sevcik@hotmail.com
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#pragma once
#ifndef BALLS_ENGINE
#define BALLS_ENGINE

#include "includes.h"
#include <vector>
#include <string>
#include <UniShader\UniShader.h>
#include "hybridCam.h"
#include "math3d.h"

class HybridCam;

class BallsEngine{
public:
	BallsEngine();
	void loadShaders(const std::string& shadersPath);
	bool LoadSetupFromFile(std::string fileName, bool reload = false);
	void Update(float timeStep);
	void Render(const HybridCam &camera);
private:
	struct BallVar{
		BallVar() : position(0,0,0), speed(0,0,0) {}
		vector3f position;
		vector3f speed;
	};
	struct BallConst{
		BallConst() : radius(0), mass(0), bounceFactor(0), deformationFactor(0) {}
		float radius;
		float mass;
		float bounceFactor;
		float deformationFactor;
	};
	struct Wall{
		Wall() : position(0,0,0), normal(0,0,0), width(0), height(0), deformationFactor(0){}
		vector3f position;
		vector3f normal, right;
		float width, height;
		float deformationFactor;
	};
	struct WallRenderInfo{
		WallRenderInfo() : position(0), color(0,0,0){}
		vector3f *position;
		vector3f color;
	};
	struct Vortex{
		Vortex() : position(0,0,0), direction(0,0,0), length(0), radius(0), intensity(0) {}
		vector3f position;
		vector3f direction;
		float length;
		float radius;
		float intensity;
	};
	struct VortexRenderInfo{
		VortexRenderInfo() : position(0), color(0,0,0){}
		vector3f *position;
		vector3f color;
	};

	std::vector<BallVar> ballsV;
	std::vector<BallConst> ballsC;
	std::vector<Wall> walls;
	std::vector<WallRenderInfo> wallRenderInfo;
	std::vector<Vortex> vortices;
	std::vector<VortexRenderInfo> vortRenderInfo;

	us::UniShader updateShader;
	us::Uniform::Ptr usBallVars;
	us::Uniform::Ptr usBallConsts;
	us::Uniform::Ptr usWalls;
	us::Uniform::Ptr usVortices;
	us::Uniform::Ptr usTotalWalls;
	us::Uniform::Ptr usTotalVortices;
	us::Uniform::Ptr usTotalBalls;
	us::Uniform::Ptr usGravityFactor;
	us::Uniform::Ptr usTimeStep;
	us::TextureBuffer::Ptr usBallVarsTBuf;
	us::TextureBuffer::Ptr usBallConstsTBuf;
	us::TextureBuffer::Ptr usWallsTBuf;
	us::TextureBuffer::Ptr usVorticesTBuf;
	us::Varying::Ptr usPosOutV;
	us::Varying::Ptr usSpeedOutV;

	us::UniShader renderShader;
	us::Attribute::Ptr rsPosition;
	us::Attribute::Ptr rsRadius;
	us::Uniform::Ptr rsEye;

	us::Buffer<BallVar>::Ptr ballsVBuf;
	us::Buffer<BallConst>::Ptr ballsCBuf;
	us::Buffer<Wall>::Ptr wallsBuf;
	us::Buffer<Vortex>::Ptr vorticesBuf;

	float m_gravityFactor;
	bool m_loaded;
};

#endif