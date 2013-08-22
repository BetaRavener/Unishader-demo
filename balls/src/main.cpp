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

#include "ballsView.h"
#include "ballsEngine.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "math3d.h"
#include "includes.h"

sf::String ballsWinName;
sf::Clock renderClock, updateClock;
sf::Uint32 maxFPS, maxUPS;
float maxUpdateTime;
float speedFactor = 1.0f;
int curLoad = 0;
BallsView ballsView;
sf::Vector2u preFullScreenSize;
bool fullscreen = false;
bool updatePaused = true;
bool CPUmode = false;
bool showUPS = false;
std::string title = "Bouncing Balls";

bool loadPaths(std::string& dataPath, std::string& shadersPath)
{
	std::ifstream fin = std::ifstream("balls-config");
	bool loaded = false;

	if(fin.is_open()){
		fin >> dataPath;

		if(!fin.eof()){
			fin >> shadersPath;
			loaded = true;
		}
	}
	fin.close();
	
	if(!loaded){
		std::cout << "Enter paths terminated with folder separator\n";
		std::cout << "Enter path to data folder containing scene setup files:\n";
		std::cin >> dataPath;
		std::cout << "Enter path to shaders folder containing shader files:\n";
		std::cin >> shadersPath;
	}

	bool allFound = true;

	fin.open(shadersPath + "ballsUpdate.vert");
	allFound = allFound && fin.is_open();
	fin.close();

	fin.open(shadersPath + "ballsRender.vert");
	allFound = allFound && fin.is_open();
	fin.close();

	fin.open(shadersPath + "ballsRender.geom");
	allFound = allFound && fin.is_open();
	fin.close();

	fin.open(shadersPath + "ballsRender.frag");
	allFound = allFound && fin.is_open();
	fin.close();

	if(allFound){
		std::ofstream fout = std::ofstream("balls-config");
		if(fout.is_open()){
			fout << dataPath << std::endl;
			fout << shadersPath << std::endl;
		}
		fout.close();
	}
	
	return allFound;
}

