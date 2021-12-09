#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <windows.h> 

#include "commonData.h"
#include "Player.h"
#include "Enemy.h"
#include "GUIElement.h"

using namespace std;

const chrono::milliseconds tick(25);
int difficulty;

const int nScreenWidth = 80;
const int nScreenHeight = 40;

int nFieldWidth = 71;
int nFieldHeight = 31;

int nStatFieldWidth = nFieldWidth;
int nStatFieldHeight = 4;

int nSideBarFieldWidth = nScreenWidth - nFieldWidth;
int nSideBarFieldHeight = nScreenHeight;

int nChatFieldWidth = nFieldWidth;
int nChatFieldHeight = 5;

int nGameMapWidth = 200;
int nGameMapHeight = 200;

const int textSize = 13;

unsigned char* pField = nullptr; //Play field
unsigned char* statField = nullptr; //Player stats field
unsigned char* sideBarField = nullptr; //Side bar field
unsigned char* chatField = nullptr; //Chat field
unsigned char* map = nullptr; //The play map

vector<Enemy> enemies; //All enemies in-game

coords GUIPos = { 0,0 };
coords GUISize = { nScreenWidth,nScreenHeight };
GUIElement GUIs[2] = { GUIElement("MainMenu", true, GUIPos, GUISize), GUIElement("PauseMenu", false, GUIPos, GUISize) };//All GUI Elements that may be used

coords validMove(coords pPos, coords move, bool tp) { //Only checks valid moves on the map.

	//Check that the player is moving within map bounds and not onto an X
	//If the player is teleporting

	

	if (tp) {
		if (move.x >= 0 && move.x < nGameMapWidth && move.y >= 0 && move.y < nGameMapHeight) {
			if (map[move.y * nGameMapWidth + move.x] == 0) {
				return move;
			}
			else {
				move.x = (map[move.y * nGameMapWidth + pPos.x] == 0) ? pPos.x : move.x;
				move.y = (map[pPos.y * nGameMapWidth + move.x] == 0) ? pPos.y : move.y;
				return move;
			}
		}
		else {
			move.x = (move.x >= 0 && move.x < nGameMapWidth) ? move.x : ((move.x < 0) ? 0 : nGameMapWidth);
			move.y = (move.y >= 0 && move.y < nGameMapHeight) ? move.y : ((move.y < 0) ? 0 : nGameMapHeight);

			if (map[move.y * nGameMapWidth + move.x] == 0) {
				return move;
			}
			else {
				do {
					move.x += 1;
					move.y += 1;
				} while (map[move.y * nGameMapWidth + move.x] != 0);
			}
			return move;
		}
	}
	else {

		coords newPos = { pPos.x + move.x, pPos.y + move.y }; //Get the coordinates the player wants to move to

		if (newPos.x < 0 || newPos.x >= nGameMapWidth || newPos.y < 0 || newPos.y >= nGameMapHeight) { // Check for which way the player going out of bounds
			if (newPos.x < 0 || newPos.x >= nGameMapWidth) {
				move.x = 0;
			}
			if (newPos.y < 0 || newPos.y >= nGameMapHeight) {
				move.y = 0;
			}
		} //Should set the player's new position on the fringe of the map

		newPos = { pPos.x + move.x, pPos.y + move.y };

		if (map[newPos.y * nGameMapWidth + newPos.x] != 0) { //If we've hit a non-empty space

			//If there's an obstacle on the x axis, mult by 0. If on y, mult by 0

			move = { move.x * ((map[pPos.y * nGameMapWidth + newPos.x] == 0) ? 1 : 0),move.y * ((map[newPos.y * nGameMapWidth + pPos.x] == 0) ? 1 : 0) };

		}

		newPos = { pPos.x + move.x, pPos.y + move.y };

		return newPos;
	}
}

