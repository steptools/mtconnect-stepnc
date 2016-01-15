#include"WP.h"
#include"stdafx.h"

Exec^ WP::getExecutable(__int64 i){ if (i<getExecutableCount()) { return execList[i]; } };
void WP::addExecutable(Exec^ e) { execList->Add(e); };

__int64 WP::getExecutableCount() { return execList->Count; };

WP::WP(String^ n, __int64 ID, __int64 in, WP^p) :Exec(n, ID, in, p, true) {
	execList = gcnew List<Exec^>();
};
ToolPath ^  WP::firstPath() {

	return  firstPath(this);
}

ToolPath ^  WP::firstPath(WP^ root) {
	if (root == nullptr) {
		root = this;
	}
	Exec^tempExec;
	WP^ tempWP;
	WS^ tempWS;
	for (int i = 0; i<root->getExecutableCount(); i++) {
		tempExec = root->getExecutable(i);
		if (tempExec->isWP()) {
			tempWP = dynamic_cast<WP^>(tempExec);
			return firstPath(tempWP);
		}
		else {
			tempWS = dynamic_cast<WS^>(tempWS);
			return tempWS->getPath(0);

		}


	}
	return nullptr;

};
__int64 WP::distanceAbove(WP^ ancestor) {
	WP^ temp = this;
	__int64 count = 0;
	while (temp!=nullptr&&temp->getId() != ancestor->getId()) {
	
		temp = temp->getParent;
		count += 1;
		
	
	}
	return count;


}