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

#include "waveView.h"
#include "waveEngine.h"
#include "settingsUI.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "includes.h"

sf::String waveWinName;
sf::Clock renderClock;
sf::Uint32 maxFPS;
WaveView waveView;
SettingsUI settingsUI;
sf::Vector2u preFullScreenSize;
bool fullscreen = false;
bool updatePaused = false;
std::string title = "Wave Propagation";

bool loadPaths(std::string& dataPath, std::string& shadersPath)
{
	std::ifstream fin = std::ifstream("waves-config");
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

	fin.open(dataPath + "font/LiberationSans-Regular.ttf", std::ios::app);
	allFound = allFound && fin.is_open();
	fin.close();

	fin.open(shadersPath + "wave.vert", std::ios::app);
	allFound = allFound && fin.is_open();
	fin.close();

	fin.open(shadersPath + "wave.frag", std::ios::app);
	allFound = allFound && fin.is_open();
	fin.close();

	if(allFound){
		std::ofstream fout = std::ofstream("waves-config");
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

	sf::Font font = sf::Font();
	if(!font.loadFromFile(dataPath + "font/LiberationSans-Regular.ttf")){
		std::cerr << "Couldn't load font, exiting.";
		return 1;
	}
	waveView.SetSize(500,500);
	maxFPS = 30;
	sf::ContextSettings set; set.majorVersion = 3; set.minorVersion = 2;
	sf::Window waveWin(sf::VideoMode(500, 500), title, sf::Style::Default, set);
	std::cout << "Version: " << (char*)glGetString(GL_VERSION) << std::endl << std::endl;

	waveWin.setActive();
	glewInit();	
	waveView.Init();
	waveView.camera.ZoomR(-50);
	WaveEngine waveEngine(shadersPath);

	if(!waveEngine.AreShadersSupported()){
		std::cout << "Couldn't compile shaders (OpenGL version too low?). Exiting program." << std::endl;
		std::cin.get();
		return 1;
	}

	waveWin.setActive(false);

	settingsUI.Init();
	settingsUI.SetWaveEngine(&waveEngine);

	renderClock.restart();	
	while(waveWin.isOpen()){
		settingsUI.Update();
		sf::Event waveEvent;
		while(waveWin.pollEvent(waveEvent)){
			if(waveEvent.type == sf::Event::Closed){
				waveWin.close();
			}
			else if(waveEvent.type == sf::Event::Resized){
				waveWin.setActive();
				waveView.SetSize(waveEvent.size.width, waveEvent.size.height);
				waveWin.setActive(false);
			}
			else if(waveEvent.type == sf::Event::KeyPressed){
				float step = 1.0f;
				float rotation = Deg2Rad(1.0f);
				if(waveEvent.key.shift){
					step *= 5;
					rotation *= 5;
				}
				switch(waveEvent.key.code){
					case sf::Keyboard::Q:
						waveView.camera.MoveR(vector3f(0,0,step));
						break;
					case sf::Keyboard::E:
						waveView.camera.MoveR(vector3f(0,0,-step));
						break;
					case sf::Keyboard::W:
						waveView.camera.MoveR(vector3f(0,step,0));
						break;
					case sf::Keyboard::S:
						waveView.camera.MoveR(vector3f(0,-step,0));
						break;
					case sf::Keyboard::A:
						waveView.camera.MoveR(vector3f(-step,0,0));
						break;
					case sf::Keyboard::D:
						waveView.camera.MoveR(vector3f(step,0,0));
						break;
					case sf::Keyboard::I:
						waveView.camera.RotateR(vector3f(0,rotation,0));
						break;
					case sf::Keyboard::K:
						waveView.camera.RotateR(vector3f(0,-rotation,0));
						break;
					case sf::Keyboard::J:
						waveView.camera.RotateR(vector3f(-rotation,0,0));
						break;
					case sf::Keyboard::L:
						waveView.camera.RotateR(vector3f(rotation,0,0));
						break;
					case sf::Keyboard::X:
						waveView.camera.RotateA(vector3f(Deg2Rad(90),0,0));
						break;
					case sf::Keyboard::Y:
						waveView.camera.RotateA(vector3f(0,Deg2Rad(90),0));
						break;
					case sf::Keyboard::Z:
						waveView.camera.RotateA(vector3f(0,0,0));
						break;
					case sf::Keyboard::V:
						waveView.camera.ZoomR(step);
						break;
					case sf::Keyboard::B:
						waveView.camera.ZoomR(-step);
						break;
					case sf::Keyboard::Num1:
						waveEngine.SwShowPoints();
						break;
					case sf::Keyboard::Num2:
						waveEngine.SwWireframe();
						break;
					case sf::Keyboard::F10:{
						std::ofstream fout("glInfo.txt");
						fout << "Version: " << (char*)glGetString(GL_VERSION) << std::endl;
						fout << "Extensions: " << std::endl << (char*)glGetString(GL_EXTENSIONS) << std::endl;
						break;
						}
					case sf::Keyboard::Space:
						updatePaused = !updatePaused;
						break;
					case sf::Keyboard::Return:
						if(waveEvent.key.control == true){
							fullscreen = !fullscreen;
							waveEngine.ExportFromCurContext(true);
							waveWin.close();
							if(fullscreen){
								preFullScreenSize = waveView.GetSize();
								waveView.SetSize(	sf::VideoMode::getFullscreenModes().front().width,
													sf::VideoMode::getFullscreenModes().front().height);
								waveWin.create(sf::VideoMode(waveView.GetWidth(), waveView.GetHeight()), title, sf::Style::Fullscreen);
							}
							else{
								waveView.SetSize(preFullScreenSize);
								waveWin.create(sf::VideoMode(waveView.GetWidth(), waveView.GetHeight()), title);
							}
							waveEngine.ImportToNewContext(shadersPath);
						}
						break;
				}
			}
		}

		settingsUI.Render(font);
		if(renderClock.getElapsedTime() > sf::seconds(1.0f/maxFPS)){
			if(!updatePaused)
				waveEngine.AddTime(renderClock.getElapsedTime().asMilliseconds());
			renderClock.restart();
			waveWin.setActive();

			//Clear color and depth buffer
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearColor(1.0f,1.0f,1.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);

			//Activate view and render simulation
			waveView.Activate();
			waveEngine.Render(waveView.camera);
			waveView.camera.DrawCrosshair();
			waveView.Deactivate();

			waveWin.display();
			waveWin.setActive(false);
		}
	}

	//finish cleanup
	settingsUI.Close();
	waveView.camera.Clear();
	return 0;
}