#pragma once

#include <iostream>

#include "commonData.h"

using namespace std;

class GUIElement {
public:
	GUIElement(string f, bool v, coords& a, coords& b) : fileName(f), visible(v) { pos = {a.x,a.y}; size = {b.x,b.y}; }
	string getFileName();
	coords getPos();
	coords getSize();
	bool getVisibility();
	void toggleVisibility();
	void toggleVisibility(bool);
private:
	string fileName;
	bool visible;
	coords pos;
	coords size;
};

string GUIElement::getFileName() {
	return fileName;
}

coords GUIElement::getPos() {
	return pos;
}

coords GUIElement::getSize() {
	return size;
}

bool GUIElement::getVisibility() {
	return visible;
}

void GUIElement::toggleVisibility(){
	visible = !visible;
}

void GUIElement::toggleVisibility(bool b) {
	visible = b;
}