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
//each array is the tool coordinate with delta t to reach the particular pt
//the first cooridinate has a time stamp of 0
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
List<List<long long>^> ^getWSPaths(long long workplan, STEPNCLib::Finder ^find){
	List<long long> ^wsList=gcnew List<long long>();
	
	List<List<long long>^> ^ pathList=gcnew List<List<long long>^>();
	getAllWorksteps(workplan,find,wsList);
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->Add(find->GetWorkingstepPathAll(x));
		
	}
	return pathList;
}

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

//return list of array of all rapid tool paths
//each array repesents a consecutive rapid movement
List<List<long long>^> ^ getAllRapidPaths(long long workplan, STEPNCLib::Finder ^find){
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
//return list of transitions Feed->rapid,rapid to feed
//each array has two curve ids of (F,R)or (R,F) 
//third element of array indicates type of transition 0 F->R 1 R->F
List<array<long long>^> ^ GetRapidToolPaths(long long workplan, STEPNCLib::Finder ^find)
{
	List<long long> ^wsList=gcnew List<long long>();
	List<long long>^pathList=gcnew List<long long>();
	//List<long long>^tempPathList=nullptr;
	List<array<long long>^> ^ rapidToolPaths=gcnew List<array<long long>^>();
	//get all enabled working steps form a specified work plan
	getAllWorksteps(workplan,find,wsList);
	Console::WriteLine("workplan {0}",workplan);
	//for each ws append all of the tool paths within that
	//ws to pathList
	for each(long long x in wsList ){
		Console::WriteLine("ws : {0}",find->GetWorkingstepName2(x));
		pathList->AddRange(find->GetWorkingstepPathAll(x));
		
	}
	bool currentPathIsRapid=false;
	array<long long>^ tempTrans;
	//for each consecutive pair of paths in path list, determine if there is a 
	//transition from feed->rapid or rapid->feed
	//if there is a transition record the pair of paths in rapidtoolpaths
	if(pathList->Count>0){currentPathIsRapid=find->GetPathRapid(pathList[0]);}
	//current path is index i-1
	for( int i=1;i<pathList->Count;i++){
		if(find->GetPathRapid(pathList[i])!=currentPathIsRapid){
			tempTrans=gcnew array<long long>(3);
				tempTrans[0]=pathList[i-1];
			tempTrans[1]=pathList[i];
			if(currentPathIsRapid){
			tempTrans[2]=1;
			}else
			{tempTrans[2]=0;}
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
	
	double dist= Math::Sqrt( Math::Pow(dx,2)+Math::Pow(dy,2)+Math::Pow(dz,2))/coor2[3];
	
		//Console::WriteLine("dx {0} dy{1} dz {2}  deltat {3} speed{4}",dx,dy,dz,coor2[3],dist);
	
	return dist;
}
List<double> ^getAllPoints(List<long long>^ paths,STEPNCLib::Finder ^find){
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
void patchRapid(List<long long>^ pathList,STEPNCLib::AptStepMaker ^ apt, STEPNCLib::Finder ^find,double max_feed){
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
array<double>^ vectorAdd(array<double>^vec1,array<double>^vec2){
	array<double>^ result=gcnew array<double>(vec1->Length);
	for (int i=0;i<vec1->Length;i++){
		result[i]=vec1[i]=vec2[2];
	
	
	}
	return result;
}

double vectorDot(array<double>^ vec1,array<double>^ vec2){
double sum=0;
for (int i=0;i<vec1->Length;i++){

	sum+=vec1[i]*vec2[i];
}

return sum;

}
array<double>^vectorNorm(array<double>^vec){
	return vectorDivide(vec,vectorMag(vec));


}
double vectorMag(array<double>^ vec){

	double sum=0;
	for (int i=0;i<vec->Length;i++){
	
		sum+=Math::Pow(vec[i],2.0);
	}
	return Math::Sqrt(sum);
}
array<double>^ vectorSubtract(array<double>^pt1, array<double>^pt2){
	if (pt1->Length!=pt2->Length){
		Console::WriteLine("cannot perform vecotr subtract , vectors are not the same lenght");
	}
	array<double> ^diff=gcnew array<double>(pt1->Length);
for (int i=0;i<pt1->Length;i++){
diff[i]=pt1[i]-pt2[i];

}
return diff;
}
array<double>^ vectorMult(array<double>^vec,double scalar){
	array<double> ^result =gcnew array<double>(vec->Length);
	for (int i=0;i<vec->Length;i++){
		result[i]=vec[i]*scalar;
		
	
	}

}
array<double>^ vector3dCrossProduct(array<double>^ vec1,array<double>^vec2){
	array<double>^ result=gcnew array<double>(3);
	result[0]=vec1[1]*vec2[2]-vec1[2]*vec2[1];
	result[1]=-1.0*(vec1[0]*vec2[2]-vec1[2]*vec2[0]);
	result[2]=vec1[0]*vec2[1]-vec1[1]*vec2[0];
	return result;
}
array<double>^ vectorDivide(array<double>^ vec,double scalar){
	array<double>^ result=gcnew array<double>(vec->Length);
	for (int i=0;i< result->Length;i++){
		result[i]=vec[i]/scalar;
	
	}
	return result;
}

//all arrays must have a length of 3
/*
geomalgorithms.com
*/

double pointToLineDistance(array<double>^linePt1,array<double>^linePt2,array<double>^ coor){
	
	
	//difference between the two points that make the line seg
	array<double>^diffSeg=vectorSubtract(linePt2,linePt1);
	//difference between the target point and the first point of the line seg
	array<double>^diffPointToCoor1=vectorSubtract(coor,linePt1);
	//difference between the target point and the second point in the line seg
	array<double>^diffPointToCoor2=vectorSubtract(coor,linePt2);
	double projection =vectorDot(diffSeg,diffPointToCoor1);
	if(projection<=0){
		return vectorMag(diffPointToCoor1);
	}
	double segDot=vectorDot(diffSeg,diffSeg);
	if(segDot<=projection){
		return vectorMag(diffPointToCoor2);
	}

	double percentSeg=projection/segDot;

	array<double>^ perpendicularPt=vectorAdd(linePt1,vectorMult(diffSeg,percentSeg));
	array<double>^ perpendicular=vectorSubtract(perpendicularPt,coor);
	return vectorMag(perpendicular);



}
//http://www.9math.com/book/projection-point-plane
double pointToTrimCurveDistance(long long curve,array<double>^coor, STEPNCLib::Finder ^find){
	
	bool direction;
//start coordinates,center point cooridinates , normal vector , projected vector	
double  x,y,z,ex,ey,ez,cx,cy,cz,r,nx,ny,nz,px,py,pz;

find->GetPathCurveStartPoint(curve,x,y,z);
	
find->GetPathArc(curve,ex,ey,ez,cx,cy,cz,r,direction);

find->GetPathArcAxis(curve,ex,ey,ez,nx,ny,nz);
array<double>^ curveCenter=gcnew array<double>{cx,cy,cz};
array<double> ^curveStart=gcnew array<double>{x,y,z};
array<double>^curveEnd =gcnew array<double>{ex,ey,ez};
array<double> ^originalNorm=gcnew array<double>{nx,ny,nz};
array<double>^vec1=vectorSubtract(curveStart,curveCenter);
array<double>^vec2=vectorSubtract(curveEnd,curveCenter);
array<double>^arcDirection=nullptr;
array<double>^curveStartToPointDirection=nullptr;
array<double>^pointToCurveEndDirection=nullptr;

array<double>^ unitNorm=vectorDivide(originalNorm,vectorMag(originalNorm));
//get point projected in the plane of the curve
double offset=vectorDot(unitNorm,curveStart);
double pointInPlaneFormula=vectorDot(unitNorm,coor)+offset;
double squaredUnitNorm=Math::Pow(vectorMag(unitNorm),2);
double quotient=pointInPlaneFormula/squaredUnitNorm;
px=coor[0]- unitNorm[0]*quotient;
py=coor[1]-unitNorm[1]*quotient;
pz=coor[2]-unitNorm[2]*quotient;

//dist(center and point in plane)-r = distance 
//corner case arc does not exist on that side then closest point on arc is
//the end points
//point projected onto the plane of the curve defined by center pt startpt and end pt
array<double>^ projectedPoint=gcnew array<double>{px,py,pz};

array<double>^ centerToPoint= vectorSubtract(coor,curveCenter);
array<double>^ unitCenterToPoint=vectorNorm(centerToPoint);
	double dist=vectorMag(centerToPoint)-r;



//get pt on circle that is closest to the target pt
//center+normalize(p-center)*R
array<double>^ circlePt=vectorAdd(curveCenter,vectorMult(unitCenterToPoint,r));


//determine whether corner case exists via cross product first

//just need the direction of curve sweep
if(!direction){
	arcDirection=vectorNorm(vector3dCrossProduct(vec1,vec2));
curveStartToPointDirection=vectorNorm(vector3dCrossProduct(vec1,centerToPoint));
pointToCurveEndDirection=vectorNorm(vector3dCrossProduct(centerToPoint,vec2));
}else{
	arcDirection= vectorNorm(vector3dCrossProduct(vec2,vec1));
	curveStartToPointDirection=vectorNorm(vector3dCrossProduct(centerToPoint,vec1));
	pointToCurveEndDirection=vectorNorm(vector3dCrossProduct(centerToPoint,vec2));
}


//compare cross products to see if they are in the same direction
if(vectorDot(arcDirection,curveStartToPointDirection)>0 &&vectorDot(pointToCurveEndDirection,arcDirection)>0){
//closest point is actually within the arc
	return dist;
}
//closest point is actually one of the end points

double distFromStartPt=vectorMag(vectorSubtract(coor,curveStart));
double distFromEndPt=vectorMag(vectorSubtract(coor,curveStart));
if(distFromStartPt>distFromEndPt){

	return distFromEndPt;
}else{return
	distFromStartPt;
}






}
double curveDist(long long pathIndex, long long curve,array<double>^coor, STEPNCLib::Finder ^find){
	array<double>^coor1=gcnew array<double>(3);
	coor1[0]=coor[0];
	coor1[1]=coor[1];
	coor1[2]=coor[2];
	String^ type=find->GetPathCurveType(curve);
	//assume 3 axis wiht tool axis being the z axis	
	if(type="trimmed_curve"){
	return	pointToTrimCurveDistance( curve,coor1, find);
	
	}else if(type=="polyline"){
		double x,y,z;
	
	find->GetPathCurveStartPoint(curve,x,y,z);
	
		array<double>^lnpt1=gcnew array<double>(3);
		array<double>^lnpt2=gcnew array<double>(3);
		array<double>^temp=nullptr;
	//curve is made of polylines
		double min=-1;
		double dist=0;
		List<double> ^pts=find->GetPathPolylinePointAll(curve);
		int totalPts=pts->Count;
		if(totalPts>=2){
		lnpt1[0]=pts[0];
		lnpt1[1]=pts[1];
		lnpt1[2]=pts[2];
		lnpt2[0]=pts[3];
		lnpt2[1]=pts[4];
		lnpt2[2]=pts[5];

		}
		for(int i=1; i<totalPts;i++){
			dist=pointToLineDistance(lnpt1,lnpt2,coor);
			if(dist<min ||min==-1){
			
			dist=min;
			}
	
		temp=lnpt1;
		lnpt1=lnpt2;
		
		lnpt2=temp;
		if(i+1<totalPts){
			lnpt2[0]=pts[(i+1)*3];
			lnpt2[1]=pts[(i+1)*3+1];
			lnpt2[2]=pts[(i+1)*3+2];
		}		
		
		}
		return min;
	
	}else{
		Console::WriteLine("invalid curve type {0}",type);
	return-1;
	}





}
//checks whether a coordinate belongs to the path or the next path
//return whether thier is a match or not and updates the indices if the current path was updated
//calculates the distance from each curve on the path from the pt

//corner cases
//	next path is a rapid->either the curve is the last curve of the consecutive feed paths or return false

bool matchToCurrentPath(List<List<long long>^>^ allPaths,long long %wsIndex,long long% pathIndex,double threshold, STEPNCLib::Finder ^find,array<double>^coor){
	

	List<long long >^ allCurves =find->GetPathCurveAll(pathIndex);
	for each( long long curve in allCurves){
	
		if(curveDist(pathIndex,curve,coor,find)<=threshold){
		return true;
		}
	
	}
return false;
}
//only need feed paths
void recordMatchingFeedPath(List<List<long long>^>^allPaths,long long& wsIndex,long long& pathIndex,array<double>^coor,STEPNCLib::Finder ^find,STEPNCLib::AptStepMaker ^ apt){

	List<long long>^ currentWS=allPaths[wsIndex];
	long long currentPath=currentWS[pathIndex];
	long long total=find->GetPathCurveCount(currentPath);
	bool nextWs=false;
	if(matchToCurrentPath(allPaths,wsIndex,pathIndex,.01,find,coor)){
	
		apt->GoToXYZ("point",coor[0],coor[1],coor[2]);
	
	}else{
	//corner cases next path starts next WS
	//next path is a rapid
	
	//get next path

		if(pathIndex+1<currentWS->Count){
			pathIndex+=1;
			currentPath=currentWS[pathIndex];
		}else if(wsIndex+1<allPaths->Count){
			wsIndex+=1;
			nextWs=true;
			currentPath=currentWS[pathIndex];
		
		}else{
		
			Console::WriteLine("No Match for Coor {0}{1}{2} with WS# {3}",coor[0],coor[1],coor[2],wsIndex);
			return;
		}
		//try the next path
		if(matchToCurrentPath(allPaths,wsIndex,pathIndex,.01,find,coor)){
			if(nextWs){
				String^ WSName=String::Format("index {0}",wsIndex);
				apt->Workingstep(WSName);
			}
		apt->GoToXYZ("point",coor[0],coor[1],coor[2]);
	
		}else{
		//assume machine stopped following designated path
			Console::WriteLine("No Match for Coor {0}{1}{2} with WS# {3}",coor[0],coor[1],coor[2],wsIndex);
			if(nextWs){
			wsIndex-=1;
				pathIndex=allPaths[wsIndex]->Count-1;
			}else{
				pathIndex-=1;
			}
		}

	}

}
void matchWSandPatch(String^partFile,String^coorFile,String^outName,String^ path,bool toInches ){
Console::WriteLine("input \n part {0}\n xml file{1} \n path to desired workplan {2}",partFile,coorFile,path);


	List<array<double>^> ^coorList=	coordinatesWithTime(coorFile,toInches);
	Console::WriteLine("Got coordinates list");
	STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	array<String^>^ nameList=path->Split('/');

	STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	apt->Inches();

	//open the part file
	apt->Open238 (partFile);

	find->Open238 (partFile);
	//get the workplan id 
	long long id= getWorkPlanByPath(nameList,find);
	//get all tool paths within a work plan that has a transition from feed to rapid or
	//rapid to feed
	//List<List<long long>^> ^rapidPaths= getAllRapidPaths(id, find);
	//List<List<long long>^>^feedPaths=getAllFeedPaths(id,find);
	List<long long> ^wsList=gcnew List<long long>();
	
	
	getAllWorksteps(id,find,wsList);
	long long wsIndex=0;
	long long pathIndex=0;
	long long curveindex=0;
	List<List<long long>^>^allPaths=getWSPaths(id,find);
	List<long long>^ currentWS=allPaths[0];
	long long currentPath=currentWS[0];
	long long currentCurve=find->GetPathAreaCurveNext(currentPath,0);
	bool coorIsRapid=false;
	//reference to determine if current corr represents a rapid
	double maxFeed=find->GetMachineParametersFeedrate(id);
	String ^units=find->GetMachineParamatersFeedrateUnit(id);
	Console::WriteLine("max feed rate is {0} {1}",maxFeed,units);
	array<double>^coor1=nullptr;
	array<double>^coor2=nullptr;
	bool preCoorIsRapid=false;
	for (int index =1;index<coorList->Count;index++){
		coor1=coorList[index-1];
		coor2=coorList[index];
		if(actualFeedRate(coor1,coor2)<=maxFeed){
			coorIsRapid=false;
		
		}else{
			coorIsRapid=true;
		}
		if(index==1){preCoorIsRapid=coorIsRapid;}
		if(!coorIsRapid){
			
			recordMatchingFeedPath(allPaths, wsIndex, pathIndex,coor2,find, apt)
		
		}else if(preCoorIsRapid!=coorIsRapid){
		
		//patch in all consecutive rapid paths


		
		}

	
	}
	

}


//given a stpnc file and a workplan within the stepnc file
// append a workplan with tool paths genereated from the xml data
//that contains tool position
//the tool paths represented in the xml file should be identical to the
//toolpaths of the ws in the identified workplan
void appendPatchWorkPlan(String^partFile,String^coorFile,String^outName,String^ path,bool toInches ){
	Console::WriteLine("input \n part {0}\n xml file{1} \n path to desired workplan {2}",partFile,coorFile,path);
	//assume all units are inches or inches per sec
	double max_feed=14.0/60;//inches per sec
	//get path position coordinates from the xml file with delta t in seconds at end
	List<array<double>^> ^coorList=	coordinatesWithTime(coorFile,toInches);
	Console::WriteLine("Got coordinates list");
	STEPNCLib::AptStepMaker ^ apt = gcnew STEPNCLib::AptStepMaker();

	array<String^>^ nameList=path->Split('/');

	STEPNCLib::Finder ^find = gcnew STEPNCLib::Finder();
	apt->Inches();

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
	Console::WriteLine("number of path coordinates {0}",coorList->Count);
	Console::WriteLine("number of transitions {0}",transitionPaths->Count);
	//determine if starting position is in rapid mode or feed
	if(coorList->Count>1){
		coor1=coorList[0];
		coor2=coorList[1];
	
		if (actualFeedRate(coor1,coor2)>max_feed){
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
	for (int i=1;i<coorList->Count;i++){
		coor1=coorList[i-1];
		coor2=coorList[i];

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

int main(int argc, char * argv[])
{
	
	MtConnector ^ mt;
	mt=gcnew MtConnector();
	//mt->startPush("http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@type=\"Path_Position\"]");
	//http://agent.mtconnect.org/VMC-3Axis/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@category=\"SAMPLE\"]"
	//http://okuma-matata:5000/sample?count=1000&interval=10
	//"http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@id=\"Mp1LPathPos\"]");
	mt->getRequest("http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@type=\"Path_Position\"]");
	//PullFromServer(mt);
 //appendToFile("sample_300ms.xml","hardmoldy_ext.stpnc","moldy_300.stpnc",true);
	//appendPatchWorkPlan("hardmoldy_ext.stpnc","sample_300ms.xml","patched_300.stpnc","HARDMOLDY/Profiling/Boeing",true);
	String ^readIn =Console::ReadLine();
	return 0;
}

