#include "WS.h"
#include"stdafx.h"

ToolPath^ WS::getPath(__int64 i) { if (i<pathList->Count) { return pathList[i]; } else { return nullptr; } }

__int64 WS::getPathCount(){ return pathList->Count; }

void WS::addPath(ToolPath^ t) { pathList->Add(t); };

__int64 WS::getToolId() { return tool; }
WS::WS(String^ n, __int64 ID, __int64 in, WP^p, __int64 toolID) :Exec(n, ID, in, p, false)
{
	pathList = gcnew List<ToolPath^>;

	tool = toolID;
}

List<WP^>^WS::pathtoRoot() {
	List<WP^> ^path = gcnew List<WP^>();
	WP^ temp1 = getParent();

	
		while (temp1 != nullptr) {
			path->Add(temp1);
			temp1 = temp1->getParent();


		}

	

	return path;

}