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
#ifndef HYBRID_CAM_H
#define HYBRID_CAM_H

#include <cfloat>
#include "openGL.h"
#include "math3d.h"

#define gimLockBuffer FLT_EPSILON*1E3

class HybridCam{
public:
	HybridCam();
	void Init();
	void Clear();
	void DrawCrosshair();
	const vector3f &GetEye() const {return eye;}
	const vector3f &GetRLookAt() const {return rLookAt;}
	const vector3f &GetLookAt() const {return lookAt;}
	const vector3f &GetUp() const {return up;}
	bool IsOrbiting() const { return orbit; }
	bool IsChanged() const { return changed; }
	void AnnulChanges() { changed = false; }
	void MoveA(const vector3f &vec);
	void MoveR(const vector3f &vec);
	void RotateA(const vector3f &vec);
	void RotateR(const vector3f &vec);
	void InvertY(){invertedY = !invertedY;}
	void SwitchCamMode(bool orbit);
	void ZoomR(const float& zoom);
	void Reset();
protected:
	vector3f eye, lookAt, up;
	bool invertedY, orbit, changed;
	float orbitDist;
	vector3f rLookAt;
	vector3f rotations;

	//croshair variables
	bool remakeAxis;
	float *axisColors;
	float *axisEnds;
	GLuint colorVBO, vertexVBO;
	GLuint crosshairEndVAO, crosshairLineVAO;
};

#endif