int main()
{
	srand(time(NULL));

	//Create the player

	Player plr("Steve");

	// Create Screen Buffer

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Font things

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof cfi;
	cfi.nFont = 0;
	cfi.dwFontSize.X = textSize;
	cfi.dwFontSize.Y = textSize;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Courier New");

	SetCurrentConsoleFontEx(hConsole,0, &cfi);

	// Create the different fields

	pField = new unsigned char[nFieldWidth * nFieldHeight];
	statField = new unsigned char[nStatFieldWidth * nStatFieldHeight];
	sideBarField = new unsigned char[nSideBarFieldWidth * nSideBarFieldHeight];
	chatField = new unsigned char[nChatFieldWidth * nChatFieldHeight];
	map = new unsigned char[nGameMapWidth * nGameMapHeight];

	/*for (int i = 0; i < nStatFieldWidth * nStatFieldHeight; i++) statField[i] = L' ';
	for (int i = 0; i < nSideBarFieldWidth * nSideBarFieldHeight; i++) sideBarField[i] = L' ';
	for (int i = 0; i < nChatFieldWidth * nChatFieldHeight; i++) chatField[i] = L' ';*/
	for (int i = 0; i < nGameMapWidth * nGameMapHeight; i++) map[i] = 0;

	//Map construction============

	ifstream mfin("MapFeatures.csv", ios::in);

	int objX, objY, objS;
	string input, sX, sY, sS;

	//Read from map features file. I literally set up my own "file type"

	while (!mfin.eof()) { //Until the end of the file has been reached.
		getline(mfin, input, ',');
		getline(mfin, sX, ','); //Position.x
		getline(mfin, sY, ','); //Position.y
		getline(mfin, sS, '\n'); //Size (in lines) of the following object

		//Assign integer values

		objX = stoi(sX);
		objY = stoi(sY);
		objS = stoi(sS);

		for (int i = 0; i < objS; i++) { //For every line of the object
			getline(mfin, input, '\n'); //Read the full line
			for (int j = 0; j < input.length(); j++) { //For every character in the line
				map[(objY + i) * nGameMapWidth + objX + j] = (input[j] == '.') ? 0 : 1; //Add it to the map at (ObjectPosition.x + #line_we're_on, ObjectPosition.y + #character_we've_encountered)
			}
		}
	}

	mfin.close(); // Close the file

	//Main Menu creation===========

	GUIElement& mainMenu = GUIs[0];

	GUIElement& PauseMenu = GUIs[1];

	//Variable creation

	string n;
	bool lost = false; //Checks if the player is alive or dead
	bool pause = false; //Checks if the player has paused the game
	bool menu = true;
	bool bKey[5]; //All keys we need to check for constant input stored true if held

	bool type[39]; //All keys we need to check for being pressed. Stored true if held
	bool keyHeld[39]; //All keys being held down

	coords move{ 0,0 }; //Coordinates for player movement
	coords ePos{ 0,0 };
	coords plrPos{ 12,12 }; //The player's coordinates

	INPUT_RECORD ir[128];
	DWORD nRead;

	//TEMPORARY VALUES

	cooldown enemySpawn{0};
	


	while (!lost) { //While the game is still "valid"

		//GAME TIMING======================

		this_thread::sleep_for(tick); //Frame creation every 25 ms

		//INPUT============================

		//Keyboard input====

		//For any key we need to consistently check is being pressed

		for (int k = 0; k < 5; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)(L"WASD\x10"[k]))) != 0; // 'w' 's' 'a' 'd' shift 

		//For any key we need to check is pressed once.

		for (int k = 0; k < 39; k++) {
			keyHeld[k] = type[k]; //Check before so we get input at least once
			type[k] = (0x8000 & GetAsyncKeyState((unsigned char)(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\x20\x0d\x1b"[k]))) != 0; //alphabet numbers space enter escape
		}

		//GAME LOGIC=======================

		plrPos = plr.getPos(); //Re-check position each frame
		int plrX = plrPos.x; //Get integer shorthands for EOA
		int plrY = plrPos.y;

		if (!menu) {
			if (type[38] && !keyHeld[38]) pause = !pause;

			if (pause && !keyHeld[38]) {
				PauseMenu.toggleVisibility(true);
			}
			else if (!pause && !keyHeld[38]) {
				PauseMenu.toggleVisibility(false);
			}

			//The real game

			if (!pause && !keyHeld[38]) {

				plr.tick();

				//During play====

				//Move the player

				move = { ((bKey[3]) ? 1 : 0) - ((bKey[1]) ? 1 : 0),((bKey[2]) ? 1 : 0) - ((bKey[0]) ? 1 : 0) };

				if (plr.moveCool() <= 0.0) {
					plr.move(validMove(plr.getPos(), move, false));
				}

				if (type[36] && plr.attackCool() <= 0.0) {

				}

				//TEMPORARY Enemy spawn every few seconds within 10 blocks of the player

				if (enemySpawn.seconds <= 0) {
					coords newL{ 0,0 };
					newL = { plrX - 10 + (rand() % 20), plrY - 10 + (rand() % 20) };
					enemies.push_back(Enemy(validMove(plrPos, newL, true)));
					enemySpawn.seconds = 5.0;
				}
				else {
					enemySpawn.seconds -= .05;
				}
			}
			else { //If we're in the pause menu
			if (type[4]) {
				menu = true;
				pause = false;
				PauseMenu.toggleVisibility();
				mainMenu.toggleVisibility(true);
				continue;
			}
			}
		}
		else {
		if (type[37]) {
			menu = false;
			mainMenu.toggleVisibility(false);
			continue;
		}
		else if (type[38]) {
			break;
		}
		}

		//Check player interaction input

		//During pause====

		//RENDER OUTPUT====================

		//Draw stat field

			// Stuff goes here

		//Draw side bar field

			// Stuff goes here

		//Draw chat field

			// Stuff goes here

		//Draw play field

				//The play field will render the map in respect to the player's position.
				//The map, stored in '.', 'X', etc. will be directly drawn from

		for (int y = 0; y < nFieldHeight; y++) { //For each line in the playScreen
			for (int x = 0; x < nFieldWidth; x++) { //For each "pixel" in that line

				//If the selected pixel is outside the map;
				//Dependent on the player's position. The player, being the center of the screen, will be the reference
				//We check to see if the pixel of the map the screen wants to render is valid.
				//The formula for this is ( plr.x - half_screenWidth + index of how many pixels we are in ( 0 to screen width) , plr.y - half_screenHeight + index of how many pixels down we are (0 to screen height) )
				//The player's XY is their location on the map

				if (plrY - (nFieldHeight / 2) + y < 0 || plrY - (nFieldHeight / 2) + y >= nGameMapHeight || plrX - (nFieldWidth / 2) + x < 0 || plrX - (nFieldWidth / 2) + x >= nGameMapWidth) {
					screen[y * nScreenWidth + x] = '-';
					continue;
				}

				//If it's inside the map, then we need to draw out of the map what type of pixel it is.
				//First, we get the index of the pixel on the screen we want to draw (same formula as before).
				//Then, we'll find which map pixel we want to draw, the player being the reference point. (follows a similar formula)
				//The formula will be ( plr.x - half_screenWidth (beginning reference point) + index of how many pixels we are in, plr.y - half_screenHeight (beginning reference point) + index of how many pixels we are down )

				screen[(y /* + nStatFieldHeight */)*nScreenWidth + x] = L" #"[map[(plrY - (nFieldHeight / 2) + y) * nGameMapHeight + plrX - (nFieldWidth / 2) + x]]; //Warning that map isn't initialized, but it's fine.
			}
		}

		//Draw the enemies

		int eX,eY = 0;

		for (int i = 0; i < enemies.size(); i++) {
			Enemy& cE = enemies[i];
			ePos = cE.getPos(); //Re-check position each frame
			eX = ePos.x; //Get integer shorthands for EOA
			eY = ePos.y;
			if (eX - plrX + (nFieldWidth / 2) >= 0 || eX - plrX + (nFieldWidth / 2) <= 0 || eY - plrY + (nFieldHeight / 2)>= 0 || eY - plrY + (nFieldHeight / 2) <= 0) {
				screen[(eY - plrY + nFieldHeight/2) * nScreenWidth + eX - plrX + nFieldWidth/2] = 'X';
			}
		}

		//Draw the player, right in the middle of the field.

		screen[(nFieldHeight / 2) * nScreenWidth + (nFieldWidth / 2)] = 'O';

		//Render GUIElements last

		for (int i = 0; i < 2; i++) { //For each GUI element
			GUIElement& cGUI = GUIs[i];
			if (GUIs[i].getVisibility()) { //If the GUI element is visible

				//Create a backboard real fast

				for (int a = 0; a < cGUI.getSize().y; a++) {
					for (int b = 0; b < cGUI.getSize().x; b++) {
						screen[(a + cGUI.getPos().y) * nScreenWidth + b + cGUI.getPos().x] = ' ';
					}
				}

				//Render elements on the GUI

				coords pos = cGUI.getPos();
				coords size = cGUI.getSize();

				//Open the GUI Element's object list. Includes text only for now.
				
				fstream fin(cGUI.getFileName() + ".csv", ios::in);

				string input, sX, sY, sS;
				int objX, objY, objS;

				while (!fin.eof()) { //While we still have file to read
					getline(fin, input, ','); //Type
					getline(fin, sX, ','); //PosX
					getline(fin, sY, ','); //PoxY
					getline(fin, sS, '\n'); //Lines

					objX = stoi(sX); //Integer-ize
					objY = stoi(sY);
					objS = stoi(sS);

					for (int i = 0; i < objS; i++) { //For each line in the object
						getline(fin, input, '\n'); //Get the object's display
						for (int j = 0; j < input.length(); j++) { //For each character in the line

							//Similar formula to that before, but renders independently.
							//Adds according to the object's character value.
							if (input[j] != '.') {
								screen[(objY + i + pos.y) * nScreenWidth + objX + j + pos.x] = input[j];
							}
						}
					}
				}

				fin.close(); //Close the file

			}
		}
		
		//Display frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}	

	CloseHandle(hConsole);
	std::cout << "Game Over!!" << endl;
	std::system("pause");
	return 0;
}