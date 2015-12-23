// Example STEP-NC Program using C++/CLI
// ========================================
//
// This sample C++ project calls the STEP-NC Machine Programming API
// using the .NET interface with C++/CLI calls.  The program creates a
// small machining program with a few toolpath moves.
//
// The project creates a command line program that writes test.stpnc.
// You can find more examples and other programming documentation at:
//
//	http://www.steptools.com/support/stepnc_docs/
//
#include "MtConnector.h"
using namespace System;

using namespace System::Xml;
using namespace System::Collections::Generic;

//array<int>^ CurrentPos(String ^xml)
List<array<double>^> ^  coordinates(String ^fileName,bool convert_to_inches){
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

void PullFromServer(MtConnector ^ mt){






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

void appendToFile(String^ file_name,String^ original,String^ new_file_name,bool convert_to_inches){
	List<array<double>^> ^coor_List= coordinates(file_name,convert_to_inches);
    // Create a trivial STEP-NC file
	Console::WriteLine("Got Position Data From File");
    STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	////////////////////////
   STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	   
	    //	    APT.Open238("new_hardmoldy.stpnc");
	    //	    Find.Open238("new_hardmoldy.stpnc");
	    apt->Open238(original);
	    apt->Open238(original);

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
	for (int i=0;i<coor_List->Count;i++){
		coor=coor_List[i];
		
		str=String::Format("point {0} ",i+1);
		//Console::WriteLine(str);
		//Console::WriteLine(" {0} {1} {2}",coor[0],coor[1],coor[2]);

		apt->GoToXYZ(str, coor[0], coor[1], coor[2]);
	
	}
    
	
	apt->SaveAsModules(new_file_name);
	Console::WriteLine("done");
	
   
}
long long getWorkPlanByPath(array<String^>^ nameList,STEPNCLib::Finder ^find){
	long long root=find->GetMainWorkplan();
	bool found=false;
	List<long long>^tempList=gcnew List<long long>();
	tempList->Add(root);
	for (int i=0;i<nameList->Length;i++){
		found=false;
		for each(long long x in tempList){
			if(find->IsProgramStructure(x)){
			if(find->GetExecutableName(x)==nameList[i]){
				Console::WriteLine(find->GetExecutableName(x));
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
	Console::WriteLine(find->GetExecutableName(root));
	return root;
}


long long getWorkPlanByName(String^ name,STEPNCLib::Finder ^find,long long root){
	
	if (find->IsProgramStructure(root)){
		List<long long>^nestedPlans =find->GetNestedExecutableAll (root);
		for each (long long x in nestedPlans){
			
				if(find->IsProgramStructure(x)){
					Console::WriteLine(find->GetExecutableName(x));
					if(name==find->GetExecutableName(x)){
						return x;
					}else{
						getWorkPlanByName(name,find,x);
					
					}
				
				}
		
		
		}
	
	}



}
long long GetWorkplan(String^name,STEPNCLib::AptStepMaker ^ apt, STEPNCLib::Finder ^find){


long long wp_id = find->GetMainWorkplan();
Console::WriteLine("main workplan id is {0}",wp_id);
String^ str=nullptr;
	
str=find->GetExecutableName(wp_id);
long long temp_id=wp_id;

	if(str!=nullptr){
		Console::WriteLine("Workplan name: {0}\n", str);
		List<long long>^nestedPlans =find->GetNestedExecutableAll (wp_id);
		for each (long long x in nestedPlans){
			str=find->GetExecutableName(x);
			if(str!=nullptr){
		Console::WriteLine("Workplan name: {0}\n", str);}
		
			List<long long>^nestedExecutables=find->GetNestedExecutableAll (x);
		for each (long long y in nestedExecutables){
			str=find->GetExecutableName(y);
			
		
	if(str!=nullptr){
		Console::WriteLine("Executable name: {0}\n", str);
	if(str==name){
		return y; 
		}
	}
		
	
		}
		}
		
	
	}

}
//recursively get all enabled worksteps
void getAllWorksteps(long long root,STEPNCLib::Finder ^find,List<long long>^ wsList){
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
List<array<double>^> ^  coordinatesWithTime(String ^fileName,bool convert_to_inches){
//create instance of the xml reader
	
	XmlDocument ^doc = gcnew XmlDocument();
	doc->Load(fileName);
	XmlNode ^root = doc->DocumentElement;
		
		XmlNamespaceManager ^nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		List<array<double>^> ^Lst = gcnew List<array<double>^>();
		String ^str;
		XmlNodeList^ pathList = root->SelectNodes("//base:PathPosition", nsmgr);
		DateTime  tempTime= DateTime();
		
		DateTime  tempTime2= DateTime();
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
					tempTime.Parse(temp->Attributes["timestamp"]->Value);
				coor[3]=0;

			}else{
			tempTime2.Parse(temp->Attributes["timestamp"]->Value);
			System::TimeSpan diff=(tempTime2.Subtract(tempTime));
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



//return list of transitions Feed->rapid,rapid to feed
//each array has two curve ids of (F,R)or (R,F) 
//third element of array indicates type of transition 0 F->R 1 R->F
List<array<long long>^> ^ GetRapidToolPaths(long long workplan, STEPNCLib::Finder ^find)
{
	List<long long> ^wsList=gcnew List<long long>();
	List<long long>^pathList=gcnew List<long long>();
	List<long long>^tempPathList=nullptr;
	List<array<long long>^> ^ rapidToolPaths=gcnew List<array<long long>^>();

	getAllWorksteps(workplan,find,wsList);
	for each(long long x in wsList ){
		
		pathList->AddRange(find->GetWorkingstepPathAll(x));
		
	}
	bool currentPathIsRapid=false;
	array<long long>^ tempTrans;
	if(pathList->Count>0){currentPathIsRapid=find->GetPathRapid(pathList[0]);}
	for( int i=1;i<pathList->Count;i++){
		if(find->GetPathRapid(i)!=currentPathIsRapid){
			tempTrans=gcnew array<long long>(3);
				tempTrans[0]=pathList[i-1];
			tempTrans[1]=pathList[i];
			if(currentPathIsRapid){
			tempTrans[1]=1;
			}else
			{tempTrans[1]=0;}
			rapidToolPaths->Add(tempTrans);
		
		}
		currentPathIsRapid=find->GetPathRapid(pathList[i]);
	
	
	}

	//GetPathPolylinePointAll/Count/Next()

	return rapidToolPaths;
}
double actualFeedRate(array<double>^ coor1,array<double>^ coor2){
	double dx=coor1[0]-coor2[0];
	double dy=coor1[1]-coor2[1];
	double dz=coor1[2]-coor2[2];
	Math::Pow(dx,2)+Math::Pow()

}
void appendPatchWorkPlan(String^file,String^ path,bool toInches ){
	//assume all units are inches or inches per sec
	double max_feed=12.3/60;//inches per sec
List<array<double>^> ^coorList=	coordinatesWithTime(file,toInches);

STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();
array<String^>^ nameList=path->Split('/');
	////////////////////////
   STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
   apt->Inches();


   apt->Open238 ("hardmoldy_ext.stpnc");
   find->Open238 ("hardmoldy_ext.stpnc");
	long long id= getWorkPlanByPath(nameList,find);
List<array<long long>^> ^transitionPaths= GetRapidToolPaths(id, find);

//begin the extended version of the stpnc file


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
	array<double>^ coor1;
	array<double>^coor2;
	for (int i=1;i<coorList->Count;i++){
		coor1=coorList[i-1];
		coor2=coorList[i];
		
		str=String::Format("point {0} ",i+1);
		//Console::WriteLine(str);
		//Console::WriteLine(" {0} {1} {2}",coor[0],coor[1],coor[2]);

		apt->GoToXYZ(str, coor[0], coor[1], coor[2]);
	
	}
    
	
	apt->SaveAsModules(new_file_name);
	Console::WriteLine("done");



}

int main(int argc, char * argv[])
{
	
	MtConnector ^ mt;
	mt=gcnew MtConnector();
	//mt->startPush("http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@id=\"Mp1LPathPos\"]");
	//http://agent.mtconnect.org/VMC-3Axis/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@category=\"SAMPLE\"]"
	//http://okuma-matata:5000/sample?count=1000&interval=10
	
	//mt->getRequest("http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@id=\"Mp1LPathPos\"]");
	//PullFromServer(mt);
  // appendToFile("iteration3_10_ms.xml","hardmoldy_ext.stpnc","moldy.stpnc",true);
	appendPatchWorkPlan("HARDMOLDY/Profiling/Boeing",true);
	String ^readIn =Console::ReadLine();
	return 0;
}

