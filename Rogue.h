#pragma once

class Rogue{
public:
	Rogue();
private:
	int baseDef;
	int hpmax;
	int hp;
};

Rogue::Rogue() {
	baseDef = 0;
	hpmax = 10;
	hp = hpmax;
}