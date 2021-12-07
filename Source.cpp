#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <windows.h> 

//#include <thread>
//#include <chrono>

#include "commonData.h"
#include "Player.h"
#include "chatLog.h"
#include "Enemy.h"
#include "Object.h"
#include "Item.h"
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

unsigned char* pField = nullptr; //Play field
unsigned char* statField = nullptr; //Player stats field
unsigned char* sideBarField = nullptr; //Side bar field
unsigned char* chatField = nullptr; //Chat field
unsigned char* map = nullptr; //The play map

vector<Enemy> enemies; //All enemies in-game
vector<GUIElement> GUIs; //All GUI Elements that may be used

bool validMove(coords pPos, coords move, bool tp) { //Only checks valid moves on the map.

	//Check that the player is moving within map bounds and not onto an X
	//If the player is teleporting

	if (tp) {
		if (map[move.y * nGameMapWidth + move.x] != '.' || move.x < 0 || move.x >= nGameMapWidth || move.y < 0 || move.y >= nGameMapHeight) {
			return false;
		}
	}
	else {
		coords newPos = { pPos.x + move.x, pPos.y + move.y };

		//If we aren't moving onto an empty space, or out of the map

		if (map[newPos.y * nGameMapWidth + newPos.x] == 1 || newPos.x < 0 || newPos.x >= nGameMapWidth || newPos.y < 0 || newPos.y >= nGameMapHeight) {
			return false;
		}
	}
	
	return true;
}

int main()
{

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
	cfi.dwFontSize.X = 13;
	cfi.dwFontSize.Y = 13;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Courier New");

	SetCurrentConsoleFontEx(hConsole,0, &cfi);

	//Get mouse

	SetConsoleMode(hConsole, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

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

	//Map construction

	ifstream fin("MapFeatures.csv", ios::in);

	int objX, objY, objS;
	string input, sX, sY, sS;

	//Read from map features file. I literally set up my own "file type"

	while (!fin.eof()) { //Until the end of the file has been reached.
		getline(fin, input, ',');
		getline(fin, sX, ','); //Position.x
		getline(fin, sY, ','); //Position.y
		getline(fin, sS, '\n'); //Size (in lines) of the following object

		//Assign integer values

		objX = stoi(sX);
		objY = stoi(sY);
		objS = stoi(sS);

		for (int i = 0; i < objS; i++) { //For every line of the object
			getline(fin, input, '\n'); //Read the full line
			for (int j = 0; j < input.length(); j++) { //For every character in the line
				map[(objY + i) * nGameMapWidth + objX + j] = (input[j] == '.') ? 0 : 1; //Add it to the map at (ObjectPosition.x + #line_we're_on, ObjectPosition.y + #character_we've_encountered)
			}
		}
	}

	fin.close(); // Close the file

	//Menu creation

	coords GUIPos{ 0,0 };
	coords GUISize{ nScreenWidth,nScreenHeight };

	GUIs.push_back(GUIElement("MainMenu", true, GUIPos, GUISize)); //Add a GUI Element to the vector.

	GUIElement &mainMenu = GUIs[0]; //Set a shorthand for the mainMenu;

	//Variable creation

	string n;
	bool lost = false; //Checks if the player is alive or dead
	bool pause = false; //Checks if the player has paused the game
	bool bKey[5]; //All keys we need to check for constant input stored true if held
	bool bMouse[2]; //Left click and Right click
	bool type[36]; //All keys we need to check for being pressed. Stored true if pressed

	coords move{ 0,0 }; //Coordinates for player movement
	coords plrPos{ 12,12 }; //The player's coordinates
	coords mouse{ 0,0 }; //The mouse's position

	INPUT_RECORD ir[128];
	DWORD nRead;

	//TEMPORARY VALUES

	cooldown enemySpawn{0};
	
	while (!lost) { //While the game is still "valid"

		//GAME TIMING======================

		this_thread::sleep_for(tick); //Frame creation every 25 ms

		//INPUT============================

		//Keyboard input====

		for (int k = 0; k < 5; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)(L"WASD\x10"[k]))) != 0; // 'w' 's' 'a' 'd' shift 

		for (int k = 0; k < 36; k++)
			type[k] = (0x0001 & GetAsyncKeyState((unsigned char)(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\x20\x0d"[k]))) != 0; //alphabet numbers space enter

		//Mouse input====

		ReadConsoleInput(hConsole, ir, 128, &nRead);
		for (int i = 0; i < nRead; i++) {
			if (ir[i].EventType == MOUSE_EVENT) {
				bMouse[0] = (ir[i].Event.MouseEvent.dwButtonState & 0x01); // LMB
				bMouse[1] = (ir[i].Event.MouseEvent.dwButtonState & 0x02); // RMB
				mouse.x = ir[i].Event.MouseEvent.dwMousePosition.X; //Mouse position X
				mouse.y = ir[i].Event.MouseEvent.dwMousePosition.Y; //Mouse position Y
			}
		}

		//GAME LOGIC=======================

		plr.tick();

		//During play====
		
		//Move the player

		move = { ((bKey[3]) ? 1 : 0) - ((bKey[1]) ? 1 : 0),((bKey[2]) ? 1 : 0) - ((bKey[0]) ? 1 : 0) };

		if (plr.moveCool() <= 0) {
			if (validMove(plrPos, move, 0)) {
				plr.move(move, false);
			}

		}
		

//TEMPORARY Enemy spawn every few seconds within 10 blocks of the player

		/*if (enemySpawn.seconds <= 0) {
			coords newL{ 0,0 };
			do {
				newL = { plr.getPos().x - 10 + rand() % 20, plr.getPos().y - 10 + rand() % 20 };
			} while (!validMove(newL, coords{ 0,0 }));
			enemies.push_back(Enemy(newL));
			enemySpawn.seconds = 10;
		}
		else {
			enemySpawn.seconds -= .05;
		}*/

		//Check player interaction input

		//During pause====

		//RENDER OUTPUT====================

		plrPos = plr.getPos(); //Re-check position each frame
		int plrX = plrPos.x; //Get integer shorthands for EOA
		int plrY = plrPos.y; 

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

				screen[(y /* + nStatFieldHeight */ ) * nScreenWidth + x] = L" #"[map[(plrY - (nFieldHeight / 2) + y) * nGameMapHeight + plrX - (nFieldWidth / 2) + x]]; //Warning that map isn't initialized, but it's fine.
			}
		}

		//Draw the player, right in the middle of the field.

		screen[(nFieldHeight / 2) * nScreenWidth + (nFieldWidth / 2)] = 'O';

		//Render GUIElements last

		for (int i = 0; i < GUIs.size(); i++) { //For each GUI element
			GUIElement& cGUI = GUIs[i];
			if (cGUI.getVisibility()) { //If the GUI element is visible
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