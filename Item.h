#pragma once

#include <iostream>

class Item {
public:
	virtual void use();
private:
	int count;
};

void Item::use() {
	cout << "NO. - Respectfully, Item";
}