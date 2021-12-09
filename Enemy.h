#pragma once

#include <iostream>
#include <vector>

//#include "commonData.h"
#include "Player.h"

class Enemy;


class Player;

using namespace std;

class Enemy {
public:
	~Enemy();
	Enemy();
	Enemy(coords);
	Enemy(int, int, coords);
	coords getPos();
	void tick();
	void move(coords);

	bool checkHit(int);
	void takeDamage(int);
	int attack();
	int doDamage();

	int giveXP();

	bool dead();
	float moveCool();
	float attackCool();
private:
	string name;
	int lvl;
	int hpmax;
	int hp;
	int speed;
	int def;
	int atk;
	int acc;
	coords pos;

	stats modifiers;

	statsCool cooldowns;
};

Enemy::~Enemy() {

}

Enemy::Enemy() {
	name = "Test Dummy";
	lvl = 1;
	hpmax = 1;
	hp = hpmax;
	speed = 0;
	def = 1;
	pos.x = 0;
	pos.y = 0;
	atk = 0;
	acc = 0;
	modifiers = { 0,0,0 };
}

Enemy::Enemy(coords pPos) {
	name = "Test Dummy";
	lvl = 1;
	hpmax = 1;
	hp = hpmax;
	speed = 0;
	def = 1;
	pos.x = pPos.x;
	pos.y = pPos.y;
	atk = 0;
	acc = 0;
	modifiers = { 0,0,0 };
}

Enemy::Enemy(int cL, int gD, coords pPos) {
	name = "Enemy1";
	lvl = (int) ((gD+(cL/5.0))/(10.0/cL));
	hpmax = 3 * lvl + 3;
	hp = hpmax;
	speed = 5;
	def = 15 * lvl / (lvl + 1);
	modifiers = { 0,0,0 };
	atk = lvl / 5 + 3;
	acc = 10 * lvl / (lvl + 5);
	pos = { pPos.x,pPos.y };
	cooldowns = { 0,0,0 };
}

void Enemy::move(coords c) {
	pos.x = c.x;
	pos.y = c.y;
	cooldowns.moveCooldown = (float) ((speed + modifiers.sp) * 0.05);
}

void Enemy::tick() {
	if (cooldowns.regenCooldown <= 0) {
		if (hp < hpmax) {
			hp++;
			cooldowns.regenCooldown = 1;
		}
	}
	cooldowns.moveCooldown -= (float)((cooldowns.moveCooldown <= 0) ? 0 : 0.05);
	cooldowns.attackCooldown -= (float)((cooldowns.attackCooldown <= 0) ? 0 : 0.05);
	cooldowns.regenCooldown -= (float)((cooldowns.regenCooldown <= 0) ? 0 : 0.05);
}

coords Enemy::getPos() {
	return coords{ pos.x,pos.y };
}

bool Enemy::checkHit(int h) {
	if (h >= def) {
		return true;
	}
	return false;
}

void Enemy::takeDamage(int h) {
	hp -= h;
}

int Enemy::attack() {
	if (cooldowns.attackCooldown <= 0) {
		cooldowns.attackCooldown = 2.0;
		return (acc + (rand() % 20));
	}
	return 0;
}

int Enemy::doDamage() {
	return atk;
}

int Enemy::giveXP() {
	return 20*lvl*lvl + 1;
}

bool Enemy::dead() {
	return (hp <= 0);
}

float Enemy::moveCool() {
	return cooldowns.moveCooldown;
}

float Enemy::attackCool() {
	return cooldowns.attackCooldown;
}