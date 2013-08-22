#pragma once
#ifndef WAVE_ENGINE
#define WAVE_ENGINE

#include "includes.h"
#include <vector>
#include <deque>
#include <map>
#include "hybridCam.h"

struct WavePar{
	WavePar () : amp(0), velo(0), freq(0), center(vector3f(0,0,0)) {}
	WavePar (float a, float v, float f, vector3f c): 
		amp(a), velo(v), freq(f), center(c){}
	void Clear(){amp = 0; velo = 0; freq = 0; center = vector3f(0,0,0);}
	float amp, velo, freq;
	vector3f center;
};

struct WaveMesh{
	WaveMesh() : width(0),  height(0), spacing(0) {}
	WaveMesh(unsigned int w, unsigned int h, float s) : width(w),  height(h), spacing(s) {}
	unsigned int width, height;
	float spacing;
};

class WaveEngine{
public:
	WaveEngine(std::string shadersPath);
	void AddTime(unsigned int time)	{totalTime += time;}
	void ResetTimer();
	unsigned int GetTotalTime() {return totalTime;}
	bool AreShadersSupported() {return shadersSupported;}
	void SetMesh(const WaveMesh &mesh) {myMesh = mesh;}
	void GenMesh();
	void SwShowPoints() { showPoints = !showPoints; }
	void SwWireframe() { wireframe = !wireframe; }
	WaveMesh GetMesh() {return myMesh;}
	void AddWave(const std::string &waveName);
	void EraseWave(const std::string &waveName);
	void SetWave(const std::string &waveName, const WavePar &params);
	bool GetWave(const std::string &waveName, WavePar &params);
	void SetWaveTime(const std::string &waveName, unsigned int time);
	bool GetWaveTime(const std::string &waveName, unsigned int &time);
	void ExportFromCurContext(bool clear);
	void ImportToNewContext(std::string shadersPath);

	void Render(const HybridCam &camera);
private:
	void TransVectorToGPU();
	void TransMeshToGPU();
	bool ValidWaveCheck(const std::string &waveName, unsigned int *pos = 0);

	us::UniShader upReShader;
	us::Attribute::Ptr upRePosition;
	us::Uniform::Ptr upReWaveField;
	us::Uniform::Ptr upReWaveTimes;
	us::Uniform::Ptr usReWaveN;
	us::Uniform::Ptr usReTime;
	us::Uniform::Ptr usReCamPos;
	us::Uniform::Ptr usReLightPos;
	us::Uniform::Ptr usReUsedColor;
	us::Buffer<vector3f>::Ptr upRePositionBuf;
	us::Buffer<WavePar>::Ptr upReWaveFieldBuf;
	us::Buffer<unsigned int>::Ptr upReWaveTimesBuf;
	us::Buffer<unsigned int>::Ptr meshIndicBuf;

	bool shadersSupported;
	WaveMesh myMesh;
	std::vector<WavePar> waveField;
	std::vector<unsigned int> waveTimes;
	std::vector<std::string> waveNames;
	std::vector<vector3f> meshVert;
	std::vector<unsigned int> meshIndic;
	bool meshGenerated, showPoints, wireframe;
	unsigned int totalTime;
	GLuint vertVBO, indicVBO, upReVAO;
};

#endif