#pragma once

#include "parser.h"
#include "StepOffer.h"
#include <sstream>
#include <fstream>
#include "distcache.h"
#include "fleepath.h"
#include "Client.h"
#include "IAreaStrategy.h"
#include <utility>

struct Queen : public MAP_OBJECT
{
	Queen(MAP_OBJECT& me, CLIENT& client, IAreaStrategy* AStrategy) :
		MAP_OBJECT{ me },
		parser{ client.mParser },
		mDistCache{ client.mDistCache },
		mDebugLog{ client.mDebugLog },
		mFleePath{ client.mFleePath },
		AreaStrategy {AStrategy}
	{}


	PARSER& parser;
	DISTCACHE& mDistCache;
	std::ofstream& mDebugLog;
	FLEEPATH& mFleePath;
	IAreaStrategy* AreaStrategy;
	// akit követek
	int enemyID = -1;

	int Hits_to_die(MAP_OBJECT& unit)
	{
		auto cell = parser.GetAt(unit.pos);
		int HPChange = 0;
		if (cell == eGroundType::CREEP)
		{
			HPChange -= HP_DECAY_ON_ENEMY_CREEP;
		}
		if (cell == eGroundType::ENEMY_CREEP)
		{
			HPChange += HP_REGEN_ON_FRIENDLY_CREEP;
		}
		if (unit.side != 0)
		{
			HPChange += -1;
		}
		return std::ceil((double)unit.hp / (double)(QUEEN_DAMAGE - HPChange));
	}

	bool changeOpponent(MAP_OBJECT& current, MAP_OBJECT& other, int OtherDist)
	{
		auto distDiff = mDistCache.GetDist(pos, current.pos) - OtherDist * 2;
		return Hits_to_die(current) + distDiff > Hits_to_die(other);
	}

