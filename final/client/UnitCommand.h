#pragma once
#include "parser.h"

enum class eUnitCommand {
	CMD_MOVE,
	CMD_ATTACK,
	CMD_SPAWN,
	CMD_DEFEND
};

struct CMD
{
	eUnitCommand c;
	POS pos;
	int target_id;
};
