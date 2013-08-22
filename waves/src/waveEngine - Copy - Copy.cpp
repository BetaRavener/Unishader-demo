#include "waveEngine.h"

WaveEngine::WaveEngine(std::string shadersPath):
shadersSupported(false),
meshGenerated(false),
showPoints(false),
wireframe(false),
totalTime(0)
{
	ImportToNewContext(shadersPath);
	SetMesh(WaveMesh(100,100,0.5f));
	GenMesh();
}

void WaveEngine::ResetTimer(){
	for(unsigned int i = 0; i < waveTimes.size(); i++)
		waveTimes[i] = 0;
	totalTime = 0;
	TransVectorToGPU();
}

void WaveEngine::GenMesh(){
	const unsigned int &x = myMesh.width; 
	const unsigned int &z = myMesh.height;
	meshVert.clear();
	meshIndic.clear();
	if(x < 2 || z < 2 || myMesh.spacing == 0){
		meshGenerated = false;
		return;
	}
	float xMid = (float)(x-1)/2, zMid = (float)(z-1)/2, baseHeight = 1;
	meshVert.resize(x*z);
	for(unsigned int j = 0; j < z; j++){
		for(unsigned int i = 0; i < x; i++){
			meshVert[j*x+i] = vector3f((GLfloat)((i-xMid)*myMesh.spacing),baseHeight,(GLfloat)((j-zMid)*myMesh.spacing));
		}
	}
	unsigned int i = 0;
	meshIndic.clear();
	for(unsigned int j = 1; j < z; j++){
		for(; i < x*j; i++){
			meshIndic.push_back(i);
			meshIndic.push_back(i+x);
		}
		if(j != z-1){
			meshIndic.push_back(i-1+x);
			meshIndic.push_back(i);
		}
	}
	meshGenerated = true;
	TransMeshToGPU();
}

void WaveEngine::AddWave(const std::string &waveName){
	if(!ValidWaveCheck(waveName)){
		waveField.push_back(WavePar(3, 5, 0.5f, vector3f(0,0,0)));
		waveTimes.push_back(totalTime);
		waveNames.push_back(waveName);
		TransVectorToGPU();
	}
}

void WaveEngine::EraseWave(const std::string &waveName){
	unsigned int pos;
	if(ValidWaveCheck(waveName,&pos)){
		waveField.erase(waveField.begin()+pos);
		waveTimes.erase(waveTimes.begin()+pos);
		waveNames.erase(waveNames.begin()+pos);
		TransVectorToGPU();
	}
}

void WaveEngine::SetWave(const std::string &waveName, const WavePar &params){
	unsigned int pos;
	if(ValidWaveCheck(waveName,&pos)){
		waveField[pos] = params;
		TransVectorToGPU();
	}
}

bool WaveEngine::GetWave(const std::string &waveName, WavePar &params){
	unsigned int pos;
	if(ValidWaveCheck(waveName,&pos)){
		params = waveField[pos];
		return true;
	}
	return false;
}

void WaveEngine::SetWaveTime(const std::string &waveName, unsigned int time){
	unsigned int pos;
	if(ValidWaveCheck(waveName,&pos)){
		waveTimes[pos] = time;
		TransVectorToGPU();
	}
}

bool WaveEngine::GetWaveTime(const std::string &waveName, unsigned int &time){
	unsigned int pos;
	if(ValidWaveCheck(waveName,&pos)){
		time = waveTimes[pos];
		return true;
	}
	return false;
}

void WaveEngine::TransVectorToGPU(){
	if(waveField.size() > 0){
		upReWaveFieldBuf->setData(waveField);
		upReWaveTimesBuf->setData(waveTimes);
	}
}

void WaveEngine::TransMeshToGPU(){
	if(meshGenerated){
		upRePositionBuf->setData(meshVert);
		meshIndicBuf->setData(meshIndic);
	}
}

