using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
#pragma once
#ifndef __TOOLPATH__
#define __TOOLPATH__
#include "Exec.h"

ref class ToolPath
{
private:
	bool isRapid;
	double spindleSpeed;
	double feedrate;
	__int64 index;
	__int64 id;
	Exec^ parent;




public:
	ToolPath(Exec^ p,bool rapid,double f,double s,__int64 in, __int64 ID){
	parent=p;
	isRapid=rapid;
	feedrate=f;
	spindleSpeed=s;
	index=in;
	id=ID;
	
	}
	Exec^ getWS(){return parent;};
	Exec^getWP(){return parent->getParent();}
	__int64 getId(){return id;}
	__int64 getIndex(){return index;}
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