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

#include "hybridCam.h"

HybridCam::HybridCam():
remakeAxis(true),
axisEnds(0),
axisColors(0){}

void HybridCam::Init(){
	Reset();
	if(axisColors == 0)
		axisColors = new float[18];
	
	axisColors[0] = 1.0f;	axisColors[1] = 0;		axisColors[2] = 0;
	axisColors[3] = 1.0f;	axisColors[4] = 0;		axisColors[5] = 0;
	axisColors[6] = 0;		axisColors[7] = 1.0f;	axisColors[8] = 0;
	axisColors[9] = 0;		axisColors[10] = 1.0f;	axisColors[11] = 0;
	axisColors[12] = 0;		axisColors[13] = 0;		axisColors[14] = 1.0f;
	axisColors[15] = 0;		axisColors[16] = 0;		axisColors[17] = 1.0f;

	glGenVertexArrays(1, &crosshairLineVAO);
	glGenVertexArrays(1, &crosshairEndVAO);

	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(float), 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(float), axisColors, GL_DYNAMIC_DRAW);
	printGLError();

	glBindVertexArray(crosshairLineVAO);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glColorPointer(3, GL_FLOAT, 0, 0);
	glBindVertexArray(crosshairEndVAO);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexPointer(3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glColorPointer(3, GL_FLOAT, 6*sizeof(float), 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	printGLError();
	delete(axisColors); axisColors = 0;
}

void HybridCam::DrawCrosshair(){
	const vector3f &LA = lookAt;
	
	if(remakeAxis){
		if(axisEnds == 0)
			axisEnds = new float[18];
		float a = 1.5f;
		//create corners - indices
		axisEnds[0] = lookAt.x-a;	axisEnds[1] = lookAt.y;		axisEnds[2] = lookAt.z;
		axisEnds[3] = lookAt.x+a;	axisEnds[4] = lookAt.y;		axisEnds[5] = lookAt.z;
		axisEnds[6] = lookAt.x;		axisEnds[7] = lookAt.y-a;	axisEnds[8] = lookAt.z;
		axisEnds[9] = lookAt.x;		axisEnds[10] = lookAt.y+a;	axisEnds[11] = lookAt.z;
		axisEnds[12] = lookAt.x;	axisEnds[13] = lookAt.y;	axisEnds[14] = lookAt.z-a;
		axisEnds[15] = lookAt.x;	axisEnds[16] = lookAt.y;	axisEnds[17] = lookAt.z+a;
		remakeAxis = false;
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, 18*sizeof(float), axisEnds, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete(axisEnds); axisEnds = 0;
	}
	
	//enable states
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(crosshairLineVAO);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(4.0f);
		glDrawArrays(GL_LINES, 0, 6);
		glDisable(GL_LINE_SMOOTH);
	glBindVertexArray(crosshairEndVAO);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 3);
		glDisable(GL_POINT_SMOOTH);
	glBindVertexArray(0);
	///use this to correct error
	glPointSize(0.01f);
	glBegin(GL_POINTS);
	glColor4f(0,0,0,0);
	glVertex3f(0,0,0);
	glEnd();
	///
	glDisable(GL_BLEND);

	//disable states
    glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}

void HybridCam::Clear(){
	glDeleteBuffers(1,&colorVBO);
	glDeleteBuffers(1,&vertexVBO);
	glDeleteVertexArrays(1,&crosshairEndVAO);
	glDeleteVertexArrays(1,&crosshairLineVAO);
}

void HybridCam::Reset(){
	eye = vector3f(0,0,0);
	rotations = zeroVector3f;
	rLookAt = vector3f(0,0,-1);
	lookAt = eye + rLookAt;
	up = vector3f(0,1,0);
	orbitDist = 5;
	orbit = true;
	invertedY = true;
	changed = true;
}

void HybridCam::MoveA(const vector3f &vec){
	if(orbit){
		lookAt = vec;
		eye = lookAt-rLookAt*orbitDist;
	}
	else{
		eye = vec;
		lookAt = eye + rLookAt;
	}
	changed = true;
	remakeAxis = true;
}

void HybridCam::MoveR(const vector3f &vec){
	vector3f right = rLookAt.crossProduct(up);
	right.normalize();

	if(orbit){
		lookAt += up*vec.y;
		lookAt += right*vec.x;
		lookAt += rLookAt*vec.z;
		
		eye = lookAt-rLookAt*orbitDist;
	}
	else{
		eye += up*vec.y;
		eye += right*vec.x;
		eye += rLookAt*vec.z;
		
		lookAt = eye + rLookAt;
	}
	changed = true;
	remakeAxis = true;
}

void HybridCam::RotateA(const vector3f &vec){
	rotations = zeroVector3f;
	RotateR(vec);
	changed = true;
}

void HybridCam::RotateR(const vector3f &vec){
	if(vec != zeroVector3f){
		if(orbit){
			if(invertedY)
				rotations += vector3f(-vec.x,vec.y,0);
			else
				rotations += vector3f(-vec.x,-vec.y,0);
		}
		else{
			if(invertedY)
				rotations += vector3f(vec.x,-vec.y,0);
			else
				rotations += vector3f(vec.x,vec.y,0);
		}
	}

	//check overruns
	while (rotations.x > _2_M_PI)
		rotations.x -= _2_M_PI;
	while (rotations.x < 0.0f)
		rotations.x += _2_M_PI;
	if(rotations.y > M_PI_2-gimLockBuffer)
		rotations.y = (float)(M_PI_2-gimLockBuffer);
	else if (rotations.y < -M_PI_2+gimLockBuffer)
		rotations.y = -(float)(M_PI_2+gimLockBuffer);

	rLookAt.set(0,0,-1);
	rLookAt.rotateY(rotations.y);
	rLookAt.rotateX(rotations.x);

	up.set(0,1,0);
	up.rotateY(rotations.y);
	up.rotateX(rotations.x);

	if(orbit)
		eye = lookAt-rLookAt*orbitDist;
	else
		lookAt = eye + rLookAt;
	changed = true;
	remakeAxis = true;
}

void HybridCam::SwitchCamMode(bool orbit){
	if(orbit){
		orbitDist = 5;
		lookAt = eye+rLookAt*orbitDist;
	}
	else{
		lookAt = eye + rLookAt;
	}

	this->orbit = orbit;
	changed = true;
	remakeAxis = true;
}

void HybridCam::ZoomR(const float& zoom){
	if(orbit){
		orbitDist -= zoom;
		orbitDist = myMath::clampD(orbitDist,1);
		eye = lookAt-rLookAt*orbitDist;
	}
	changed = true;
	remakeAxis = true;
}

