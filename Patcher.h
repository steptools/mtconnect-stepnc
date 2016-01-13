using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Collections::Generic;

#ifndef __PATCHER__
#define __PATCHER__
#include"WS.h"
#include"WP.h"
#include "ToolPath.h"
#include "MtConnector.h"

ref class Patcher{

public:
	WP^ getAllExec(__int64 root,STEPNCLib::Finder ^find,WP^plan,__int64 index);


	Patcher(){root=nullptr;}
	void PullFromServer(MtConnector ^ mt);\
//creates raw tool paths with no patching
	void appendToFile(String^ file_name,String^ original,String^ new_file_name,bool convert_to_inches);
private:
	WP^ root;
	String^ originalSTPNC;
	String^newSTPNC;

	//open and init finder and apt obect file
	//append a new workplan to the end of the workplans nested under the main work plan

	void OpenFile(String^file,String^ desiredWorkPlanPath,String^newWorkPlanName,STEPNCLib::Finder ^%find,STEPNCLib::AptStepMaker^%apt, __int64% wpID);
	//Delete all workplans and executables in the same root as wpid and before wpid
	void DeleteBefore(__int64 wpid);

	//returns coordinates in inches
	List<array<double>^> ^  coordinates(String ^fileName,bool convert_to_inches);

	long long getWorkPlanByPath(array<String^>^ nameList,STEPNCLib::Finder ^find);
	void getAllWorksteps(long long root,STEPNCLib::Finder ^find,List<long long>^ wsList);
	List<array<double>^> ^  coordinatesWithTime(String ^fileName,bool convert_to_inches);
	List<List<long long>^> ^getWSPaths(long long workplan, STEPNCLib::Finder ^find);
	List<List<long long>^> ^ getAllRapidPaths(long long workplan, STEPNCLib::Finder ^find);
	double actualFeedRate(array<double>^ coor1,array<double>^ coor2);
	List<double> ^getAllPoints(List<long long>^ paths,STEPNCLib::Finder ^find);
	void patchRapid(List<long long>^ pathList,STEPNCLib::AptStepMaker ^ apt, STEPNCLib::Finder ^find,double max_feed);
	void appendPatchWorkPlan(String^partFile,String^coorFile,String^outName,String^ path,bool toInches );
	void createPatchedFile(String^ partFile,String^ WPpath,String^newFileName,String^newWorkPlan,String coor,bool toInches);
	ToolPath^ firstPath(__int64 root);
};
#endif