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
	inline void attack(Player&);
	coords getPos();
	int takeDamage(int);
	bool checkHit(int);
	void tick();
	void move(coords);
private:
	string name;
	int lvl;
	int xp;
	int hpmax;
	int hp;
	int speed;
	int def;
	int atk;
	coords pos;

	stats modifiers;

	statsCool cooldowns;
};

Enemy::Enemy() {
	name = "Test Dummy";
	lvl = 0;
	xp = 1;
	hpmax = 1;
	hp = hpmax;
	speed = 0;
	def = 0;
	pos.x = 0;
	pos.y = 0;
	atk = 0;
	modifiers = { 0,0,0 };
}

Enemy::Enemy(coords pPos) {
	name = "Test Dummy";
	lvl = 0;
	xp = 1;
	hpmax = 1;
	hp = hpmax;
	speed = 0;
	def = 0;
	pos.x = pPos.x;
	pos.y = pPos.y;
	atk = 0;
	modifiers = { 0,0,0 };
}

Enemy::Enemy(int cL, int gD, coords pos) {
	name = "Enemy1";
	lvl = (gD + (cL / 10)) / 3;
	xp = 
	hpmax = lvl * 10;
	hp = hpmax;
	speed = 3;
	def = lvl * (1 + gD);
	modifiers = { 0,0,0 };
	atk = cL * gD / 2;
}

inline void Enemy::attack(Player &p)
{
	if (p.checkHit()) {
		p.takeDamage(atk - 1 + (rand() % 3));
	}
}

bool Enemy::checkHit(int a) {
	if (a >= def) {
		return true;
	}
	return false;
}

int Enemy::takeDamage(int a) {
	hp -= a;
	cooldowns.regenCooldown = 10;
	if (hp <= 0) {
		return xp;
	}
	return 0;
}

void Enemy::move(coords c) {
	pos.x = c.x;
	pos.y = c.y;
	cooldowns.moveCooldown = ((speed - modifiers.sp) * 0.05);
}

void Enemy::tick() {
	if (cooldowns.regenCooldown <= 0) {
		if (hp < hpmax) {
			hp++;
			cooldowns.regenCooldown = 1;
		}
	}
	cooldowns.attackCooldown -= 0.05;
	cooldowns.moveCooldown -= 0.05;
	cooldowns.regenCooldown -= 0.05;
}

coords Enemy::getPos() {
	return coords{ pos.x,pos.y };
}