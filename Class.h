#pragma once
#include "Fighter.h"
#include "Rogue.h"

template<class type>
class Class : public type {
public:
	Class() : type() { baseDef += 4; } //Base defense value
};

