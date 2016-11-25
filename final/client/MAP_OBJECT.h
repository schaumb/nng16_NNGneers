#pragma once
#include "Pos.h"

struct MAP_OBJECT {
	MAP_OBJECT() = default;
	MAP_OBJECT(const MAP_OBJECT& other) :
		id{other.id},
		hp{ other.hp },
		energy{ other.energy },
		side{ other.side },
		pos{ other.pos }
	{}
	int id, hp, energy, side;
	POS pos;
};