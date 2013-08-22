#version 150 compatibility

#define maxOut 93
#define minOut 6

layout(points) in;
layout(triangle_strip, max_vertices=93) out;

uniform vec3 eye;

in float gRadius[];
out float fRadius;
out vec3 middle;
out vec3 direction;


const float PI = 3.14159265358979323846264;
const float sqrDeg = 0.2; //def: 0.1
const float linDeg = 0.1; //def: 0.5
const float divFac = 0.03; // def: 0.02

mat3 createRotMatrix(vec3 n, float angle){
	float cosA = cos(angle);
	float mCosA = 1-cosA;
	float sinA = sin(angle);
	return mat3(
		n.x*n.x*mCosA+cosA, n.x*n.y*mCosA-n.z*sinA, n.x*n.z*mCosA+n.y*sinA,
		n.x*n.y*mCosA+n.z*sinA, n.y*n.y*mCosA+cosA, n.y*n.z*mCosA-n.x*sinA,
		n.x*n.z*mCosA-n.y*sinA, n.y*n.z*mCosA+n.x*sinA, n.z*n.z*mCosA+cosA
	);
}

void main(){
	int vertexCount = maxOut/3;
	fRadius = gRadius[0];
	middle = gl_in[0].gl_Position.xyz;
	vec3 ballToEye = eye-gl_in[0].gl_Position.xyz;
	vec3 normal = normalize(ballToEye);
	float viewDist = length(ballToEye);
	
	//degradate ball quality as it is further from viewer
	vertexCount = int(float(vertexCount) / ((sqrDeg*pow(viewDist*divFac, 2) + linDeg*viewDist*divFac)/gRadius[0]));

	vec3 firstDir, tempDir;
	vec4 midPoint, tempPos;
	if(vertexCount > maxOut/3)
		vertexCount = maxOut/3;
	else if(vertexCount < minOut)
		vertexCount = minOut;
	mat3 anglIncrMatrix = createRotMatrix( normal, 2.f*PI/vertexCount);
	
	//form circle facing the camera
	midPoint = gl_ModelViewProjectionMatrix*gl_in[0].gl_Position;
	if(normal.x != 1.0f || normal.x != -1.0f)
		tempDir = firstDir = normalize(cross(normal, vec3(1.f,0.f,0.f)));
	else
		tempDir = firstDir = normalize(cross(normal, vec3(0.f,1.f,0.f)));
	tempPos = gl_ModelViewProjectionMatrix*(gl_in[0].gl_Position+vec4(tempDir,0.f)*gRadius[0]);
	
	for(int i = 0; i < vertexCount-1; i++){
		gl_Position = midPoint;
		direction = vec3(0.f,0.f,0.f);
		EmitVertex();
		
		gl_Position = tempPos;
		direction = tempDir;
		EmitVertex();
		
		tempDir = anglIncrMatrix*tempDir;
		gl_Position = tempPos = gl_ModelViewProjectionMatrix*(gl_in[0].gl_Position+vec4(tempDir,0.f)*gRadius[0]);
		direction = tempDir;
		EmitVertex();
		EndPrimitive();
	}
	//render last triangle (endVertex = startVertex)
	gl_Position = midPoint;
	direction = vec3(0.f,0.f,0.f);
	EmitVertex();
	
	gl_Position = tempPos;
	direction = tempDir;
	EmitVertex();
	
	gl_Position = gl_ModelViewProjectionMatrix*(gl_in[0].gl_Position+vec4(firstDir,0.f)*gRadius[0]);
	direction = firstDir;
	EmitVertex();
	EndPrimitive();
}