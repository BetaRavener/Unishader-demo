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
#ifndef MATH_3D
#define MATH_3D

#include <GL/glew.h>
#include <cmath>
#include <vector>

static const float M_PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
static const float M_PI_2 = M_PI/2;
static const float _2_M_PI = 2*M_PI;
static const float _3_M_PI_2 = 3*M_PI/2;
#define Deg2Rad(deg) (float)(deg*M_PI/180)

class vector3d{
public:
	GLdouble x,y,z;
};

class vector3f{
public:
	GLfloat x,y,z;

	vector3f() {};
	vector3f(const vector3f &a) : x(a.x), y(a.y), z(a.z) {}
	vector3f(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}
	GLfloat &operator [](unsigned int i){switch(i){case 0: return x; case 1: return y; case 2: return z; default: return x;}}
	std::vector<GLfloat> toStdVector() const;

	void set(float nx, float ny, float nz) {x=nx; y=ny; z=nz;}
	void operator =(const vector3f &a){x = a.x; y = a.y; z = a.z;}
	void operator =(const vector3d &a){x = (GLfloat)a.x; y = (GLfloat)a.y; z = (GLfloat)a.z;}
	vector3f operator -() const { return vector3f(-x,-y,-z); }

	vector3f operator +(const vector3f &a)const{return vector3f(x+a.x, y+a.y, z+a.z);}
	void operator +=(const vector3f &a){x+=a.x; y+=a.y; z+=a.z;}
	vector3f operator -(const vector3f &a)const{return vector3f(x-a.x, y-a.y, z-a.z);}
	void operator -=(const vector3f &a){x -= a.x; y -= a.y; z -= a.z;}
	vector3f operator *(const float &a)const{return vector3f(x*a, y*a, z*a);}
	void operator *=(const float &a){x*=a; y*=a; z*=a;}
	vector3f operator /(const float &a)const{float oneOverA = 1.0f/a; return vector3f(x*oneOverA, y*oneOverA, z*oneOverA);}
	void operator /=(const float &a){float oneOverA = 1.0f/a; x*=oneOverA; y*=oneOverA; z*=oneOverA;}

	bool operator ==(const vector3f &a) const{return x==a.x && y==a.y && z==a.z;}
	bool operator ==(const float &a) const{return x==a && y==a && z==a;}
	bool operator !=(const vector3f &a) const{return x!=a.x || y!=a.y || z!=a.z;}

	void zero(){x = y = z = 0.0f;}
	vector3f normalize();
	GLfloat magnitude();
	GLfloat dotProduct (const vector3f &a)const;
	vector3f crossProduct (const vector3f &a)const;
	vector3f reflect (const vector3f &n)const;

	vector3f rotateX(const float &radians);
	vector3f rotateY(const float &radians);
};

class vector2f{
public:
	GLfloat x,y;

	vector2f() {};
	vector2f(const vector2f &a) : x(a.x), y(a.y){}
	vector2f(float nx, float ny) : x(nx), y(ny) {}

	void set(float nx, float ny) {x=nx; y=ny;}
	void operator =(const vector2f &a){x = a.x; y = a.y;}
	vector2f operator -() const { return vector2f(-x,-y);}

	vector2f operator +(const vector2f &a)const{return vector2f(x+a.x, y+a.y);}
	void operator +=(const vector2f &a){x+=a.x; y+=a.y;}
	vector2f operator -(const vector2f &a)const{return vector2f(x-a.x, y-a.y);}
	void operator -=(const vector2f &a){x -= a.x; y -= a.y;}
	vector2f operator *(const float &a)const{return vector2f(x*a, y*a);}
	void operator *=(const float &a){x*=a; y*=a;}
	vector2f operator /(const float &a)const{float oneOverA = 1.0f/a; return vector2f(x*oneOverA, y*oneOverA);}
	void operator /=(const float &a){float oneOverA = 1.0f/a; x*=oneOverA; y*=oneOverA;}

	bool operator ==(const vector2f &a) const{return x==a.x && y==a.y;}
	bool operator ==(const float &a) const{return x==a && y==a;}
	bool operator !=(const vector2f &a) const{return x!=a.x || y!=a.y;}

	void zero(){x = y = 0.0f;}
	void normalize();
	GLfloat dotProduct(const vector2f &a)const;
};

const vector3f zeroVector3f(0,0,0);
const vector2f zeroVector2f(0,0);
const vector3f xAxis(1,0,0);
const vector3f yAxis(0,1,0);
const vector3f zAxis(0,0,1);

struct Matrix4d{
public:
	GLdouble elements[16];
};

struct Matrix3f{
public:
	GLfloat &operator[](unsigned int i){return elements[i];}
	GLfloat elements[9];
};

namespace myMath{
	float getSign(float a);
	inline float mod(const float &a, const float &b) { return a - ((int)(a/b))*b; }
	inline float clampU(const float &a, const float &b) { return (a > b) ? b : a; }
	inline float clampD(const float &a, const float &b) { return (a < b) ? b : a; }
	int AABBPlaneIntersection(const vector3f &min,const vector3f &max, const vector3f &normal, const vector3f &vec);
	bool PointInsideRect(const vector2f &point, const vector2f &rectLDC, const vector2f &rectRUC);
}

#endif