#pragma once
#include <iostream>

struct coords {
	int x;
	int y;
};

struct cooldown {
	float seconds;
};

struct stats {
	int hp;
	int def;
	int sp;
};

struct statsCool {
	float attackCooldown;
	float regenCooldown;
	float moveCooldown;
};