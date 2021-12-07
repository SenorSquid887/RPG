#pragma once
#include "Item.h"

#include "Fighter.h"
#include "Rogue.h"

#include "Human.h"

#include <iostream>
#include <vector>

using namespace std;

struct coords {
	int x;
	int y;
};

template<class type1, class type2>
class Player : public type1, public type2{
public:
	Player(string n) : name(n) { lvl = 0; xp = 0; xpthresh = 0; pos.x = 0; pos.y = 0; }
protected:
	void levelup();
	int lvl;
	int xp;
	int xpthresh;
	string name;
	coords pos;
};

template<class type1, class type2>
void Player<type1,type2>::levelup() {

}