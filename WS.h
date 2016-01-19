using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
#pragma once
#ifndef __WS__
#define __WS__

#include "Exec.h"


//#include "ToolPath.h"
ref class ToolPath;
ref class WS: Exec
{
private:

__int64 tool;
List<ToolPath^>^ pathList;

double max_feed;
double max_spindle;
public:

	//get stpnc ID of tool
	__int64 getToolId();

	double getMaxFeed();
	double getMaxSpindle();

	
	void addPath(ToolPath^t);
	
	ToolPath^ getPath(__int64 i);
	__int64 getPathCount();
	WS(String^ n, __int64 ID, __int64 in, WP^p, __int64 toolID);
	List<WP^>^pathtoRoot();
};
#endif