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

#include "settingsUI.h"
#include "waveEngine.h"

void SettingsUI::Init(){
	window.create(sf::VideoMode(500, 300, 32), "Settings", sf::Style::Titlebar);
	activeMenu = IDM::MAIN;
	CreateMenu();
}

void SettingsUI::Update(){
	sf::Event event;
    while (window.pollEvent(event)){
        if(event.type == sf::Event::Closed){
			window.close();
			return;
		}
		else if(event.type == sf::Event::Resized){
			window.setActive();
			window.setView(sf::View(sf::FloatRect(0,0,(float)event.size.width, (float)event.size.height)));
			window.setActive(false);
		}
		if(event.type == sf::Event::KeyPressed){
			if(event.key.code == sf::Keyboard::Return){
				ProcessInput();
				input.clear();
			}
		}
		if(event.type == sf::Event::TextEntered){
			if (event.text.unicode == 8 ){
				if(input.getSize() > 0)
					input.erase(input.getSize()-1);
			}
			else if( event.text.unicode != 10 && event.text.unicode != 13)
				input += event.text.unicode;
		}
	}
}

void SettingsUI::Close(){
	window.close();
	menuText.clear();
	waveEngine = 0;
}

void SettingsUI::Render(const sf::Font &font){
	window.setActive();
	window.clear(sf::Color(0,0,0));
	sf::Text text("",font);
	text.setCharacterSize(20);
	text.setColor(sf::Color(30, 255, 30));
	unsigned int i;
	for(i = 0; i < menuSize; i++){
		text.setString(menuText[i]);
		text.setPosition(10.f, i*25.f+10.f);
		window.draw(text);
	}
	text.setString("Input: " + input);
	text.setPosition(10.f, i*25.f+30.f);
	window.draw(text);

	window.display();
	window.setActive(false);
}

void SettingsUI::ProcessInput(){
	std::istringstream iss;
	iss.str(input.toAnsiString());
	int numInput = -1; float fInput = 0;
	switch(activeMenu){
	case IDM::MAIN:
		iss >> numInput;
		switch(numInput){
		case 1:
			activeMenu = IDM::WMESH; break;
		case 2:
			activeMenu = IDM::WSEL; break;
		}
		break;

	case IDM::WMESH:
		iss >> numInput;
		switch(numInput){
		case 0:
			activeMenu = IDM::MAIN; break;
		case 1:
			activeMenu = IDM::SET; activeVal = VAL::WIDTH; break;
		case 2:
			activeMenu = IDM::SET; activeVal = VAL::HEIGHT; break;
		case 3:
			activeMenu = IDM::SET; activeVal = VAL::SPACE; break;
		}
		break;

	case IDM::WSEL:
		iss >> numInput;
		switch(numInput){
		case 0:
			activeMenu = IDM::MAIN; break;
		case 1:
			activeMenu = IDM::WNEW; break;
		case 2:
			activeMenu = IDM::SET; activeVal = VAL::ACTIVE; break;
		case 3:
			waveEngine->ResetTimer(); break;
		}
		break;

	case IDM::WPAR:
		iss >> numInput;
		switch(numInput){
		case 0:
			activeMenu = IDM::WSEL; break;
		case 1:
			activeMenu = IDM::SET; activeVal = VAL::FREQ; break;
		case 2:
			activeMenu = IDM::SET; activeVal = VAL::AMP; break;
		case 3:
			activeMenu = IDM::SET; activeVal = VAL::VELO; break;
		case 4:
			activeMenu = IDM::SET; activeVal = VAL::CENT; break;
		case 5:{
			unsigned int time;
			if(waveEngine->GetWaveTime(selectedWave, time)){
				time = waveEngine->GetTotalTime();
				waveEngine->SetWaveTime(selectedWave, time);
			}
			break;}
		case 6:
			activeMenu = IDM::WDEL; break;
		}
		break;

	case IDM::WNEW:{
		waveEngine->AddWave(input);
		selectedWave = input;
		activeMenu = IDM::WPAR;
		break;}

	case IDM::WDEL:
		iss >> numInput;
		switch(numInput){
		case 0:
			activeMenu = IDM::WPAR; break;
		case 1:
			waveEngine->EraseWave(selectedWave);
			activeMenu = IDM::WSEL; break;
		}
		break;

	case IDM::SET:{
		WavePar param;
		WaveMesh mesh;
		switch(activeVal){
		case VAL::ACTIVE:
			selectedWave = input;
			activeMenu = IDM::WPAR; activeVal = VAL::N; break;
		case VAL::WIDTH:
			iss >> numInput;
			mesh = waveEngine->GetMesh();	mesh.width = numInput; waveEngine->SetMesh(mesh); waveEngine->GenMesh();
			activeMenu = IDM::WMESH; activeVal = VAL::N; break;
		case VAL::HEIGHT:
			iss >> numInput;
			mesh = waveEngine->GetMesh();	mesh.height = numInput; waveEngine->SetMesh(mesh); waveEngine->GenMesh();
			activeMenu = IDM::WMESH; activeVal = VAL::N; break;
		case VAL::SPACE:
			iss >> fInput;
			mesh = waveEngine->GetMesh();	mesh.spacing = fInput; waveEngine->SetMesh(mesh); waveEngine->GenMesh();
			activeMenu = IDM::WMESH; activeVal = VAL::N; break;
		case VAL::FREQ:
			iss >> fInput;
			if(waveEngine->GetWave(selectedWave, param)){
				param.freq = fInput; waveEngine->SetWave(selectedWave, param);
			}
			activeMenu = IDM::WPAR; activeVal = VAL::N; break;
		case VAL::AMP:
			iss >> fInput;
			if(waveEngine->GetWave(selectedWave, param)){
				param.amp = fInput; waveEngine->SetWave(selectedWave, param);
			}
			activeMenu = IDM::WPAR; activeVal = VAL::N; break;
		case VAL::VELO:
			iss >> fInput;
			if(waveEngine->GetWave(selectedWave, param)){
				param.velo = fInput; waveEngine->SetWave(selectedWave, param);
			}
			activeMenu = IDM::WPAR; activeVal = VAL::N; break;
		case VAL::CENT:{
			float fInput2 = 0; iss >> fInput >> fInput2;
			if(waveEngine->GetWave(selectedWave, param)){
				param.center = vector3f(fInput,0,fInput2); waveEngine->SetWave(selectedWave, param);
			}
			activeMenu = IDM::WPAR; activeVal = VAL::N; break;}
		}
		break;}
	}
	CreateMenu();
}

