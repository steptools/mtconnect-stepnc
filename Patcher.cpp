#include "Patcher.h"
#include"stdafx.h"
WP^ Patcher::getAllExec(__int64 root,STEPNCLib::Finder ^find,WP^plan,__int64 index){
	WP^ mainWP =nullptr;
	String^ name;
	WS^ mainWS=nullptr;
	ToolPath^mainTP=nullptr;
	__int64 toolID;
	double feed,spindle;
	bool rapid,coolant;
	if (find->IsProgramStructure(root)){
		name=find->GetExecutableName(root);
		
		mainWP=gcnew WP(name,root,index,plan);
		if(plan!=nullptr){
		plan->addExecutable(mainWP);
		}
		List<long long>^nestedPlans =find->GetNestedExecutableAllEnabled (root);
		for (int i=0;i<nestedPlans->Count;i++){
			
				if(find->IsProgramStructure(nestedPlans[i]))
					{
						getAllExec(nestedPlans[i],find,mainWP,i);
					
					}
				else if (find->IsWorkingstep(nestedPlans[i])&&find->IsEnabled(nestedPlans[i])){
					name=find->GetExecutableName(nestedPlans[i]);
					toolID=find->GetWorkingstepTool(nestedPlans[i]);	
					bool ret;
					toolID = find->GetToolNumberAsNumber(toolID, ret);
					mainWS=gcnew WS(name,nestedPlans[i],i,mainWP,toolID);
					mainWP->addExecutable(mainWS);
					//need to add all toolpaths
					List<__int64>^ pathID=find->GetWorkingstepPathAll(nestedPlans[i]);
					for (int j=0;j<pathID->Count;j++){
						find->GetPathProcess(pathID[j],feed,spindle,rapid,coolant);
						mainTP=gcnew ToolPath(mainWS,rapid,feed,spindle,j,pathID[j]);
						mainWS->addPath(mainTP);
					}
				}
				
				}
		
		
		}
	return mainWP;
	}