bool WaveEngine::ValidWaveCheck(const std::string &waveName, unsigned int *pos){
	for(unsigned int i = 0; i < waveNames.size(); i++){
		if(waveName == waveNames[i]){
			if(pos != 0)
				*pos = i;
			return true;
		}
	}
	return false;
}

void WaveEngine::Render(const HybridCam &camera){
	if(!meshGenerated)
		return;

	vector3f lightPos(50.f, 100.f, 50.f);
	vector3f meshColor(0.27f, 0.72f, 0.6f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	usReWaveN->setSource((int)waveField.size());
	usReTime->setSource((unsigned int)totalTime);
	usReCamPos->setSource(camera.GetEye().toStdVector());
	usReLightPos->setSource(lightPos.toStdVector());
	usReUsedColor->setSource(meshColor.toStdVector());
	if(wireframe){
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glLineWidth(1.0f);
	}
	else
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	upReShader.renderElements(meshIndicBuf, us::PrimitiveType::TRIANGLE_STRIP, meshIndic.size(), 0, false);

	if(showPoints){
		vector3f pointColor(0.6f, 0.1f, 0.1f);
		usReUsedColor->setSource(pointColor.toStdVector());
		glPointSize(3.0f);
		upReShader.render(us::PrimitiveType::POINTS, meshVert.size(), 0, false);
	}

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
}

void WaveEngine::ExportFromCurContext(bool clear){
	if(clear){
		upReShader.disconnectProgram();
		glDeleteBuffers(1, &vertVBO);
		glDeleteBuffers(1, &indicVBO);
		glDeleteVertexArrays(1, &upReVAO);
	}
	printGLError();
}

void WaveEngine::ImportToNewContext(std::string shadersPath){
	auto upReProgram = us::ShaderProgram::create();
	auto upReVertObj = us::ShaderObject::create();
	auto upReFragObj = us::ShaderObject::create();
	upReVertObj->loadFile(shadersPath + "wave.vert");
	upReFragObj->loadFile(shadersPath + "wave.frag");
	upReProgram->addShaderObject(upReVertObj);
	upReProgram->addShaderObject(upReFragObj);

	upRePosition = upReProgram->getInput()->addAttribute("Position");
	upReWaveField = upReProgram->getInput()->addUniform("WaveField");
	upReWaveTimes = upReProgram->getInput()->addUniform("WaveTimes");
	usReWaveN = upReProgram->getInput()->addUniform("WaveN");
	usReTime = upReProgram->getInput()->addUniform("Time");
	usReCamPos = upReProgram->getInput()->addUniform("CamPos");
	usReLightPos = upReProgram->getInput()->addUniform("LightPos");
	usReUsedColor = upReProgram->getInput()->addUniform("UsedColor");

	upReShader.connectProgram(upReProgram);
	shadersSupported = true;//upReProgram->ensureLink();
	if(!shadersSupported)
		return;	

	upRePositionBuf = us::Buffer<vector3f>::create();
	upReWaveFieldBuf = us::Buffer<WavePar>::create();
	upReWaveTimesBuf = us::Buffer<unsigned int>::create();
	meshIndicBuf = us::Buffer<unsigned int>::create();

	{
		upRePosition->connectBuffer(upRePositionBuf);
		upRePosition->setReadingMode(us::Attribute::ReadingMode::FLOAT);

		auto upReWaveFieldTBuf = us::TextureBuffer::create();
		upReWaveFieldTBuf->connectBuffer(upReWaveFieldBuf, 1, us::TextureBuffer::DataType::FLOAT);
		upReWaveField->setSource(upReWaveFieldTBuf);

		auto upReWaveTimesTBuf = us::TextureBuffer::create();
		upReWaveTimesTBuf->connectBuffer(upReWaveTimesBuf, 1, us::TextureBuffer::DataType::UNSIGNED_INT);
		upReWaveTimes->setSource(upReWaveTimesTBuf);
	}

	TransVectorToGPU();
	TransMeshToGPU();
}