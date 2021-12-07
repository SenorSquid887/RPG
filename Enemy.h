#pragma once

#include <iostream>

#include "commonData.h"
#include "Player.h"

using namespace std;

class Enemy {
public:
	Enemy();
	Enemy(coords);
	Enemy(int, int, coords);
	coords getPos();
private:
	string name;
	int lvl;
	int hpmax;
	int hp;
	int speed;
	int def;
	coords pos;
};

Enemy::Enemy() {
	name = "Test Dummy";
	lvl = 0;
	hpmax = 1;
	hp = hpmax;
	speed = 0;
	def = 0;
	pos.x = 0;
	pos.y = 0;
}

Enemy::Enemy(coords pPos) {
	name = "Test Dummy";
	lvl = 0;
	hpmax = 1;
	hp = hpmax;
	speed = 0;
	def = 0;
	pos.x = pPos.x;
	pos.y = pPos.y;
}

Enemy::Enemy(int cL, int gD, coords pos) {
	name = "Enemy1";
	lvl = (gD + (cL / 10)) / 3;
	hpmax = lvl * 10;
	hp = hpmax;
	speed = 10;
	def = lvl * (1 + gD);
}

coords Enemy::getPos() {
	return pos;
}