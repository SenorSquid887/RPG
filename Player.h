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

	string getName();
	int getLevel();

	string dispHP();
	string dispXP();
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
	if (cooldowns.regenCooldown <= 0) {
		if (hp < hpmax) {
			hp++;
			cooldowns.regenCooldown = 1.0;
		}
	}
	cooldowns.moveCooldown -= (float) ((cooldowns.moveCooldown <= 0) ? 0 : 0.05);
	cooldowns.attackCooldown -= (float) ((cooldowns.attackCooldown <= 0) ? 0 : 0.05);
	cooldowns.regenCooldown -= (float) ((cooldowns.regenCooldown <= 0) ? 0 : 0.05);
}

void Player::gainXP(int i) {
	xp += i;
	while(xp>=xpthresh) {
		xp -= xpthresh;
		lvl += 1;
		xpthresh = (int)10 + ((25 * lvl * lvl) / 3.0);
		hpmax += (int) (lvl * 2.0)/(lvl+40);
		acc = (int) (15 * (lvl / (lvl + 15.0)) + 2);
		atk += (int) (lvl / 40.0);
		def = (int) (25 * (lvl / (lvl + 15.0)));
	}
}

int Player::attack() {
	if (cooldowns.attackCooldown <= 0) {
		cooldowns.attackCooldown = 1.5;
		return (acc + (rand() % 4));
	}
	return 0;
}

int Player::doDamage() {
	return atk + rand() % 4;
}

bool Player::checkHit(int h) {
	if (h > def) {
		return true;
	}
	return false;
}

void Player::takeDamage(int h) {
	hp -= h;
	cooldowns.regenCooldown = 10;
}

bool Player::dead() {
	return (hp <= 0);
}

string Player::getName() {
	return name;
}

int Player::getLevel() {
	return lvl;
}

string Player::dispXP() {
	return (to_string(xp) + " / " + to_string(xpthresh));
}

string Player::dispHP() {
	return (to_string(hp) + " / " + to_string(hpmax));
}