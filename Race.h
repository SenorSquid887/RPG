#pragma once
#include "Elf.h"
#include "Dwarf.h"
#include "Human.h"

template<class type>
class Race : pubilc type{
public:
	Race() : type() { baseSpeed += 2; } //Base tiles per second
};