int main(){
	std::string dataPath;
	std::string shadersPath;
	if(!loadPaths(dataPath, shadersPath))
		return 1;

	sf::ContextSettings set; set.majorVersion = 3; set.minorVersion = 3;
	ballsView.SetSize(500,500);
	maxFPS = 30;
	maxUPS = 1000;
	maxUpdateTime = 40.f;
	sf::Window ballsWin(sf::VideoMode(ballsView.GetWidth(), ballsView.GetHeight()), title, sf::Style::Default, set);
	std::cout << "Version: " << (char*)glGetString(GL_VERSION) << std::endl << std::endl;
	glewInit();
	ballsView.Init();

	BallsEngine ballsEngine;
	try{
		ballsEngine.loadShaders(shadersPath);
	}
	catch(...){
		std::cerr << "Error in engine initialization, exiting" << std::endl;
		return 1;
	}

	renderClock.restart();
	updateClock.restart();

	while(ballsWin.isOpen()){
		sf::Event ballsEvent;
		while(ballsWin.pollEvent(ballsEvent)){
			if(ballsEvent.type == sf::Event::Closed){
				ballsWin.close();
				return 0;
			}
			else if(ballsEvent.type == sf::Event::Resized){
				ballsView.SetSize(ballsEvent.size.width, ballsEvent.size.height);
			}
			else if(ballsEvent.type == sf::Event::KeyPressed){
				float step = 0.2f;
				float rotation = Deg2Rad(1.0f);
				if(ballsEvent.key.shift){
					step *= 20;
					rotation *= 5;
				}
				switch(ballsEvent.key.code){
					case sf::Keyboard::Q:
						ballsView.camera.MoveR(vector3f(0,0,step));
						break;
					case sf::Keyboard::E:
						ballsView.camera.MoveR(vector3f(0,0,-step));
						break;
					case sf::Keyboard::W:
						ballsView.camera.MoveR(vector3f(0,step,0));
						break;
					case sf::Keyboard::S:
						ballsView.camera.MoveR(vector3f(0,-step,0));
						break;
					case sf::Keyboard::A:
						ballsView.camera.MoveR(vector3f(-step,0,0));
						break;
					case sf::Keyboard::D:
						ballsView.camera.MoveR(vector3f(step,0,0));
						break;
					case sf::Keyboard::I:
						ballsView.camera.RotateR(vector3f(0,rotation,0));
						break;
					case sf::Keyboard::K:
						ballsView.camera.RotateR(vector3f(0,-rotation,0));
						break;
					case sf::Keyboard::J:
						ballsView.camera.RotateR(vector3f(-rotation,0,0));
						break;
					case sf::Keyboard::L:
						ballsView.camera.RotateR(vector3f(rotation,0,0));
						break;
					case sf::Keyboard::H:
						ballsView.camera.Reset();
						break;
					case sf::Keyboard::X:
						ballsView.camera.RotateA(vector3f(Deg2Rad(90),0,0));
						break;
					case sf::Keyboard::Y:
						ballsView.camera.RotateA(vector3f(0,Deg2Rad(90),0));
						break;
					case sf::Keyboard::Z:
						ballsView.camera.RotateA(vector3f(0,0,0));
						break;
					case sf::Keyboard::V:
						ballsView.camera.ZoomR(step);
						break;
					case sf::Keyboard::B:
						ballsView.camera.ZoomR(-step);
						break;
					case sf::Keyboard::R:
						if(curLoad > 0){
							switch(curLoad){
							case 1:
								ballsEngine.LoadSetupFromFile(dataPath + "setup1.txt",true);
								break;
							case 2:
								ballsEngine.LoadSetupFromFile(dataPath + "setup2.txt",true);
								break;
							case 3:
								ballsEngine.LoadSetupFromFile(dataPath + "setup3.txt",true);
								break;
							}
							updatePaused = true;
						}
						break;
					case sf::Keyboard::Num1:
						ballsEngine.LoadSetupFromFile(dataPath + "setup1.txt");
						curLoad = 1;
						updatePaused = true;
						break;
					case sf::Keyboard::Num2:
						ballsEngine.LoadSetupFromFile(dataPath + "setup2.txt");
						curLoad = 2;
						updatePaused = true;
						break;
					case sf::Keyboard::Num3:
						ballsEngine.LoadSetupFromFile(dataPath + "setup3.txt");
						curLoad = 3;
						updatePaused = true;
						break;
					case sf::Keyboard::Add:
						if(speedFactor < 1)
							speedFactor*=2;
						break;
					case sf::Keyboard::Subtract:
						speedFactor/=2;
						break;
					case sf::Keyboard::F10:{
						std::ofstream fout("glInfo.txt");
						fout << "Version: " << (char*)glGetString(GL_VERSION) << std::endl;
						fout << "Extensions: " << std::endl << (char*)glGetString(GL_EXTENSIONS) << std::endl;
						break;
						}
					case sf::Keyboard::Space:
						updatePaused = !updatePaused;
						if(!updatePaused)
							updateClock.restart();
						break;
					case sf::Keyboard::Return:
						showUPS = !showUPS;
						break;
				}
			}
		}
		if(updateClock.getElapsedTime().asMilliseconds() > 1000.0f/maxUPS && !updatePaused){
			float time = myMath::mod((float)(updateClock.getElapsedTime().asMilliseconds()*speedFactor),maxUpdateTime);
			if(showUPS)
				std::cout << std::fixed << std::setprecision(2) << (1000.0f/updateClock.getElapsedTime().asMilliseconds()) << "|";
			updateClock.restart();
			ballsEngine.Update(time);
		}
		if(renderClock.getElapsedTime().asMilliseconds() > 1000.0f/maxFPS){
			renderClock.restart();
			ballsWin.setActive();

			//Clear color and depth buffer
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(1.0f,1.0f,1.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			//Activate view and render simulation
			ballsView.Activate();
			ballsEngine.Render(ballsView.camera);
			ballsView.camera.DrawCrosshair();
			ballsView.Deactivate();

			ballsWin.display();
		}
		sf::sleep(sf::seconds(1.0f/maxUPS/2));
	}
	return 0;
}