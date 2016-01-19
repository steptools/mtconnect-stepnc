#include "WS.h"
#include"stdafx.h"

ToolPath^ WS::getPath(__int64 i) { if (i<pathList->Count) { return pathList[i]; } else { return nullptr; } }

__int64 WS::getPathCount(){ return pathList->Count; }

void WS::addPath(ToolPath^ t) { pathList->Add(t); };

__int64 WS::getToolId() { return tool; }
WS::WS(String^ n, __int64 ID, __int64 in, WP^p, __int64 toolID) :Exec(n, ID, in, p, false)
{
	pathList = gcnew List<ToolPath^>;
	max_feed = -1;
	max_spindle = -1;
	tool = toolID;
}
double WS::getMaxFeed() {
	double temp = 0;
	ToolPath^ tp;
	if (max_feed == -1) {
		for (int i = 0; i < getPathCount(); i++) {
			tp = getPath(i);
			if (!tp->rapid()) {
				temp = tp->getFeed();

				if (temp > max_feed) {
					max_feed = temp;

				}
			}
		}
	
	
	}
	return max_feed;

}
double WS::getMaxSpindle() {
	double temp = 0;
	ToolPath^ tp;
	if (max_spindle == -1) {
		for (int i = 0; i < getPathCount(); i++) {
			tp = getPath(i);
			if (!tp->rapid()) {
				temp = tp->getSpindle();

				if (temp > max_spindle) {
					max_spindle = temp;

				}
			}
		}


	}
	return max_spindle;

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