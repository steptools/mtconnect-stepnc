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


	

	Exec^ getNextExec(){return getParent()->getExecutable(getIndex()+1);};
	void addPath(ToolPath^t){pathList->Add(t);};
	
	ToolPath^ getPath(__int64 i){if(i<pathList->Count){return pathList[i];}else {return nullptr;}}
	WS(String^ n,__int64 ID,__int64 in,WP^p,__int64 toolID):Exec( n,ID,in,p)
	{
	pathList=gcnew List<ToolPath^>;
	
	tool=toolID;
	}

};
#endif