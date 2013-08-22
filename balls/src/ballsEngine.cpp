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

#include "ballsEngine.h"
#include <iomanip>
#include <iostream>
#include <fstream>

BallsEngine::BallsEngine(){
	m_gravityFactor = 9.81f;
	m_loaded = false;
}

void BallsEngine::loadShaders(const std::string& shadersPath)
{
	//create shaders
	{
		us::ShaderProgram::Ptr updateProgram = us::ShaderProgram::create();
		us::ShaderObject::Ptr usVertObj = us::ShaderObject::create();
		usVertObj->loadFile(shadersPath + "ballsUpdate.vert");
		usBallVars = updateProgram->getInput()->addUniform("ballVars");
		usBallConsts = updateProgram->getInput()->addUniform("ballConsts");
		usWalls = updateProgram->getInput()->addUniform("walls");
		usVortices = updateProgram->getInput()->addUniform("vortices");
		usTotalWalls = updateProgram->getInput()->addUniform("totalWalls");
		usTotalVortices = updateProgram->getInput()->addUniform("totalVortices");
		usTotalBalls = updateProgram->getInput()->addUniform("totalBalls");
		usGravityFactor = updateProgram->getInput()->addUniform("gravityFactor");
		usTimeStep = updateProgram->getInput()->addUniform("timeStep");
		usBallVarsTBuf = us::TextureBuffer::create();
		usBallConstsTBuf = us::TextureBuffer::create();
		usWallsTBuf = us::TextureBuffer::create();
		usVorticesTBuf = us::TextureBuffer::create();
		usPosOutV = updateProgram->getOutput()->addVarying("retBallPos");
		usSpeedOutV = updateProgram->getOutput()->addVarying("retBallSpeed");
		updateProgram->getOutput()->interleave(true);
		updateProgram->addShaderObject(usVertObj);
		updateShader.connectProgram(updateProgram);
	}

	{
		us::ShaderProgram::Ptr renderProgram = us::ShaderProgram::create();
		us::ShaderObject::Ptr rsVertObj = us::ShaderObject::create();
		rsVertObj->loadFile(shadersPath + "ballsRender.vert");
		us::ShaderObject::Ptr rsGeomObj = us::ShaderObject::create();
		rsGeomObj->loadFile(shadersPath + "ballsRender.geom");
		us::ShaderObject::Ptr rsFragObj = us::ShaderObject::create();
		rsFragObj->loadFile(shadersPath + "ballsRender.frag");
		rsPosition = renderProgram->getInput()->addAttribute("position");
		rsRadius = renderProgram->getInput()->addAttribute("radius");
		rsEye = renderProgram->getInput()->addUniform("eye");
		renderProgram->addShaderObject(rsVertObj);
		renderProgram->addShaderObject(rsGeomObj);
		renderProgram->addShaderObject(rsFragObj);
		renderShader.connectProgram(renderProgram);
	}

	ballsVBuf = us::Buffer<BallVar>::create();
	ballsCBuf = us::Buffer<BallConst>::create();
	wallsBuf = us::Buffer<Wall>::create();
	vorticesBuf = us::Buffer<Vortex>::create();

	usBallVarsTBuf->connectBuffer(ballsVBuf, 3, us::TextureBuffer::DataType::FLOAT);
	usBallConstsTBuf->connectBuffer(ballsCBuf, 1, us::TextureBuffer::DataType::FLOAT);
	usWallsTBuf->connectBuffer(wallsBuf, 1, us::TextureBuffer::DataType::FLOAT);
	usVorticesTBuf->connectBuffer(vorticesBuf, 1, us::TextureBuffer::DataType::FLOAT);

	usBallVars->setSource(usBallVarsTBuf);
	usBallConsts->setSource(usBallConstsTBuf);
	usWalls->setSource(usWallsTBuf);
	usVortices->setSource(usVorticesTBuf);
	
	rsPosition->connectBuffer(ballsVBuf, 0, 3*sizeof(float));
	rsPosition->setReadingMode(us::Attribute::ReadingMode::FLOAT);
	rsRadius->connectBuffer(ballsCBuf, 0, 3*sizeof(float));
	rsRadius->setReadingMode(us::Attribute::ReadingMode::FLOAT);
}

