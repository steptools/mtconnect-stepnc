
#pragma once
using namespace System;
using namespace System::Net;
using namespace System::Xml;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
ref class MtConnector
{
private:
	String^ server;
	String^query1;
	String^query2;
	Boolean startedPush;
	XmlDocument ^doc ;
	WebClient ^wc;
	int getMessageSize(String^bound,StreamReader^ reader);
	array<wchar_t>^ getMessage(int size,StreamReader ^ reader);
	void printXMLData(array<wchar_t>^ buff,DateTime ^%,bool firstTime,StreamWriter^ writer);
public:
	MtConnector();
	array<double>^ getPos();
	void printData();
	Boolean startPush(String ^ str);
	void getRequest(String^ str,String^ dataLocation);

	
	void EventHandler(Object^ sender, OpenReadCompletedEventArgs^ e);


};
//delegate void OpenReadCompletedEventHandler (Object^ sender, OpenReadCompletedEventArgs^ e);
	
