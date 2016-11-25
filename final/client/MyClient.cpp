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
    for(auto it = myQueens.begin(); it != myQueens.end(); ++it) {
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
    
    auto&& pos = strategy->GetStepOffers();
    int i = 0;
    std::cout << "Pos size: " << pos.size() << std::endl;
    for(auto& queen : mParser.Units) if(queen.side == 0) {
        if(i >= pos.size()) break;
        if(queen.energy >= QUEEN_BUILD_CREEP_TUMOR_COST) {
            mUnitTarget[queen.id] = pos[i].command;
            ++i;
        }
    }
}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
