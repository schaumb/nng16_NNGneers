#include "Client.h"
#include "parser.h"

class MYCLIENT : public CLIENT
{
public:
	MYCLIENT();
protected:
	virtual std::string GetPassword() { return std::string("u6MPVp"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();
};

CLIENT *CreateClient();
