#include "stdafx.h"
#include "MyClient.h"
#include "DumbAreaStrategy.h"

MYCLIENT::MYCLIENT()
    : strategy(std::make_unique<DumbAreaStrategy>(*this))
{
}

void MYCLIENT::Process()
{
	mUnitTarget.clear();
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
        myQueens.emplace(queen.id, Queen(queen, *this, strategy.get()));
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
    
    // (1)
    for(auto& queen : myQueens) if(queen.second.side == 0) {
        auto&& asd = queen.second.CalcOffer();
        if(asd.Attack.certanty == 10) {
            mUnitTarget[queen.first] = asd.Attack.command;
            std::cout << "Unit " << queen.first << " self choose attack to: " << asd.Attack.command.target_id << std::endl;
            continue;
        }
        if(asd.Build.certanty == 10) {
            mUnitTarget[queen.first] = asd.Build.command;
            std::cout << "Unit " << queen.first << " self choose build to: " << asd.Build.command.pos << std::endl;
            continue;
        }
    }
    std::vector<Step> commands =  strategy->GetQueenAttacks();
    std::vector<Step> steps = strategy->GetQueenSteps();
    commands.insert(commands.end(), steps.begin(), steps.end());
    
    std::stable_sort(commands.begin(), commands.end(), [](const Step& s1, const Step& s2) { return s1.certanty > s2.certanty; });
    
    // (2)
    for(Step st : commands) {
        auto minIt = myQueens.end();
        int minDistance = 80;
        for(auto it = myQueens.begin(); it != myQueens.end(); ++it) {
            if(mUnitTarget.count(it->first)) continue;
            
            int dist  = mDistCache.GetDist(st.command.pos, it->second.pos);
            if(dist < minDistance) {
                minDistance = dist;
                minIt = it;
            }
        }
        if(minIt == myQueens.end()) {
            std::cout << "No one can do this recommended step from strategy" << std::endl;
            continue;
        }
        std::cout << "Unit " << minIt->first << " applied for this job!" << std::endl;
        
        mUnitTarget[minIt->first] = st.command;
    }
    
    // (3) - (4)
    for(auto& queen : myQueens) if(!mUnitTarget.count(queen.first)) {
        auto&& asd = queen.second.CalcOffer();
        if(asd.Attack.certanty > asd.Build.certanty) {
            std::cout << "Unit " << minIt->first << " self attack - or move to (id / pos) " << asd.Attack.command.target_id << " / "  << asd.Attack.command.pos << std::endl;
        
            mUnitTarget[queen.first] = asd.Attack.command;
        } else {
            std::cout << "Unit " << minIt->first << " self build to (id / pos) "  << asd.Attack.command.pos << std::endl;
            mUnitTarget[queen.first] = asd.Attack.command;
        }
    }
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
