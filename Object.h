#pragma once

#include <iostream>

#include "commonData.h"

using namespace std;

// An object is an item that shows up on the map, and can be interacted with. An object has a base position.

class Object {
public:
	Object();
	virtual void interact();
protected:
	coords pos;
};

Object::Object() {
	pos.x = 0;
	pos.y = 0;
}

void Object::interact() {
	pos.x = 0;
	pos.y = 0;
}