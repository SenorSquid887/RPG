#pragma once

class Fighter{
public:
	Fighter();
private:
	int baseDef;
	int hpmax;
	int hp;
};

Fighter::Fighter() {
	baseDef = 2;
	hpmax = 12;
	hp = hpmax;
}