	inline std::pair<bool,POS> canBuild()
	{
		if (!(mFleePath.GetDistToFriendlyCreep(pos) <= 1) || energy < QUEEN_BUILD_CREEP_TUMOR_COST) { return std::make_pair(false, pos); }
		POS bestpos; 
		int bestgoodness = 0;
		int goodness = AreaStrategy->GetSpawnGoodness(pos.x, pos.y);
		if (goodness> bestgoodness)
		{
			goodness = bestgoodness;
			bestpos = POS(pos.x, pos.y);
		}
		goodness = AreaStrategy->GetSpawnGoodness(pos.x-1, pos.y);
		if (goodness> bestgoodness)
		{
			goodness = bestgoodness;
			bestpos = POS(pos.x-1, pos.y);
		}
		goodness = AreaStrategy->GetSpawnGoodness(pos.x, pos.y-1);
		if (goodness> bestgoodness)
		{
			goodness = bestgoodness;
			bestpos = POS(pos.x, pos.y-1);
		}
		goodness = AreaStrategy->GetSpawnGoodness(pos.x+1, pos.y);
		if (goodness> bestgoodness)
		{
			goodness = bestgoodness;
			bestpos = POS(pos.x+1, pos.y);
		}
		goodness = AreaStrategy->GetSpawnGoodness(pos.x, pos.y+1);
		if (goodness> bestgoodness)
		{
			goodness = bestgoodness;
			bestpos = POS(pos.x, pos.y+1);
		}

		if (bestgoodness < 0) { return std::make_pair(false, pos); }
		else { return std::make_pair(true, bestpos); }
	}
	StepOffer CalcOffer()
	{
		StepOffer retval;
		const auto myCell = parser.GetAt(pos);
		retval.Build.command.c = eUnitCommand::CMD_SPAWN;
		//build
		if (mFleePath.GetDistToFriendlyCreep(pos) <= 1 && energy >= QUEEN_BUILD_CREEP_TUMOR_COST)
		{
			retval.Build.command.pos = pos;
			//minél több energia, minél kevesebb élet.
			retval.Build.certanty = (5 * energy) / QUEEN_MAX_ENERGY;
			retval.Build.certanty += (5 * (QUEEN_MAX_HP - hp)) / QUEEN_MAX_HP;
		}

		//attack
		//in hits
		int ourArmyHP=0, enemyArmyHP=0, closestEnemyDistance=10000;
		std::vector<MAP_OBJECT>::iterator opponent = parser.Units.end();
		std::vector<MAP_OBJECT>::iterator closestEnemy = parser.Units.end();

		for (auto unit = parser.Units.begin(); unit != parser.Units.end(); ++unit)
		{
			auto dist = mDistCache.GetDist(pos, unit->pos);
			if (unit->side != 0 && closestEnemyDistance> dist)
			{
				closestEnemyDistance = dist;
				closestEnemy = unit;
			}

			if (dist < 5)
			{
				if (unit->side == 0)
				{
					ourArmyHP += Hits_to_die(*unit);
				}
				else
				{
					if (dist <= 1)
					{
						retval.EnemysInHitRange++;
					}
					enemyArmyHP += Hits_to_die(*unit);
				}
				if (opponent == parser.Units.end() || changeOpponent(*opponent, *unit, dist))
				{
					opponent = unit;
				}
			}
		}

		int actualcreep = 0;
		if (opponent != parser.Units.end())
		{
			//pánikszerûen építkezni kezdünk mert ölnek
			auto buildpos = canBuild();
			if (buildpos.first && ceil((float)Hits_to_die(*this) / (retval.EnemysInHitRange)) <= floor((float)energy / QUEEN_BUILD_CREEP_TUMOR_COST))
			{
				retval.Build.command.pos = buildpos.second;
				retval.Build.certanty = 10;
			}
			//ütik a hatcheryt, erõsebbek vagyunk, feláldozom magam(mert lesz másik)
			else if (mDistCache.GetDist(parser.OwnHatchery.pos, opponent->pos) < 3
				|| ourArmyHP >= enemyArmyHP
				|| parser.OwnHatchery.energy >= HATCHERY_BUILD_QUEEN_COST + (3 * actualcreep + 50) / 4)
			{
				retval.Attack.command.c = eUnitCommand::CMD_ATTACK;
				retval.Attack.certanty = 10;
				retval.Attack.command.target_id = opponent->id;
			}
			//hátrálunk
			else
			{
				retval.Attack.command.c = eUnitCommand::CMD_MOVE;
				retval.Attack.certanty = 10;
				retval.Attack.command.pos = parser.OwnHatchery.pos;

			}
		}
		//épületet akarunk ölni!
		else 
		{
			std::vector<MAP_OBJECT>::iterator closestFriendlyBuilding, closestEnemyBuilding, closestFriendToEnemy;
			int minEnemyBuildingRange = 10000, minFriendBuildingRange = 10000, minFriendToEnemy=10000, minEnemyToEnemy=1000;
			for (auto building = parser.CreepTumors.begin(); building!= parser.CreepTumors.end(); ++building)
			{
				auto dist = mDistCache.GetDist(building->pos, pos);
				auto enemydist = mDistCache.GetDist(building->pos, closestEnemy->pos);
				if (building->side == 0 && dist < minFriendBuildingRange)
				{
					minFriendBuildingRange = dist;
					closestFriendlyBuilding = building;
				}
				if (building->side != 0 && dist < minEnemyBuildingRange)
				{
					minEnemyBuildingRange = dist;
					closestEnemyBuilding = building;
					if (enemydist < minEnemyToEnemy)
					{
						minFriendToEnemy = enemydist;
					}
				}
				if (building->side == 0 && enemydist < minFriendToEnemy)
				{
					minFriendToEnemy = enemydist;
					closestFriendToEnemy = building;
				}

			}

			if (minEnemyBuildingRange < minFriendToEnemy)
			{
				//támadunk
				if (parser.OwnHatchery.energy >= HATCHERY_BUILD_QUEEN_COST || closestEnemyBuilding->hp / QUEEN_DAMAGE + minEnemyBuildingRange < minEnemyToEnemy)
				{
					retval.Attack.certanty = 5;
					retval.Attack.command.c = eUnitCommand::CMD_ATTACK;
					retval.Attack.command.target_id = closestEnemyBuilding->id;
				}
				//kolbászolunk a határon
				else
				{
					retval.Attack.certanty = 2;
					if (myCell == eGroundType::CREEP)
					{
						retval.Attack.command.c = eUnitCommand::CMD_MOVE;
						retval.Attack.command.pos = closestEnemyBuilding->pos;
					}
					else
					{
						retval.Attack.command.c = eUnitCommand::CMD_MOVE;
						retval.Attack.command.pos = closestFriendToEnemy->pos;
					}
				}
			}
			else 
			{
				//futunk védeni
				retval.Attack.command.c = eUnitCommand::CMD_MOVE;
				retval.Attack.command.pos = closestFriendToEnemy->pos;
				retval.Attack.certanty = 9;
			}
		}

		return retval;
	}
};
