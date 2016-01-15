using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

#ifndef __TOOLPATH__
#define __TOOLPATH__

#include "WS.h"

ref class ToolPath
{
private:
	bool isRapid;
	double spindleSpeed;
	double feedrate;
	__int64 index;
	__int64 id;
	WS^ parent;




public:
	ToolPath(WS^ p, bool rapid, double f, double s, __int64 in, __int64 ID);
	WS^ getWS();
	
	__int64 getId();
	__int64 getIndex();

	ToolPath^ nextPath();
	ToolPath^ nextPath(bool % newWP, bool% newWS);
	ToolPath^ recurseToNextToolPath(WP^ current, __int64 startIndexAfter);
	ToolPath^ recurseToolPath(Exec^ current);
	//get pointer to next path regardless if it is a new WS or WP
	//null if non exist
	/*
	ToolPath ^ nextPath(bool %newWS,bool %newWP);
	//pointer to next path in current WS null if no other path is left in the current WS
	ToolPath ^ nextPath();
	ToolPath^ getNextRapid(bool %newWS,bool newWP);
	List<ToolPath^> getConsecutiveRapids();
	List<ToolPath^> getNextConsecutiveRapids();
	*/

};
#endif