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
private:
	string name;
	int lvl;
	int xp;
	int hpmax;
	int hp;
	int speed;
	int def;
	int atk;
	int acc;
	int damage;
	coords pos;

	stats modifiers;

	statsCool cooldowns;
};

Enemy::~Enemy() {

}

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
	acc = 0;
	damage = 0;
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
	acc = 0;
	damage = 0;
	modifiers = { 0,0,0 };
}

Enemy::Enemy(int cL, int gD, coords pos) {
	name = "Enemy1";
	lvl = (gD + (cL / 5)) / 3;
	xp = lvl * 10;
	hpmax = lvl * 10;
	hp = hpmax;
	speed = 3;
	def = lvl * (1 + gD);
	modifiers = { 0,0,0 };
	atk = cL * gD / 2;
	acc = (int)(cL * gD / 5);
	damage = cL * gD;
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
	cooldowns.attackCooldown -= (float) 0.05;
	cooldowns.moveCooldown -= (float) 0.05;
	cooldowns.regenCooldown -= (float) 0.05;
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
	hp -= hpmax;
}

int Enemy::attack() {
	return acc + (rand() % 10);
}

int Enemy::doDamage() {
	return atk;
}

int Enemy::giveXP() {
	return (int) (lvl * 1.5);
}

bool Enemy::dead() {
	return (hp <= 0);
}