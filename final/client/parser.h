#pragma once

static const int HATCHERY_BUILD_QUEEN_COST = 24000;
static const int HATCHERY_MAX_ENERGY = 80000;
static const int HATCHERY_MAX_HP = 1500;
static const int QUEEN_BUILD_CREEP_TUMOR_COST = 100;
static const int QUEEN_MAX_ENERGY = 200;
static const int QUEEN_MAX_HP = 175;
static const int CREEP_TUMOR_SPAWN_ENERGY = 60;
static const int CREEP_TUMOR_MAX_HP = 200;
static const int QUEEN_DAMAGE = 40;
static const int HP_DECAY_ON_ENEMY_CREEP = 2;
static const int HP_REGEN_ON_FRIENDLY_CREEP = 2;
static const int ENERGY_REGEN = 1;
static const int MAX_QUEENS = 8;
static const int HATCHERY_SIZE = 3;
static const int MAX_TICK = 1200;
#include "MAP_OBJECT.h"
#include "eGroundType.h"

struct PARSER
{
	PARSER();
	int tick;
	int versus[2]; // your ID and enemy ID. versus[1]==0 if not real opponent (test)
	int w, h;
	std::vector<eGroundType> Arena;
	std::vector<MAP_OBJECT> Units;
	MAP_OBJECT OwnHatchery;
	MAP_OBJECT EnemyHatchery;
	std::vector<MAP_OBJECT> CreepTumors;

	eGroundType GetAt(const POS &p) const { return p.x<w && p.y<h ? Arena[p.x + p.y*w] : eGroundType::WALL; }
	void ParseUnits(const std::vector<std::string> &ServerResponse, int &index, int count, std::vector<MAP_OBJECT> &container);
	enum eMatchResult {
		ONGOING,
		VICTORY,
		DRAW,
		DEFEAT
	};
	eMatchResult match_result;
	void Parse(const std::vector<std::string> &ServerResponse);
};
