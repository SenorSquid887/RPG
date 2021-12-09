#pragma once

#include <iostream>
#include <vector>

#include "commonData.h"
#include "Enemy.h"

using namespace std;

class Player {
public:
	Player(string n) : name(n) { lvl = 0; xp = 0; xpthresh = 10; hpmax = 10; hp = hpmax; speed = 3; def = 10; atk = 0; pos = { 15,7 }; damage = 2; }
	void move(coords);
	void takeDamage(int);
	inline void attack(Enemy&);
	char getChar();
	coords getPos();
	double moveCool();
	double attackCool();
	void addCooldown(statsCool);
	void tick();
	bool checkHit();

private:
	void regen();
	void gainXP(int);
	void playerTick(); //Handles all player stat processes that change. Like regen and cooldowns.

	string name;

	int lvl;
	int xp;
	int xpthresh;
	int hp;
	int hpmax;
	int speed;
	int def;
	int damage;
	int atk;

	statsCool cooldowns{ 0,0,0 };

	coords pos;

	stats modifiers{ 0,0,0 };
	stats modifTime{ 0,0,0 };
};

coords Player::getPos(){
	return pos;
}

void Player::move(coords c) {
	pos.x = c.x;
	pos.y = c.y;
	cooldowns.moveCooldown = ((speed - modifiers.sp) * 0.05);
}

inline void Player::attack(Enemy &e) {
	if (e.checkHit(rand()%20)) {
		gainXP((e.takeDamage(rand() % (3) + damage)));
	}
}

double Player::moveCool() {
	return cooldowns.moveCooldown;
}

double Player::attackCool() {
	return cooldowns.attackCooldown;
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

void Player::gainXP(int i) {
	xp += 1;
	if (xp >= xpthresh) {
		xp -= xpthresh;
		lvl += 1;
		xpthresh = xpthresh * 1.5;
	}
}