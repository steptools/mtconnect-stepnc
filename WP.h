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
	WP(String^ n,__int64 ID,__int64 in,WP^p):Exec(n,ID,in,p,true){
		execList=gcnew List<Exec^>;
	}
	
	ToolPath^  WP::firstPath(WP^ root){
		if (root=nullptr){
			root=this;
		}
		Exec^tempExec;
		WP^ tempWP;
		WS^ tempWS;
		for(int i=0;i<root->getExecutableCount();i++){
			tempExec=root->getExecutable(i);
			if (tempExec->isWP){
				tempWP=dynamic_cast<WP^>(tempExec);
				return firstPath(tempWP);
			}else{
				tempWS=dynamic_cast<WS^>(tempWS);
				return tempWS->getPath(0);
			
			}

		
		}
		return nullptr;
			
	}
	Exec^ getExecutable(__int64 i){if(i<getExecutableCount()){return execList[i];}};
	
	__int64 getExecutableCount(){return execList->Count;}

};
#endif