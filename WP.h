using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

#ifndef __WP__
#define __WP__
#include "Exec.h";
#include "WS.h";
 ref class WP:Exec
{
private:

	
	List<Exec^>^ execList;
public:
	void addExecutable(Exec^);
	WP(String^ n,__int64 ID,__int64 in,WP^p):Exec(n,ID,in,p){
		execList=gcnew List<Exec^>;
		
	
	}
	
	Exec^ getExecutable(__int64 i){if(i<getExecutableCount()){return execList[i];}};
	
	__int64 getExecutableCount(){return execList->Count;}

};
#endif