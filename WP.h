using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

#ifndef __WP__
#define __WP__
#include "Exec.h"

#include"ToolPath.h"

 ref class WP:Exec
	 
{
private:

	
	List<Exec^>^ execList;
public:
	void addExecutable(Exec^ e);
	WP(String^ n, __int64 ID, __int64 in, WP^p);

	Exec^ getExecutable(__int64 i);
	
	__int64 getExecutableCount();
	ToolPath ^  firstPath(WP^ root);

};
#endif