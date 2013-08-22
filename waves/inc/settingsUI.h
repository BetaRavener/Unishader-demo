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

#pragma once
#ifndef SETTINGS_UI
#define SETTINGS_UI

#include "includes.h"
#include <iostream>
#include <sstream>
#include "math3d.h"

class WaveEngine;

class SettingsUI{
public:
	void Init();
	void Close();
	void Update();
	void Render(const sf::Font &font);
	void SetWaveEngine(WaveEngine *w) { waveEngine = &(*w); }
private:
	class IDM{
	public:
		enum {MAIN = 0, WMESH, WSEL, WPAR, WNEW, WDEL, SET};
	};
	class VAL{
	public:
		enum {N = 0,WIDTH,HEIGHT,SPACE,FREQ,AMP,VELO,CENT,ACTIVE};
	};

	WaveEngine *waveEngine;
	std::string selectedWave;

	void ProcessInput();
	void CreateMenu();

	unsigned int activeMenu, activeVal;
	std::vector<sf::String> menuText;
	unsigned int menuSize;

	sf::RenderWindow window;
	sf::String input;
};

#endif