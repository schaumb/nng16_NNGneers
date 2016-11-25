#include "stdafx.h"
#include "MyClient.h"

MYCLIENT::MYCLIENT()
{
}

void MYCLIENT::Process()
{

}


CLIENT *CreateClient()
{
	return new MYCLIENT();
}