//generates position information from the xml file
List<array<double>^> ^  Patcher::coordinates(String ^fileName,bool convert_to_inches){
//create instance of the xml reader
	
	XmlDocument ^doc = gcnew XmlDocument();
	doc->Load(fileName);
	XmlNode ^root = doc->DocumentElement;
		
		XmlNamespaceManager ^nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		List<array<double>^> ^Lst = gcnew List<array<double>^>();
		String ^str;
		XmlNodeList^ pathList = root->SelectNodes("//base:PathPosition", nsmgr);
		
		array<double> ^coor;
		for (int i=0;i<pathList->Count;i++){
			XmlNode^ temp=pathList->Item(i);
			str=temp->InnerText;
			if(str!="UNAVAILABLE"){
			//Console::WriteLine(str);
			array<String^>^ arr;
			arr=str->Split(' ');
			coor=gcnew array<double>(3);
			try
			{
			coor[0]=Convert::ToDouble(arr[0]);
			coor[1]=Convert::ToDouble(arr[1]);
			coor[2]=Convert::ToDouble(arr[2]);
			if(convert_to_inches){
			coor[0]=coor[0]*0.0393701;
			coor[1]=coor[1]*0.0393701;
			coor[2]=coor[2]*0.0393701;
			
			}
			//Console::WriteLine(arr[0]);
			//Console::WriteLine(coor[0]);
			Lst->Add((array<double> ^)coor->Clone());
			}
			catch (Exception^ e){
			Console::WriteLine(e);
			
			}
			}
		
		}
return Lst;
}
//request a sampel of position information form server
void Patcher::PullFromServer(MtConnector ^ mt){






STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	////////////////////////
   STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
   apt->Inches();


   apt->Open238 ("hardmoldy_ext.stpnc");
    find->Open238 ("hardmoldy_ext.stpnc");
	//feat->OpenSpecificWorkpiece("test_with_rawpiece2.stpnc","HardModly_design.stp");
	
    int wp_id = find->GetMainWorkplan();
	Console::WriteLine("Main Workplan name %S\n", (find->GetExecutableName(wp_id)));

    // add a new workplan at the end of the project
    apt->NestWorkplanAfter("Workplan for new results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);

    // Starting first workingstep in new plan

    // Create a trivial STEP-NC file
    apt->Workingstep("Extension 1");



    // Create a trivial STEP-NC file
    apt->PartNo ("test part");
	 apt->Rawpiece ("HardMoldy_stock.stp");
    long raw_id = apt->GetCurrentRawpiece ();
	 apt->DefineTool(0.25, 5.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    apt->PutWorkpiecePlacement (raw_id, 0, 0, 0, 0, 0, -1, 1, 0, 0);
	apt->GeometryForToolNumber ("Iscar_2.stp", 1);
    apt->LoadTool (1);
     apt->SpindleSpeed (300);
    apt->Feedrate (30);


		apt->GoToXYZ("point0", 0,0,200);
		apt->GoToXYZ("point0", 0,0,200);

   String ^str;
	array<double>^ coor;
	for (int i=0;i<100;i++){
		//Console::WriteLine(i);
	coor=mt->getPos();
	if(coor!=nullptr){

		str= gcnew String(String::Format("point {0} ",i));
		Console::WriteLine(str);
        Console::WriteLine(coor[0]);

		apt->GoToXYZ(str, coor[0]*0.0393701, coor[1]*0.0393701, coor[2]*0.0393701);
	}
	}
	
	
    apt->SaveAsModules("hardmoldy_extended");
	Console::WriteLine("done");
	String ^readIn =Console::ReadLine();
 

}


void Patcher::appendToFile(String^ file_name,String^ original,String^ new_file_name,bool convert_to_inches){
	//List<array<double>^> ^coor_List= coordinates(file_name,convert_to_inches);
	RawData^ sample = gcnew RawData();
	sample->changeSourceTxt(file_name);
	sample->parse();
	//sample->convertMMToInches();
	
	// Create a trivial STEP-NC file
	Console::WriteLine("Got Position Data From File");
    STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	////////////////////////
   STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	   
	    //	    APT.Open238("new_hardmoldy.stpnc");
	    //	    Find.Open238("new_hardmoldy.stpnc");
	    
	    apt->Open238(original);
		apt->Inches();
	    long wp_id = find->GetMainWorkplan();
	    Console::WriteLine("Main Workplan name " + find->GetExecutableName(wp_id));

	    // add a new workplan at the end of the project
	    apt->NestWorkplanAfter("Workplan for MTConnect results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);

	    // Starting first workingstep in new plan
	    apt->Workingstep("testing");



//////////////////////////
    //apt->PartNo("test part");
    //apt->DefineTool(0.25, 5.0, 1.0, 2.0, 3.0, 4.0, 5.0);
	//apt->GeometryForToolNumber ("Iscar_2.stp", 1);


		//String^ boeingTool=find->GetToolNumberAsNumber()
		Console::WriteLine("is tool defined {0}",apt->IsToolDefined(1));
			apt->LoadTool(1);
    apt->SpindleSpeed (300);
    apt->Feedrate (30);
	String ^str;
	array<double>^ coor;
	for (int i=0;i<sample->getSize();i++){
		coor=sample->getCoor(i);
		
		str=String::Format("point {0} ",i+1);
		//Console::WriteLine(str);
		//Console::WriteLine(" {0} {1} {2}",coor[0],coor[1],coor[2]);

		apt->GoToXYZ(str, coor[0], coor[1], coor[2]);
	
	}
	apt->EndWorkplan();
	apt->EndWorkplan();
	
	apt->SaveAsModules(new_file_name);
	Console::WriteLine("done");
	
   
}
//use a path of wP1/wp2/wp3 to get id of wp3 
long long Patcher::getWorkPlanByPath(array<String^>^ nameList,STEPNCLib::Finder ^find){
	long long root=find->GetMainWorkplan();
	bool found=false;
	List<long long>^tempList=gcnew List<long long>();
	tempList->Add(root);
	for (int i=0;i<nameList->Length;i++){
		found=false;
		for each(long long x in tempList){
			if(find->IsProgramStructure(x)){
			if(find->GetExecutableName(x)==nameList[i]){
				//Console::WriteLine(find->GetExecutableName(x));
			root =x;
			tempList=find->GetNestedExecutableAll(root);
			found=true;
			break;
			}
			}
		
		}
		if(found==false){
		return -1;
		}
	
	
	}
	
	Console::WriteLine("Found WorkPlan{0}",find->GetExecutableName(root));
	return root;
}
//recursively get all enabled worksteps
void Patcher::getAllWorksteps(long long root,STEPNCLib::Finder ^find,List<long long>^ wsList){
	if (find->IsProgramStructure(root)){
		List<long long>^nestedPlans =find->GetNestedExecutableAllEnabled (root);
		for each (long long x in nestedPlans){
			
				if(find->IsProgramStructure(x))
					{
						getAllWorksteps(x,find,wsList);
					
					}
				else if (find->IsWorkingstep(x)&&find->IsEnabled(x)){
					wsList->Add(x);
				}
				
				}
		
		
		}
	
	}
//each array is the tool coordinate with delta t to reach the particular pt
//the first cooridinate has a time stamp of 0
List<array<double>^> ^  Patcher::coordinatesWithTime(String ^fileName,bool convert_to_inches){
//create instance of the xml reader
	
	XmlDocument ^doc = gcnew XmlDocument();
	doc->Load(fileName);
	XmlNode ^root = doc->DocumentElement;
		
		XmlNamespaceManager ^nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		List<array<double>^> ^Lst = gcnew List<array<double>^>();
		String ^str;
		XmlNodeList^ pathList = root->SelectNodes("//base:PathPosition", nsmgr);
		DateTime  tempTime=  DateTime();
		
		DateTime  tempTime2=  DateTime();
		array<double> ^coor;
		bool firstTimeStamp=true;
		for (int i=0;i<pathList->Count;i++){
			XmlNode^ temp=pathList->Item(i);
			str=temp->InnerText;
			if(str!="UNAVAILABLE"){
				
			//Console::WriteLine(str);
			array<String^>^ arr;
			arr=str->Split(' ');
			coor=gcnew array<double>(4);
			if(firstTimeStamp){
					tempTime=tempTime.Parse(temp->Attributes["timestamp"]->Value);
				coor[3]=0;
				firstTimeStamp=false;
			}else{
			tempTime2=tempTime2.Parse(temp->Attributes["timestamp"]->Value);
			//Console::WriteLine("    timestamp {0}",tempTime2);
			System::TimeSpan diff=(tempTime2.Subtract(tempTime));
			//Console::WriteLine("diff {0}",diff.TotalSeconds);
			coor[3]=diff.TotalSeconds;
			tempTime=tempTime2;

				
			
			
			}

			try
			{
			coor[0]=Convert::ToDouble(arr[0]);
			coor[1]=Convert::ToDouble(arr[1]);
			coor[2]=Convert::ToDouble(arr[2]);
			if(convert_to_inches){
			coor[0]=coor[0]*0.0393701;
			coor[1]=coor[1]*0.0393701;
			coor[2]=coor[2]*0.0393701;
			
			}
			//Console::WriteLine(arr[0]);
			//Console::WriteLine(coor[0]);
			Lst->Add((array<double> ^)coor->Clone());
			}
			catch (Exception^ e){
			Console::WriteLine(e);
			
			}
			}
		
		}
return Lst;
}
List<List<long long>^> ^Patcher::getWSPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	
	List<List<long long>^> ^ pathList=gcnew List<List<long long>^>();
	getAllWorksteps(workplan,find,wsList);
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->Add(find->GetWorkingstepPathAll(x));
		
	}
	return pathList;
}
/*
List<List<long long>^> ^getAllFeedPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	List<long long>^pathList=gcnew List<long long>();
	List<List<long long>^> ^ feedToolPaths=gcnew List<List<long long>^>();
	List<long long>^ tempList=nullptr;
	bool startedRapid=true;
	getAllWorksteps(workplan,find,wsList);
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->AddRange(find->GetWorkingstepPathAll(x));
		
	}
	for each (long long path in pathList){
		if (tempList==nullptr){
			tempList=gcnew List<long long>();
			startedRapid=true;
		}

		if(!find->GetPathRapid(path)){

			tempList->Add(path);
			startedRapid=false;
		}else{
			if(startedRapid==false){
				feedToolPaths->Add(tempList);
				tempList=nullptr;
			
			}

		
		}
	
	}
	return feedToolPaths;
}
*/
//return list of array of all rapid tool paths
//each array repesents a consecutive rapid movement
List<List<long long>^> ^ Patcher::getAllRapidPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	List<long long>^pathList=gcnew List<long long>();
	List<long long>^tempPathList=nullptr;
	List<List<long long>^> ^ rapidToolPaths=gcnew List<List<long long>^>();
	//get all enabled working steps form a specified work plan
	getAllWorksteps(workplan,find,wsList);
	Console::WriteLine("workplan {0}",workplan);
	//for each ws append all of the tool paths within that
	//ws to pathList
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->AddRange(find->GetWorkingstepPathAll(x));
		
	}
	bool prevPathIsRapid=false;
	bool currentPathIsRapid=false;
	
	//current path is index i-1
	for( int i=0;i<pathList->Count;i++){
		currentPathIsRapid=find->GetPathRapid(pathList[i]);

		if(currentPathIsRapid &&tempPathList==nullptr){
			tempPathList=gcnew List<long long>();
			tempPathList->Add(pathList[i]);
		}else if(currentPathIsRapid &&tempPathList!=nullptr){
			tempPathList->Add(pathList[i]);
		}else if(!currentPathIsRapid &&tempPathList!=nullptr){
			rapidToolPaths->Add(tempPathList);
			tempPathList=nullptr;
		}


		
	
	}
	return rapidToolPaths;
}

