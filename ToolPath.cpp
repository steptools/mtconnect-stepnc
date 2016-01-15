#include "ToolPath.h"
#include"stdafx.h"

ToolPath^ ToolPath::nextPath(){
	if(parent!=nullptr){
		return (parent)->getPath(getIndex()+1);
	
	}else{
		
		Console::WriteLine("Error path  is not nested in a WS");
		return nullptr;}
return nullptr;
}


ToolPath::ToolPath(WS^ p, bool rapid, double f, double s, __int64 in, __int64 ID) {
	parent = p;
	isRapid = rapid;
	feedrate = f;
	spindleSpeed = s;
	index = in;
	id = ID;
	copyID = -1;
}
WS^ ToolPath::getWS() { return parent; }
__int64 ToolPath::getId(){ return id; }
__int64 ToolPath::getIndex() { return index; }

ToolPath^ ToolPath::nextPath(bool % newWP, bool% newWS) {
	ToolPath^temp =nextPath();
	Exec^ tempExec;
	if (temp != nullptr) {
		newWP = false;
		newWS = false;
		return temp;
	}
	else {
		WS^ tempWS = getWS();
		ToolPath^ nextToolPath = nullptr;
		List<WP^>^path = getWS()->pathtoRoot();
		for (int i = 0; i<path->Count; i++) {
			nextToolPath = recurseToNextToolPath(path[i], tempWS->getIndex());
			if (nextToolPath != nullptr) {
				return nextToolPath;
			}

		}
	}
}

ToolPath^ ToolPath::recurseToNextToolPath(WP^ current, __int64 startIndexAfter) {
	ToolPath^ result = nullptr;
	for (int i = startIndexAfter + 1; i<current->getExecutableCount(); i++) {
		result = recurseToolPath(current->getExecutable(i));
		if (result != nullptr) {
			return result;
		}
	}


}
ToolPath^ ToolPath::recurseToolPath(Exec^ current) {
	WS^ tempWS = nullptr;
	WP^ tempWP = nullptr;
	WP^ tempWP2;
	if (current->isWP()) {
		tempWP = dynamic_cast<WP^>(current);
	}
	else {
		tempWS = dynamic_cast<WS^>(current);
	}
	if (tempWS != nullptr) {
		if (tempWS->getPathCount()>0) {
			return tempWS->getPath(0);

		}
	}
	if (tempWP != nullptr) {
		for (int i = 0; i<tempWP->getExecutableCount(); i++) {

			return recurseToolPath(tempWP->getExecutable(i));


		}


	}
	return nullptr;
}

__int64 ToolPath::getCopyID() { return copyID; }
bool ToolPath::copyCreated() { if (copyID == -1) { return false; } else { return true; } }