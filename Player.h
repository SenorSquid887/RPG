#pragma once

#include <iostream>
#include <vector>

#include "commonData.h"
#include "Enemy.h"

class Enemy;

using namespace std;


class Player {
public:
	Player(string);
	void move(coords);
	char getChar();
	coords getPos();
	double moveCool();
	double attackCool();
	void addCooldown(statsCool);
	void tick();

	int attack();
	int doDamage();
	bool checkHit(int);
	void takeDamage(int);

	void gainXP(int);

	bool dead();
private:
	void regen();
	void playerTick(); //Handles all player stat processes that change. Like regen and cooldowns.

	string name;

	int lvl;
	int xp;
	int xpthresh;
	int hp;
	int hpmax;
	int speed;
	int def;
	int atk;
	int acc;

	statsCool cooldowns{ 0,0,0 };

	coords pos;

	stats modifiers{ 0,0,0 };
	stats modifTime{ 0,0,0 };
};

Player::Player(string n) {
	name = n;
	lvl = 0;
	xp = 0;
	xpthresh = 10;
	hpmax = 10;
	hp = hpmax;
	speed = 3;
	def = 10;
	atk = 2;
	pos = { 15,7 };
	acc = 2;
}

coords Player::getPos(){
	return pos;
}

void Player::move(coords c) {
	pos.x = c.x;
	pos.y = c.y;
	cooldowns.moveCooldown = (float) ((speed - modifiers.sp) * 0.05);
}

double Player::moveCool() {
	return cooldowns.moveCooldown;
}

double Player::attackCool() {
	return cooldowns.attackCooldown;
}

void Player::addCooldown(statsCool s) {
	if (s.moveCooldown >= 0)
		cooldowns.moveCooldown -= (float) 0.05;
}

void Player::tick() {
	cooldowns.moveCooldown -= (float) ((cooldowns.moveCooldown <= 0) ? 0 : 0.05);
	cooldowns.attackCooldown -= (float) ((cooldowns.attackCooldown <= 0) ? 0 : 0.05);
	cooldowns.regenCooldown -= (float) ((cooldowns.regenCooldown <= 0) ? 0 : 0.05);
}

void Player::gainXP(int i) {
	xp += 1;
	if (xp >= xpthresh) {
		xp -= xpthresh;
		lvl += 1;
		xpthresh = (int) (xpthresh * 1.5);
	}
}

int Player::attack() {
	return acc + (rand() % 10);
}

int Player::doDamage() {
	return atk;
}

bool Player::checkHit(int h) {
	if (h > def) {
		return true;
	}
	return false;
}

void Player::takeDamage(int h) {
	hp -= h;
}

bool Player::dead() {
	return (hp <= 0);
}