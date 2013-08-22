#version 150 compatibility

uniform samplerBuffer ballVars;
uniform samplerBuffer ballConsts;
uniform samplerBuffer walls;
uniform samplerBuffer vortices;
uniform int totalWalls;
uniform int totalVortices;
uniform int totalBalls;
uniform float gravityFactor;
uniform float timeStep;
in int dummyAttrib;

#define ball1 0
#define ball2 1

struct BallVar{
	vec3 position;
	vec3 speed;
};

struct BallConst{
	float radius;
	float mass;
	float bounceFactor;
	float deformationFactor;
};

BallVar ballsV[2];
BallConst ballsC[2];

out vec3 retBallPos;
out vec3 retBallSpeed;

void WallCollision(int i){
	vec3 wallP, wallN, wallR, third;
	float width, height, wallD;
	wallP.x = texelFetch(walls,i*12+0).r;
	wallP.y = texelFetch(walls,i*12+1).r;
	wallP.z = texelFetch(walls,i*12+2).r;
	wallN.x = texelFetch(walls,i*12+3).r;
	wallN.y = texelFetch(walls,i*12+4).r;
	wallN.z = texelFetch(walls,i*12+5).r;
	wallR.x = texelFetch(walls,i*12+6).r;
	wallR.y = texelFetch(walls,i*12+7).r;
	wallR.z = texelFetch(walls,i*12+8).r;
	
	vec3 joint = ballsV[0].position-wallP;
	float distance = dot(wallN, joint);
	if(distance < 0){
		wallN = -wallN;
		distance = 0.f-distance;
	}
	
	if(distance <= ballsC[0].radius){
		//check if ball is inside plane
		width = texelFetch(walls,i*12+9).r;
		height = texelFetch(walls,i*12+10).r;
		third = cross(wallN, wallR);

		vec3 insideVec = ballsV[0].position - distance*wallN;
		vec3 realNormal = vec3(0,0,0);

		float wd, hd;
		wd = dot(wallR, insideVec);
		hd = dot(third, insideVec);

		if(abs(wd) <= width && abs(hd) <= height){
			//entirely
			realNormal = wallN;
		}
		else if(abs(wd)-ballsC[0].radius <= width && abs(hd)-ballsC[0].radius <= height){
			//at the edge, compute edge normal
			if(abs(wd) > width)
				realNormal += wallR*wd;
			if(abs(hd) > height)
				realNormal += third*hd;
			realNormal = normalize(realNormal);
		}
		else{
			//not at all
			return;
		}
				
		wallD = texelFetch(walls,i*12+11).r;			
		ballsV[0].speed = reflect(ballsV[0].speed, realNormal)*ballsC[0].bounceFactor*(1-wallD);

		//update ball so it's not behind wall
		float intersection = ballsC[0].radius-distance;
		if(intersection > 0)
			ballsV[0].position += realNormal*intersection;
	}
}

void ApplyVortex(int i){
	vec3 pos, dir;
	float radius, intensity, len;
	pos.x = texelFetch(vortices,i*9+0).r;
	pos.y = texelFetch(vortices,i*9+1).r;
	pos.z = texelFetch(vortices,i*9+2).r;
	dir.x = texelFetch(vortices,i*9+3).r;
	dir.y = texelFetch(vortices,i*9+4).r;
	dir.z = texelFetch(vortices,i*9+5).r;
	len = texelFetch(vortices,i*9+6).r;
	radius = texelFetch(vortices,i*9+7).r;
	vec3 joint = ballsV[0].position-pos;
	float mapped = dot(joint, dir);
	if(mapped <= len && mapped >= 0){
		float distance = length(joint-mapped*dir);
		if(distance <= radius){
			intensity = texelFetch(vortices,i*9+8).r;
			ballsV[0].speed += dir*(intensity*(timeStep/1000.0f));
		}
	}
}

void fastLoad(int vID, int i){
    ballsV[i].position = texelFetch(ballVars,vID*2+0).rgb;
    ballsC[i].radius = texelFetch(ballConsts,vID*4+0).r;
}

void restLoad(int vID, int i){
    ballsV[i].speed = texelFetch(ballVars,vID*2+1).rgb;
	ballsC[i].mass = texelFetch(ballConsts,vID*4+1).r;
	ballsC[i].bounceFactor = texelFetch(ballConsts,vID*4+2).r;
	ballsC[i].deformationFactor = texelFetch(ballConsts,vID*4+3).r;
}

void completeLoad(int vID, int i){
    ballsV[i].position = texelFetch(ballVars,vID*2+0).rgb;
	ballsV[i].speed = texelFetch(ballVars,vID*2+1).rgb;
	ballsC[i].radius = texelFetch(ballConsts,vID*4+0).r;
	ballsC[i].mass = texelFetch(ballConsts,vID*4+1).r;
	ballsC[i].bounceFactor = texelFetch(ballConsts,vID*4+2).r;
	ballsC[i].deformationFactor = texelFetch(ballConsts,vID*4+3).r;
}

void BallCollision(int i){
	fastLoad(i, ball2);
	float radiusSum = ballsC[ball1].radius + ballsC[ball2].radius;
	vec3 joint = ballsV[ball2].position - ballsV[ball1].position;
    if(joint[0] > radiusSum && joint[1] > radiusSum && joint[2] > radiusSum)
        return;
        
    restLoad(i, ball2);
    
	float d = length(joint);
	if(d <= radiusSum){
		vec3 collisionDirectionN = joint/d;
		float colSpeed[2], refSpeed; 
		colSpeed[0] = dot(collisionDirectionN, ballsV[ball1].speed);
		colSpeed[1] = dot(collisionDirectionN, ballsV[ball2].speed);

		//balls won't hit
		if(colSpeed[1] > colSpeed[0])
			return;

		//calculate new speed vector
		refSpeed =	(colSpeed[0] * (ballsC[ball1].mass - ballsC[ball2].mass) + 2 * ballsC[ball2].mass * colSpeed[1]) /
					(ballsC[ball1].mass + ballsC[ball2].mass);
		refSpeed = refSpeed * (1 - ballsC[ball2].deformationFactor) * ballsC[ball1].bounceFactor;
		ballsV[ball1].speed = ballsV[ball1].speed - collisionDirectionN * colSpeed[0] + collisionDirectionN * refSpeed;

		//update balls so they don't intersect
		float intersection = (ballsC[ball1].radius + ballsC[ball2].radius) - d;
		if(intersection > 0)
			ballsV[ball1].position -= collisionDirectionN * (intersection / 2.0f);
			//second ball will take the other portion
	}
}

void main(void){
	completeLoad(gl_VertexID, ball1);

	for(int i = 0; i < totalBalls; i++){
		if(i != gl_VertexID)
			BallCollision(i);
	}
	
	ballsV[ball1].speed.y -= (gravityFactor*(timeStep/1000.0f));
	
	for(int i = 0; i < totalWalls; i++)
		WallCollision(i);
		
	for(int i = 0; i < totalVortices; i++)
		ApplyVortex(i);
	  
    //update position
	ballsV[ball1].position += (ballsV[ball1].speed*(timeStep/1000.0f));
	retBallPos = ballsV[ball1].position;
	retBallSpeed = ballsV[ball1].speed;
}