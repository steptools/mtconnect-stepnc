#pragma once
using namespace System;
using namespace System;
using namespace System::Net;

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;
using namespace System::Xml;
using namespace System::Collections::Generic;
ref class RawData {
private:
	const String^ TXTFILE = "TXT";
	String^ sourceFile;
	StreamReader ^read ;
	String^type;
	List<array<double>^>^ coor;
public:

	bool changeSourceTxt(String^ file);
	//bool changeSourceXml(String^file);
	//bool changeSourceMTConnect(String^ request);

	bool parse();
	RawData() { read = nullptr;
	sourceFile = nullptr;
	}
	__int64 getSize();
	bool RawData::parseTxt();

	array<double>^getCoor(__int64 i);

	double actualFeedRate(__int64, __int64);


};