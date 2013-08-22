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

#include "math3d.h"

std::vector<GLfloat> vector3f::toStdVector() const
{
	std::vector<GLfloat> vec(3,0);
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	return vec;
}

vector3f vector3f::reflect (const vector3f &n)const{
	return n*(-2.0f)*(this->dotProduct(n)) + (*this);
}

vector3f vector3f::crossProduct (const vector3f &a)const{
	return vector3f(
		y*a.z - z*a.y,
		z*a.x - x*a.z,
		x*a.y - y*a.x);
}

GLfloat vector3f::dotProduct (const vector3f &a)const{
	return x*a.x + y*a.y + z*a.z;
}

vector3f vector3f::normalize(){
	float oneOverMag = 1.0f/this->magnitude();
	return vector3f(x*oneOverMag,y*oneOverMag,z*oneOverMag);
}

GLfloat vector3f::magnitude(){
	return sqrt(x*x + y*y + z*z);
}

vector3f vector3f::rotateY(const float &radians)
{
	float t_y = y;

	y = t_y*cos(radians) + z*sin(radians);
	z = t_y*-sin(radians) + z*cos(radians);
	return *this;
}

vector3f vector3f::rotateX(const float &radians)
{
	float t_x = x;

	x = t_x*cos(radians) + z*-sin(radians);
	z = t_x*sin(radians) + z*cos(radians);
	return *this;
}

GLfloat vector2f::dotProduct(const vector2f &a)const{
	return x*a.x + y*a.y;
}

void vector2f::normalize(){
	float mag = x*x + y*y;
	float oneOverMag = 1.0f/sqrt(mag);
	x*=oneOverMag;
	y*=oneOverMag;
}

float myMath::getSign(float a){
	if(a>=0)
		return 1;
	else
		return -1;
}

int myMath::AABBPlaneIntersection(const vector3f &min,const vector3f &max, const vector3f &normal, const vector3f &vec){
	float d = vec.dotProduct(normal);
	float minD, maxD;

	if(normal.x > 0.0f){
		minD = normal.x*min.x; maxD = normal.x*max.x;}
	else{
		minD = normal.x*max.x; maxD = normal.x*min.x;}

	if(normal.y > 0.0f){
		minD += normal.y*min.y; maxD += normal.y*max.y;}
	else{
		minD += normal.y*max.y; maxD += normal.y*min.y;}

	if(normal.z > 0.0f){
		minD += normal.z*min.z; maxD += normal.z*max.z;}
	else{
		minD += normal.z*max.z; maxD += normal.z*min.z;}

	if(minD >= d)
		return +1;
	if(maxD <= d)
		return -1;
	return 0;
}

bool myMath::PointInsideRect(const vector2f &point, const vector2f &rectLDC, const vector2f &rectRUC){
	if(point.x > rectLDC.x && point.x < rectRUC.x){
		if(point.y > rectLDC.y && point.y < rectRUC.y)
			return true;
	}
	return false;
}