#version 150 compatibility

in vec3 position;
in float radius;
out float gRadius;

void main(){
	gRadius = radius;
	gl_Position = vec4(position,1);
}