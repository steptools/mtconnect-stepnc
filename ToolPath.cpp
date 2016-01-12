#include "ToolPath.h";


ToolPath^ ToolPath::nextPath(){
	if(parent!=nullptr){
		return parent->getPath(getIndex()+1);
	
	}else{
		
		Console::WriteLine("Error path  is not nested in a WS");
		return nullptr;}
return nullptr;
}

ToolPath^ ToolPath::nextPath(bool % newWP, bool% newWS){
	ToolPath^temp=nextPath();
	Exec^ tempExec;
	if (temp!=nullptr){
		newWP=false;
		newWS=false;
	return temp;
	}
	else{
		WS^ tempWS=parent;
		List<WP^>^path=pathtoRoot(tempWS);
	}
}
List<WP^>^pathtoRoot(WS^temp){
	List<WP^> ^path=gcnew List<WP^>();
	WP^ temp1=temp->getParent();
	if(temp!=nullptr){
		while (temp1!=nullptr){
		path->Add(temp1);
		temp1=temp1->getParent();
		
		
		}
	
	}

	return path;

}
ToolPath^ recurseToNextToolPath(Exec^ currentExec,Exec^ prevExec){


	
}