void SettingsUI::CreateMenu(){
	std::wostringstream woss;
	switch(activeMenu){
		case IDM::MAIN:
			menuSize = 2;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"1. Mesh parameters";
			menuText[1] = L"2. Wave parameters";
			break;

		case IDM::WMESH:
			menuSize = 4;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"0. Back";
			menuText[1] = L"1. Width";
			menuText[2] = L"2. Height";
			menuText[3] = L"3. Spacing";
			break;

		case IDM::WSEL:
			menuSize = 4;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"0. Back";
			menuText[1] = L"1. Create new wave";
			menuText[2] = L"2. Select wave by name";
			menuText[3] = L"3. Reset all timers";
			break;

		case IDM::WPAR:
			menuSize = 7;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"0. Back";
			menuText[1] = L"1. Frequency";
			menuText[2] = L"2. Amplitude";
			menuText[3] = L"3. Spread velocity";
			menuText[4] = L"4. Epicentre";
			menuText[5] = L"5. Reset timer";
			menuText[6] = L"6. Delete this wave";
			break;

		case IDM::WNEW:
			menuSize = 1;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"Enter name for new wave";
			break;

		case IDM::WDEL:
			menuSize = 3;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"Deleting wave: "; menuText[0] += selectedWave.data();
			menuText[1] = L"0. Back";
			menuText[2] = L"1. Delete";
			break;

		case IDM::SET:{
			WavePar param;
			WaveMesh mesh;
			menuSize = 1;
			menuText.clear(); menuText.resize(menuSize);
			menuText[0] = L"Current value: ";

			switch(activeVal){
			case VAL::ACTIVE:
				menuText[0] += selectedWave; 
				break;
			case VAL::WIDTH:
				mesh = waveEngine->GetMesh();	woss << mesh.width;
				menuText[0] += woss.str(); 
				break;
			case VAL::HEIGHT:
				mesh = waveEngine->GetMesh();	woss << mesh.height;
				menuText[0] += woss.str(); 
				break;
			case VAL::SPACE:
				mesh = waveEngine->GetMesh();	woss << mesh.spacing;
				menuText[0] += woss.str(); 
				break;
			case VAL::FREQ:
				if(waveEngine->GetWave(selectedWave, param))
					woss << param.freq;
				else
					woss << "invalid wave selected";
				menuText[0] += woss.str(); 
				break;
			case VAL::AMP:
				if(waveEngine->GetWave(selectedWave, param))
					woss << param.amp;
				else
					woss << "invalid wave selected";
				menuText[0] += woss.str(); 
				break;
			case VAL::VELO:
				if(waveEngine->GetWave(selectedWave, param))
					woss << param.velo;
				else
					woss << "invalid wave selected";
				menuText[0] += woss.str(); 
				break;
			case VAL::CENT:
				if(waveEngine->GetWave(selectedWave, param)){
					woss << param.center.x << " " << param.center.z;
					menuText[0] += woss.str();
				}
				else
					woss << "invalid wave selected";
				break;
			}
			break;}

		default:
			activeMenu = IDM::MAIN;
			break;
	}
}