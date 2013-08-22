#version 150 compatibility

uniform vec3 eye;
const vec3 lightPos = vec3(50,80,50);
const float diffuseContr = 0.4f;
const float specularContr = 0.2f;
const float ambientContr = 0.3f;

in float fRadius;
in vec3 middle;
in vec3 direction;
out vec4 color;

const float PI = 3.14159265358979323846264;

float CalcIntensity(){
	float angle = PI/2.f*(1.f-length(direction));
	vec3 normal = normalize(normalize(eye-middle)*sin(angle)+direction*cos(angle));
	vec3 position = middle+normal*fRadius;
	vec3 iLightVec = lightPos - position;
	float distance = length(iLightVec);
	iLightVec = normalize(iLightVec);
	vec3 halfVector = normalize(iLightVec + eye);
	float diffIntensity = max(0.f, dot(normal, iLightVec));
	float specIntensity = 0.f;
	if(diffIntensity != 0){
		specIntensity = pow(max(0.f, dot(normal, halfVector)), 16.0f);
	}
	return diffuseContr*diffIntensity + specularContr*specIntensity + ambientContr;
}

void main(){
	color = vec4(vec3(1.f,0.f,0.f)*CalcIntensity(), 1.f);
}