List<double> ^Patcher::getAllPoints(List<long long>^ paths,STEPNCLib::Finder ^find){
	List<long long >^ curves=nullptr;
	List<double> ^pts=gcnew List<double>();
	for each (long long path in paths){
		curves=find->GetPathCurveAll(path);
		for each (long long curve in curves){
			pts->AddRange(find->GetPathPolylinePointAll(curve));
		
		
		}
	
	}
return pts;
}
//takes coordinates from curves in the paths form pathlsit and generates tool paths
void Patcher::patchRapid(List<long long>^ pathList,STEPNCLib::AptStepMaker ^ apt, STEPNCLib::Finder ^find,double max_feed){
	apt->Rapid();
	apt->Workingstep("starting Rapid");
		List<double>^ pts=getAllPoints(pathList,find);
		int totalPts=pts->Count/3;
		for (int i=0;i<totalPts;i++){
			apt->GoToXYZ("rapid",pts[i*3],pts[i*3+1],pts[i*3+2]);
		
		}

	apt->Feedrate(max_feed);
	apt->Workingstep("starting Feed");
}


void Patcher::debugOut(STEPNCLib::AptStepMaker^apt, STEPNCLib::Finder^ find,double feed) {
	
	Console::WriteLine("feed {0}", feed);

}

//given a stpnc file and a workplan within the stepnc file
// append a workplan with tool paths genereated from the xml data
//that contains tool position
//the tool paths represented in the xml file should be identical to the
//toolpaths of the ws in the identified workplan
void Patcher::appendPatchWorkPlan(String^partFile,String^coorFile,String^outName,String^ path,bool toInches ){
	Console::WriteLine("input \n part {0}\n xml file{1} \n path to desired workplan {2}",partFile,coorFile,path);
	//assume all units are inches or inches per sec
	double max_feed=.3;//inches per sec
	//get path position coordinates from the xml file with delta t in seconds at end

	
	//List<array<double>^> ^coorList=	coordinatesWithTime(coorFile,toInches);
	RawData^ sample = gcnew RawData();
	sample->changeSourceTxt(coorFile);
	sample->parse();
	sample->convertMMToInches();
	Console::WriteLine("Got coordinates list");
	STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	array<String^>^ nameList=path->Split('/');

	STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	apt->Inches();
	find->APIUnitsInch();
	//open the part file
	apt->Open238 (partFile);

	find->Open238 (partFile);
	//get the workplan id 
	long long id= getWorkPlanByPath(nameList,find);
	//get all tool paths within a work plan that has a transition from feed to rapid or
	//rapid to feed
	List<List<long long>^> ^transitionPaths= getAllRapidPaths(id, find);

	//begin the extended version of the stpnc file


	long wp_id = find->GetMainWorkplan();
	Console::WriteLine("Main Workplan name " + find->GetExecutableName(wp_id));

	// add a new workplan at the end of the project
	apt->NestWorkplanAfter("Workplan for MTConnect results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);
	// Starting first workingstep in new plan

	apt->Workingstep("testing");


	Console::WriteLine("is tool defined {0}",apt->IsToolDefined(1));
	apt->LoadTool(1);
	apt->SpindleSpeed (300);
	apt->Feedrate (max_feed);


	String ^str;
	array<double>^ coor1;
	array<double>^coor2;
	
	bool startRapid=false;
	bool currentCoorisRapid=true;
	int currentTransition=0;
	__int64 debugStep;
	//Console::WriteLine("number of path coordinates {0}",coorList->Count);
	//Console::WriteLine("number of transitions {0}",transitionPaths->Count);
	//determine if starting position is in rapid mode or feed
	if(sample->getSize()>1){
		coor1 = sample->getCoor(0);
		coor2= sample->getCoor(1);
		
		if (actualFeedRate(coor1, coor2)>max_feed){
			currentCoorisRapid=true;
			

		}else{

			currentCoorisRapid=false;
		}
			
		if(currentCoorisRapid){
				apt->Rapid();
			}else{
			

				apt->Feedrate(max_feed);
			}
		}
	
	double actualFeed=0;
	//iterate over all pair of consecutive points
	for (int i=1;i<sample->getSize();i++){
		coor1=sample->getCoor(i-1);
		coor2=sample->getCoor(i);

		str=String::Format("point {0} ",i+1);
		actualFeed=actualFeedRate(coor1,coor2);
		if (actualFeed>max_feed){
			currentCoorisRapid=true;
			//Console::WriteLine("actualFeed {0}",actualFeed);
		}else{

			currentCoorisRapid=false;
		}
		
		if (!startRapid&&currentCoorisRapid){
			
			if(currentTransition<transitionPaths->Count){

				//Console::WriteLine("rapid at coor ({0}{1}{2})",coor1[0],coor1[1],coor1[2]);
				startRapid=true;
				patchRapid(transitionPaths[currentTransition],apt,find,max_feed);
				currentTransition+=1;
			}
		
		}else if(currentCoorisRapid==false){
			debugStep = apt->GetCurrentWorkingstep();
			if (find->GetWorkingstepName2(debugStep)->Contains("WS 24")) {
				Console::WriteLine("WS {0}", find->GetWorkingstepName2(debugStep));
				debugOut(apt, find, actualFeed);

			}


			startRapid=false;
			apt->GoToXYZ("feed",coor2[0],coor2[1],coor2[2]);
		}
		/*if(currentTransition<transitionPaths->Count){
			Console::WriteLine(" coor ({0}{1}{2}) actual {3} max {4}",coor2[0],coor2[1],coor2[2],actualFeed,max_feed);
			}*/
		

	}
	apt->SaveAsModules(outName);
	Console::WriteLine("done");



}
//open a stpnc file locate the desired workplan via path 
//modify find and apt pointers to instance of of thier objects
//pass the reference the WPID of the desired workplan
//force the apt builder and the find object to work in inches
void Patcher::OpenFile(String^file,String^ desiredWorkPlanPath,String^newWorkPlanName,STEPNCLib::Finder ^%find,STEPNCLib::AptStepMaker^%apt, __int64% wpID){

//	apt = gcnew STEPNCLib::AptStepMaker();
	//find=gcnew STEPNCLib::Finder();
	find->Open238(file);
	apt->Open238(file);
	array<String^>^ nameList=desiredWorkPlanPath->Split('/');
	wpID= getWorkPlanByPath(nameList,find);
	apt->Inches();
	find->APIUnitsInch();
}
//Delete all workplans on directly nested under the main workplan
//except for wpid
void Patcher::DeleteBefore(__int64 wpid,STEPNCLib::Finder ^find,STEPNCLib::AptStepMaker^apt){
	__int64 mainWP=find->GetMainWorkplan();
	List<__int64> ^WPList=find->GetNestedExecutableAll(mainWP);
	for (int i=0;i< WPList->Count;i++){
		if(WPList[i]!=wpid){
			apt->DeleteExecutable(WPList[i]);
		}
	
	
	}


}


bool Patcher::rapidStarted(List<bool>^ state) {
	bool startIsRapid = state[0];
	bool  trans = startIsRapid;
	int rapidToFeed = 0;
	int feedToRapid = 0;
	int firstFeedToRapidPos;
	for (int i = 0; i < state->Count; i++) {
		if (trans!=state[i])
		{
			if (trans &&!state[i]) {
				rapidToFeed += 1;
			
			}
			else {
				feedToRapid += 1;
				firstFeedToRapidPos = i;
			}
			
		}
		trans = state[i];
	
	
	}
	if (feedToRapid == 1&&rapidToFeed==0) 
	{
		state->RemoveRange(0, firstFeedToRapidPos + 1);
		
		
		
		return true; }
	else {
		return false;
	}

}
bool Patcher::isFeedState(List<bool>^state,int count) {
	if (state->Count < count) {
		count = state->Count - 1;
	
	}
	for (int i = state->Count - 1; i > state->Count-count; i--) {
		if (state[i])
		{
			return false;
		}



	}
	return true;
}
void Patcher::createPatchedFile(String^ partFile,String^ WPpath,String^newFileName,String^newWorkPlan,String^ coor){
	STEPNCLib::AptStepMaker^ apt = gcnew AptStepMaker();
	STEPNCLib::Finder^find = gcnew Finder();
	bool newWS = true;
	bool newWP = true;
	
	__int64 oldWPID;
	RawData^ samples = gcnew RawData();
	OpenFile(partFile,WPpath,newWorkPlan,find,apt,oldWPID);
	array<String^>^ nameList=WPpath->Split('/');
	//get the workplan id 
	
	samples->changeSourceTxt(coor);
	samples->parse();
	samples->convertMMToInches();
	root=getAllExec(oldWPID,find,nullptr,0);
	
/////init

	long wp_id = find->GetMainWorkplan();
	Console::WriteLine("Main Workplan name " + find->GetExecutableName(wp_id));

	// add a new workplan at the end of the project
	apt->NestWorkplanAfter("Workplan for MTConnect results", find->GetWorkplanExecutableCount(wp_id) + 1, wp_id);
	// get the first path 
	ToolPath^ firstPath=root->firstPath();
	//init apt with first WS befor loop

	List<WP^>^ path = firstPath->getWS()->pathtoRoot();
	path->Reverse();
	for (int i = 0; i < path->Count; i++) {
		apt->NestWorkplan(path[i]->getName());
		path[i]->setCopyID(apt->GetCurrentWorkplan());

	}
	generateWSForToolPath(apt, find, firstPath->getWS(), nullptr);
	double feed = 0;
	array<double>^ coor1;
	array<double>^coor2;
	ToolPath^ currentTP = firstPath;
	bool firstRapid = true;
	bool prevCoorIsRapid = false;
	List<bool> ^pastRapids = gcnew List<bool>();
	for (int i = 0; i < samples->getSize()-1; i++) {
		coor1 = samples->getCoor(i);
		coor2 = samples->getCoor(i + 1);
		feed = samples->actualFeedRate(i,i+1);
		if (i == 0) { prevCoorIsRapid = false;
		pastRapids->Add(prevCoorIsRapid);
		}
		if ( feed >.6) {
			
			
			
			prevCoorIsRapid = true;
		}
		else {
			prevCoorIsRapid = false;
		}
		
		if (pastRapids->Count>5) {
			pastRapids->RemoveAt(0);
		}
			pastRapids->Add( prevCoorIsRapid);
		
	
			
			if (!prevCoorIsRapid&&isFeedState(pastRapids,3)) {
				
				apt->Feedrate(currentTP->getWS()->getMaxFeed());
				
				apt->SpindleSpeed(currentTP->getWS()->getMaxSpindle());
				apt->GoToXYZ("feed", coor2[0], coor2[1], coor2[2]);
			}else if(rapidStarted(pastRapids)) {

				currentTP = patchRapidToolPaths(apt, find, currentTP);



			}
		
		
		
	
		if (currentTP == nullptr) { break; }
	}
	apt->SaveAsModules("patchedWithWS");

}
                                                                                            //wpid of extension WP
                                                                                            //not of the copied WP

void Patcher::generateWSForToolPath(STEPNCLib::AptStepMaker^ apt, STEPNCLib::Finder^ find, WS^ ws ,ToolPath^tp) {
	apt->Workingstep(ws->getName());
	apt->LoadTool(ws->getToolId());
	ws->setCopyID(apt->GetCurrentWorkingstep());
	apt->Rapid();
	Console::WriteLine(" created mirror WS {0} ", ws->getName());
}

void Patcher::generateWPForToolPath(STEPNCLib::AptStepMaker^ apt,STEPNCLib::Finder^ find,ToolPath^newtool,ToolPath^old) {
	WS^ desiredWS = newtool->getWS();
	List<WP^> ^path = desiredWS->pathtoRoot();
	List<WP^>^oldPath = old->getWS()->pathtoRoot();
	path->Reverse();
	oldPath->Reverse();
	int max1 = path->Count;
	int max2 = oldPath->Count;
	int  countAbove;
	int positionInPath;
	WP^branchAfter = nullptr;
	for (int i = 0; i < max1&&i < max2; i++) {
		if (path[i]->getId() != oldPath[i]->getId()) {
		
			positionInPath = i- 1;
			branchAfter = path[i-1];
		
		}
		
	
	}
	if (branchAfter != nullptr) {
		countAbove = oldPath->Count - (positionInPath + 1);
	for (int i = 0; i < countAbove; i++) {
		apt->EndWorkplan();
	
	}
	}
	else {
		//there is no wp change
		if (max1 == max2) {
			return;
		}
		branchAfter = old->getWS()->getParent();
		positionInPath = oldPath->Count - 1;
	}
	for (int i = positionInPath + 1; i < max1; i++) {
		apt->NestWorkplan(path[i]->getName());
		path[i]->setCopyID(apt->GetCurrentWorkplan());
	}

}
void Patcher::rapidToPoints(STEPNCLib::AptStepMaker^apt,STEPNCLib::Finder ^ find, List<__int64>^STPNCpath) {
	List<double>^ coor = getAllPoints(STPNCpath, find);
	int totalPts = coor->Count / 3;
	apt->Rapid();
	for (int i = 0; i<totalPts; i++) {
		apt->GoToXYZ("rapid", coor[i * 3], coor[i * 3 + 1], coor[i * 3 + 2]);

	}

}
bool transitionToFeed(ToolPath ^tp1, ToolPath ^tp2) {
	if (tp1!=nullptr&&tp2!=nullptr&&tp1->rapid()&&tp2->rapid()) {
		return false;
	}
	else { return true; }

}
ToolPath^ Patcher::patchRapidToolPaths(STEPNCLib::AptStepMaker^ apt, STEPNCLib::Finder^ find, ToolPath^tp) {
	bool wp, ws;
	ToolPath ^tp1 = tp;
ToolPath ^tp2 = nullptr;
	List<__int64> ^STPNCpath = gcnew List<__int64>();
	while (tp1->rapid()==false) {
		tp1=tp1->nextPath(wp, ws);
		if (ws || wp) {
			Console::WriteLine(" Transitioned to a new WS without rapiding");
		}
		}
	/*
	if (tp1->getWS()->getIndex() >= 9) {
		apt->Workingstep("Rapiding");
	
	}*/
	List<__int64> ^temp = gcnew List<__int64>();
	temp->Add(tp1->getId());
	List<double> ^loc = getAllPoints(temp, find);
	//Console::WriteLine("rapid starting at WS {0} index {1}  coor {2} {3} {4}", tp1->getWS()->getName(), tp1->getIndex(), loc[0], loc[1], loc[2]);
	while (tp1->rapid())
	{
		STPNCpath->Add(tp1->getId());
		rapidToPoints(apt, find, STPNCpath);
		STPNCpath->Clear();
		ws = false;
		wp = false;
		tp2= tp1->nextPath(wp, ws);
		if (tp2 == nullptr) { return nullptr; }
		if (ws || wp) {

			//rapidToPoints(apt, find, STPNCpath);
			//STPNCpath->Clear();
			if (wp) {
				generateWPForToolPath(apt, find, tp2, tp1);

			}
			generateWSForToolPath(apt, find, tp2->getWS(), nullptr);

		}
		tp1 = tp2;
	}
	/*
	if (tp1->getWS()->getIndex() >= 9) {
		apt->Workingstep("Feed");

	}
	*/
	return tp1;
}
/*
ToolPath^ Patcher::patchRapidToolPaths(STEPNCLib::AptStepMaker^ apt, STEPNCLib::Finder^ find, ToolPath^tp) {
	bool prevRapid = false;
	bool currentRapid = false;
	bool first = true;
	ToolPath ^WStransisiton = nullptr;
	List<__int64> ^STPNCpath = gcnew List<__int64>();
	
	
	
	bool wp = false;
	bool ws = false;
	while (!(prevRapid==true&&currentRapid==false)||first) {
		if (first) {
			first = false;
			prevRapid = tp->rapid();

			if (prevRapid == true) { STPNCpath->Add(tp->getId()); 
			rapidToPoints(apt, find, STPNCpath);
			STPNCpath->Clear();
			}
		
		}
		else {
			prevRapid = currentRapid;
		
		}

		ws = false;
		wp = false;
		
		
		WStransisiton = tp->nextPath(wp, ws);
		if (WStransisiton == nullptr) {
			break;
		
		}
		
		if (ws || wp) {
			
			//rapidToPoints(apt, find, STPNCpath);
			//STPNCpath->Clear();
			if (wp) {
				generateWPForToolPath(apt, find, WStransisiton, tp);
				
			}
			generateWSForToolPath(apt, find, tp->getWS(), WStransisiton);
			
		}
		
		currentRapid = WStransisiton->rapid();
		if (currentRapid == true) {
			STPNCpath->Add(WStransisiton->getId());
			rapidToPoints(apt, find, STPNCpath);
			STPNCpath->Clear();
		}
		
		tp = WStransisiton;
		
	}
	
	
	apt->Feedrate(tp->getWS()->getMaxFeed());
	apt->SpindleSpeed(tp->getWS()->getMaxSpindle());
	return tp;
}
*/


double Patcher::actualFeedRate(array<double>^ coor1, array<double>^ coor2) {


	double dx = coor1[0] - coor2[0];
	double dy = coor1[1] - coor2[1];
	double dz = coor1[2] - coor2[2];

	double dist = Math::Sqrt(Math::Pow(dx, 2) + Math::Pow(dy, 2) + Math::Pow(dz, 2)) / coor2[3];

	//Console::WriteLine("dx {0} dy{1} dz {2}  deltat {3} speed{4}",dx,dy,dz,coor2[3],dist);

	return dist;

}