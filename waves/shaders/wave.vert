#version 150 compatibility

uniform vec3 LightPos;
uniform vec3 CamPos;
uniform vec3 UsedColor;
uniform samplerBuffer WaveField;
uniform usamplerBuffer WaveTimes;
uniform int WaveN;
uniform uint Time;

out vec3 Color;

in vec3 Position;

const float offset = 0.01;
const float PI = 3.14159265358979323846264;
const float SpecularContribution = 0.5;
const float DiffuseContribution = 0.7 - SpecularContribution;
const float AmbientLight = 0.3;

struct WP{
	float amp, velo, freq;
	vec3 mid;
} wave;

float mod(float a, float b){
	return a-(b * int(a / b));
}

float mod(uint a, float b){
	return a-(b * uint(a / b));
}

float CalcIntensity(vec3 position, vec3 normal){
	vec3 reflLight = reflect(normalize(position-LightPos), normal);
	vec3 iView = normalize(CamPos - position);
	float diffIntensity = max(0.f, dot(iView, reflLight));
	float specIntensity = 0.f;
	if(diffIntensity > 0.f){
		specIntensity = pow(diffIntensity,16.f);
	}
	return DiffuseContribution*diffIntensity + SpecularContribution*specIntensity + AmbientLight;
}

void main(void){
	//setting points
	vec3 pos[5];
	for(int i = 0; i < 5; i++)
		pos[i] = Position;
	pos[1].x += offset;
	pos[2].x -= offset;
	pos[3].z += offset;
	pos[4].z -= offset;

	for(int i = 0; i < WaveN; i++){ //repeating for N waves
        uint waveTime = texelFetch(WaveTimes, i).r;
        
        if(Time < waveTime)
            continue;
            
		uint timeDif = Time - waveTime;
            
		//filing struct
		wave.amp = texelFetch(WaveField, i*6+0).r;
		wave.velo = texelFetch(WaveField, i*6+1).r;
		wave.freq = texelFetch(WaveField, i*6+2).r;
		wave.mid.x = texelFetch(WaveField, i*6+3).r;
		wave.mid.y = texelFetch(WaveField, i*6+4).r;
		wave.mid.z = texelFetch(WaveField, i*6+5).r;
	
		//calculating points
		for(int j = 0; j < 5; j++){
			float distance = length(pos[j].xz-wave.mid.xz);
			float phase = distance/wave.velo - timeDif/1000.0f;
			if(phase <= 0){
				//calculate with more precise time
				float timePer = mod(timeDif,1000.0f/wave.freq)/1000.0f;
				phase = distance/wave.velo - timePer;
				pos[j].y += wave.amp*sin(2*PI*wave.freq*phase);
			}
		}
	}
		
	//calculate normal
	vec3 v1,v2,norm;
	v1 = pos[2] - pos[1];
	v2 = pos[4] - pos[3];
	norm = cross(v1,v2);
	norm = normalize(norm);

	gl_Position = gl_ModelViewProjectionMatrix*vec4(pos[0],1);
	Color = UsedColor*CalcIntensity(pos[0],-norm);
}