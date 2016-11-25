#include "Client.h"
#include "parser.h"
#include "Queen.h"
#include "IAreaStrategy.h"
#include <memory>
#include <map>

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string GetPassword() { return std::string("u6MPVp"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();
	
	std::map<int, Queen> myQueens;
	std::unique_ptr<IAreaStrategy> strategy;
};

CLIENT *CreateClient();
