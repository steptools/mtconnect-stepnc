using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

#ifndef __WS__
#define __WS__
#include "Exec.h";

#include "WP.h";

#include "ToolPath.h"
ref class WS: Exec
{
private:

__int64 tool;
List<ToolPath^>^ pathList;


public:

	//get stpnc ID of tool
	__int64 getToolId(){return tool;}


	

	
	void addPath(ToolPath^t){pathList->Add(t);};
	
	ToolPath^ getPath(__int64 i){if(i<pathList->Count){return pathList[i];}else {return nullptr;}}
	__int64 getPathCount(){return pathList->Count;}
	WS(String^ n,__int64 ID,__int64 in,WP^p,__int64 toolID):Exec( n,ID,in,p,false)
	{
	pathList=gcnew List<ToolPath^>;
	
	tool=toolID;
	}

};
#endif