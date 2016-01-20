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
#include"stdafx.h"
#include "MtConnector.h"
#include "WP.h"
using namespace System;
using namespace STEPNCLib;
using namespace System::Xml;
using namespace System::Collections::Generic;
using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Collections::Generic;

array<double>^ vectorDivide(array<double>^ vec,double scalar){
	array<double>^ result=gcnew array<double>(vec->Length);
	for (int i=0;i< result->Length;i++){
		result[i]=vec[i]/scalar;
	
	}
	return result;
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
double vectorMag(array<double>^ vec){

	double sum=0;
	for (int i=0;i<vec->Length;i++){
	
		sum+=Math::Pow(vec[i],2.0);
	}
	return Math::Sqrt(sum);
}

array<double>^vectorNorm(array<double>^vec){
	return vectorDivide(vec,vectorMag(vec));


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
return result;
}
array<double>^ vector3dCrossProduct(array<double>^ vec1,array<double>^vec2){
	array<double>^ result=gcnew array<double>(3);
	result[0]=vec1[1]*vec2[2]-vec1[2]*vec2[1];
	result[1]=-1.0*(vec1[0]*vec2[2]-vec1[2]*vec2[0]);
	result[2]=vec1[0]*vec2[1]-vec1[1]*vec2[0];
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
/*

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
//maches path to coordiante using curve geometry
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
//tries to match coordinate to paths based on geometry
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
			
			recordMatchingFeedPath(allPaths, wsIndex, pathIndex,coor2,find, apt);
		
		}else if(preCoorIsRapid!=coorIsRapid){
		
		//patch in all consecutive rapid paths


		
		}

	
	}
	

}
*/
//recursively get all enabled worksteps and generate WP WS and TP objects

int main(int argc, char * argv[])
{
	Patcher ^pat;
	pat = gcnew Patcher();
	MtConnector ^ mt;
	mt=gcnew MtConnector();
	Console::WriteLine("created MTCOnnect");
	//mt->startPush("http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@type=\"Path_Position\"]");
	//http://agent.mtconnect.org/VMC-3Axis/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@category=\"SAMPLE\"]"
	//http://okuma-matata:5000/sample?count=1000&interval=10
	//"http://okuma-matata:5000/sample?count=1000&interval=10&path=//Path/DataItems/DataItem[@id=\"Mp1LPathPos\"]");
	//mt->getRequest("http://okuma-matata:5000/sample?count=1000&interval=1000&path=//Path/DataItems/DataItem[@type=\"PATH_POSITION\"]","DATA.txt");
	///DataItems/DataItem[@type=\"PathPosition\"]
	//PullFromServer(mt);
 //appendToFile("sample_300ms.xml","hardmoldy_ext.stpnc","moldy_300.stpnc",true);
	//appendPatchWorkPlan("hardmoldy_ext.stpnc","sample_300ms.xml","patched_300.stpnc","HARDMOLDY/Profiling/Boeing",true);
	
	pat->createPatchedFile("hardmoldy_imts_signed.stpnc", "HARDMOLDY", "patchedMoldy", "PatchWP", "Data.txt");
	//pat->appendToFile("Data.txt", "hardmoldy_imts_signed.stpnc", "patchedMoldy", true);
	//pat->appendPatchWorkPlan("hardmoldy_imts_signed.stpnc", "Data.txt", "moldySeperated", "HARDMOLDY/Profiling/Boeing", true);
	Console::WriteLine("done");
	String ^readIn =Console::ReadLine();
	return 0;
}

