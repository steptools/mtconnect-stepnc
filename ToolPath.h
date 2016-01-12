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
	double speed;
	__int64 index;
	__int64 id;
	WS^ parent;




public:
	ToolPath(WS^ p,bool rapid,double s,__int64 in, __int64 ID){
	parent=p;
	isRapid=rapid;
	speed=s;
	index=in;
	id=ID;
	
	}
	__int64 getWSId();
	__int64 getWPId();
	WS^ getWS(){return parent;};
	WP^getWP(){return parent->getParent();}
	__int64 getId(){return id;}
	__int64 getIndex(){return index;}
	//get pointer to next path regardless if it is a new WS or WP
	//null if non exist
	ToolPath ^ nextPath(bool %newWS,bool %newWP);
	//pointer to next path in current WS null if no other path is left in the current WS
	ToolPath ^ nextPath();
	ToolPath^ getNextRapid(bool %newWS,bool newWP);
	List<ToolPath^> getConsecutiveRapids();
	List<ToolPath^> getNextConsecutiveRapids();


};
#endif