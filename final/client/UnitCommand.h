#pragma once
#include "parser.h"

enum eUnitCommand {
	CMD_MOVE,
	CMD_ATTACK,
	CMD_SPAWN
};

struct CMD
{
	eUnitCommand c;
	POS pos;
	int target_id;
};
