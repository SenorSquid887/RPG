#pragma once

#include <iostream>
#include <vector>

#include "commonData.h"
#include "Enemy.h"
#include "Item.h"
#include "Object.h"

using namespace std;

class Player {
public:
	Player(string n) : name(n) { lvl = 0; xp = 0; xpthresh = 10; hpmax = 10; hp = hpmax; speed = 10; def = 10; pos = { 15,7 }; }
	void move(coords&, bool);
	void takeDamage(int);
	void attack(Enemy&);
	char getChar();
	coords getPos();
	int moveCool();
	void addCooldown(statsCool);
	void tick();
	friend ostream& operator << (ostream&, const Player&);

private:
	void regen();
	void gainXP();
	void playerTick(); //Handles all player stat processes that change. Like regen and cooldowns.

	string name;

	int lvl;
	int xp;
	int xpthresh;
	int hp;
	int hpmax;
	int speed;
	int def;

	statsCool cooldowns{ 0,0,0 };

	coords pos;

	stats modifiers{ 0,0,0 };
	stats modifTime{ 0,0,0 };
};

coords Player::getPos(){
	return pos;
}

void Player::move(coords& c, bool tp) {
	if (tp) {
		pos.x = c.x;
		pos.y = c.y;
	}
	else {
		pos.x += c.x;
		pos.y += c.y;
		cooldowns.moveCooldown = 1;
	}
}

int Player::moveCool() {
	return cooldowns.moveCooldown;
}

void Player::addCooldown(statsCool s) {
	if (s.moveCooldown >= 0)
		cooldowns.moveCooldown -= 0.05;
}

void Player::tick() {
	cooldowns.moveCooldown -= (cooldowns.moveCooldown <= 0) ? 0 : 0.05;
	cooldowns.attackCooldown -= (cooldowns.attackCooldown <= 0) ? 0 : 0.05;
	cooldowns.regenCooldown -= (cooldowns.regenCooldown <= 0) ? 0 : 0.05;
}