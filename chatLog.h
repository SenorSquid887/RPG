#pragma once

#include <iostream>
#include <fstream>

#include "GUIObject.h"

using namespace std;

class chatLog : public GUIObject {
public:
	void addToChat(string);
private:
	string chat[5];
};