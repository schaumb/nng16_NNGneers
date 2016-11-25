#include "stdafx.h"
#include "MyClient.h"
#include "DumbAreaStrategy.h"

MYCLIENT::MYCLIENT()
    : strategy(std::make_unique<DumbAreaStrategy>(*this))
{
}

void MYCLIENT::Process()
{
    // rm dead queens
    for(auto it = myQueens.begin(); it != myQueens.end();) {
        int id = it->first;
        if(std::all_of(mParser.Units.begin(), mParser.Units.end(), [&id](const MAP_OBJECT& o) { return o.id != id; })) {
            it = myQueens.erase(it);
        } else {
            ++it;
        }
    }
    
    for(auto& queen : mParser.Units) if(queen.side == 0) {
        myQueens.emplace(queen.id, Queen(queen, *this));
    }


    strategy->Process();
    
    auto&& pos = strategy->GetTumorSteps();
    std::cout << "Pos size: " << pos.size() << std::endl;
    
    for(auto& tumor : mParser.CreepTumors) {
		for (auto itRec = pos.begin(); itRec != pos.end(); ++itRec) {
            if(itRec->command.target_id == tumor.id && tumor.energy >= CREEP_TUMOR_SPAWN_ENERGY) {
				command_buffer << "creep_tumor_spawn " << tumor.id << " " << itRec->command.pos.x << " " << itRec->command.pos.y << std::endl;
                //mUnitTarget[tumor.id] = itRec->command;
				std::cout << "tumor " << tumor.pos.x << " " << tumor.pos.y << "->" << itRec->command.pos.x << " " << itRec->command.pos.y << std::endl;
                pos.erase(itRec);
                break;
            }
        }
    }
    
    for(auto& queen : myQueens) if(queen.second.side == 0) {
        // TODO
    }
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