bool BallsEngine::LoadSetupFromFile(std::string fileName, bool reload){
	std::ifstream fin(fileName.c_str());
	if(fin.fail()){
		std::cout << "Couldn't open file " << fileName << std::endl;
		return false;
	}
	sf::Uint32 i = 0, k = 0, l = 0;
	if(!reload){
		//clear data
		ballsV.clear();
		ballsC.clear();
		for(unsigned int i = 0; i <  wallRenderInfo.size(); i++){
			delete(wallRenderInfo[i].position); 
			wallRenderInfo[i].position = 0;
		}
		for(unsigned int i = 0; i <  vortRenderInfo.size(); i++){
			delete(vortRenderInfo[i].position); 
			vortRenderInfo[i].position = 0;
		}
		walls.clear();
		vortices.clear();
		wallRenderInfo.clear();
		vortRenderInfo.clear();
	}

	short type;
	while(!fin.eof()){
		fin >> type;
		if(type == 0){
			if(!reload){
				ballsV.push_back(BallVar());
				ballsC.push_back(BallConst());
			}
			for(int j=0; j<3; j++) fin >> ballsV[i].position[j];
			for(int j=0; j<3; j++) fin >> ballsV[i].speed[j];
			fin >> ballsC[i].radius;
			fin >> ballsC[i].mass;
			fin >> ballsC[i].bounceFactor;
			fin >> ballsC[i].deformationFactor;
			i++;
		}
		else if(type == 1){
			if(!reload){
				walls.push_back(Wall());
				wallRenderInfo.push_back(WallRenderInfo());
				wallRenderInfo[k].position = new vector3f[4];
			}
			for(int j=0; j<3; j++) fin >> walls[k].position[j];
			for(int j=0; j<3; j++) fin >> walls[k].normal[j];
			walls[k].normal = walls[k].normal.normalize();
			fin >> walls[k].width >> walls[k].height;
			fin >> walls[k].deformationFactor;
			for(int j=0; j<3; j++) fin >> wallRenderInfo[k].color[j];

			//prebuild information for rendering
			vector3f third;
			if(abs(walls[k].normal.z) != 1)
				walls[k].right = vector3f(0,0,1);
			else
				walls[k].right = vector3f(0,1,0);
			walls[k].right = walls[k].normal.crossProduct(walls[k].right);
			third = walls[k].normal.crossProduct(walls[k].right);
			wallRenderInfo[k].position[0] = walls[k].position-walls[k].right*walls[k].width-third*walls[k].height;
			wallRenderInfo[k].position[1] = walls[k].position+walls[k].right*walls[k].width-third*walls[k].height;
			wallRenderInfo[k].position[2] = walls[k].position-walls[k].right*walls[k].width+third*walls[k].height;
			wallRenderInfo[k].position[3] = walls[k].position+walls[k].right*walls[k].width+third*walls[k].height;
			k++;
		}
		else if(type == 2){
			if(!reload){
				vortices.push_back(Vortex());
				vortRenderInfo.push_back(VortexRenderInfo());
				vortRenderInfo[l].position = new vector3f[2];
			}
			for(int j=0; j<3; j++) fin >> vortices[l].position[j];
			for(int j=0; j<3; j++) fin >> vortices[l].direction[j];
			vortices[l].direction = vortices[l].direction.normalize();
			fin >> vortices[l].length;
			fin >> vortices[l].radius;
			fin >> vortices[l].intensity;
			for(int j=0; j<3; j++) fin >> vortRenderInfo[l].color[j];

			//prebuild information for rendering
			vortRenderInfo[l].position[0] = vortices[l].position;
			vortRenderInfo[l].position[1] = vortices[l].position+vortices[l].direction*vortices[l].length;
			l++;
		}
	}
	fin.close();

	//load data to gpu buffers
	ballsVBuf->setData(&ballsV[0], ballsV.size());
	ballsCBuf->setData(&ballsC[0], ballsC.size());
	if(walls.size() > 0)
		wallsBuf->setData(&walls[0], walls.size());
	if(vortices.size() > 0)
		vorticesBuf->setData(&vortices[0], vortices.size());
	
	//set uniforms
	int temp = walls.size();
	usTotalWalls->setSource(&temp, 1);
	temp = vortices.size();
	usTotalVortices->setSource(&temp, 1);
	temp = ballsV.size();
	usTotalBalls->setSource(&temp,1);
	
	m_loaded = true;

	return true;
}

void BallsEngine::Update(float timeStep){
 	usTimeStep->setSource(&timeStep, 1);
	usGravityFactor->setSource(&m_gravityFactor, 1);
	updateShader.render(us::PrimitiveType::POINTS, ballsV.size());
	std::vector<BallVar> temp;
	updateShader.program()->getOutput()->getInterleavedBuffer<BallVar>()->getDataCopy(temp);
	ballsVBuf->setData(temp);
}

void BallsEngine::Render(const HybridCam &camera){
	if(!m_loaded)
		return;

	glPointSize(20);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	for(unsigned int i = 0; i < wallRenderInfo.size(); i++){
		if(wallRenderInfo[i].color != vector3f(1.f,2.f,1.f)){
			glBegin(GL_TRIANGLE_STRIP);
				glColor3f(wallRenderInfo[i].color.x,wallRenderInfo[i].color.y,wallRenderInfo[i].color.z);
				glVertex3f(wallRenderInfo[i].position[0].x, wallRenderInfo[i].position[0].y, wallRenderInfo[i].position[0].z);
				glVertex3f(wallRenderInfo[i].position[1].x, wallRenderInfo[i].position[1].y, wallRenderInfo[i].position[1].z);
				glVertex3f(wallRenderInfo[i].position[2].x, wallRenderInfo[i].position[2].y, wallRenderInfo[i].position[2].z);
				glVertex3f(wallRenderInfo[i].position[3].x, wallRenderInfo[i].position[3].y, wallRenderInfo[i].position[3].z);
			glEnd();
		}
	}

	for(unsigned int i = 0; i < vortRenderInfo.size(); i++){
		if(vortRenderInfo[i].color != vector3f(1.f,2.f,1.f)){
			glLineWidth(3.0f);
			glBegin(GL_LINES);
				glColor3f(vortRenderInfo[i].color.x,vortRenderInfo[i].color.y,vortRenderInfo[i].color.z);
				glVertex3f(vortRenderInfo[i].position[0].x, vortRenderInfo[i].position[0].y, vortRenderInfo[i].position[0].z);
				glVertex3f(vortRenderInfo[i].position[1].x, vortRenderInfo[i].position[1].y, vortRenderInfo[i].position[1].z);
			glEnd();
		}
	}

	float temp[3];
	temp[0] = camera.GetEye().x; temp[1] = camera.GetEye().y; temp[2] = camera.GetEye().z;
	rsEye->setSource(temp, 3);

	renderShader.render(us::PrimitiveType::POINTS, ballsV.size());

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}