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

#include "waveView.h"
#include <iostream>
#include <string>

WaveView::WaveView() :
fovy(45.0f),
nearPlane(0.1f),
farPlane(500.0f),
needProjUpdate(true),
needModelViewUpdate(true),
active(false),
mySize(sf::Vector2u(400,400))
{}

void WaveView::Init(){
	camera.Init();
	camera.SwitchCamMode(true);
	camera.MoveA(vector3f(0,0,0));
	camera.RotateA(vector3f(0,M_PI/4,0));
	camera.ZoomR(-3);
}

void WaveView::Activate(){
	if(!active){
		glGetDoublev(GL_PROJECTION_MATRIX, savedProjMatrix.elements);
		glGetDoublev(GL_MODELVIEW_MATRIX, savedModelViewMatrix.elements);
		if(needProjUpdate){
			UpdateProjectionMatrix();
			needProjUpdate = false;
		}
		if(needModelViewUpdate || camera.IsChanged()){
			UpdateModelViewMatrix();
			needModelViewUpdate = false;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(projMatrix.elements);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(modelViewMatrix.elements);
		active = true;
	}
}

void WaveView::Deactivate(){
	if(active){
		camera.AnnulChanges();
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(savedProjMatrix.elements);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(savedModelViewMatrix.elements);
		active = false;
	}
}

void WaveView::SetSize(const sf::Uint32 &width, const sf::Uint32 &height){
	mySize = sf::Vector2u(width,height);
	UpdateProjectionMatrix();
}

void WaveView::SetSize(const sf::Vector2u &size){
	mySize = size;
	UpdateProjectionMatrix();
}

void WaveView::UpdateProjectionMatrix(){
	glViewport(0, 0, mySize.x, mySize.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (float)mySize.x/(float)mySize.y, nearPlane, farPlane);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix.elements);
}

void WaveView::UpdateModelViewMatrix(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	camera.GetEye().x,camera.GetEye().y,camera.GetEye().z,
				camera.GetLookAt().x,camera.GetLookAt().y,camera.GetLookAt().z,
				camera.GetUp().x,camera.GetUp().y,camera.GetUp().z);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix.elements);
}