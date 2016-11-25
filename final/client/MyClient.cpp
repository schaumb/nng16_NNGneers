#include "stdafx.h"
#include "MyClient.h"
#include "DumbAreaStrategy.h"

MYCLIENT::MYCLIENT()
    : strategy(std::make_unique<DumbAreaStrategy>(*this))
{
}

void MYCLIENT::Process()
{
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
