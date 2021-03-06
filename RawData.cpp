#include"stdafx.h"

bool RawData::changeSourceTxt(String^ file) {
	if (file != nullptr) {
	
		sourceFile = file;
	}
	try {
		read = gcnew StreamReader(file);
	}
	catch (Exception ^e) {
		Console::WriteLine(e);
		return false;
	}
	type = TXTFILE;
	
}
bool RawData::parse() {
	coor->Clear();
	if (type == TXTFILE) {
		return parseTxt();
	}
}

void RawData::convertMMToInches() {
	array<double> ^coor = nullptr;
	for (int i = 0; i < getSize(); i++) {
		coor = getCoor(i);
		coor[0] = coor[0]  / 25.4;
		coor[1] = coor[1] / 25.4;
		coor[2] = coor[2] / 25.4;
	
	}


}
bool RawData::parseTxt() {
	String ^data = nullptr;
		array<String^>^ values;
		array<double>^ coorWithTimeDiff;
	while (!read->EndOfStream) {
		data = read->ReadLine();
		if (!data->Contains("#")&& !data->Contains("UNAVAILABLE")) {
			values = data->Split();
			coorWithTimeDiff = gcnew array<double>(4);
			coorWithTimeDiff[0]= Convert::ToDouble(values[0]);
			coorWithTimeDiff[1] = Convert::ToDouble(values[1]);
			coorWithTimeDiff[2] = Convert::ToDouble(values[2]);
			coorWithTimeDiff[3] = Convert::ToDouble(values[3]);
			coor->Add(coorWithTimeDiff);
		}
	
	}


	return true;
}

__int64 RawData::getSize() {
	return coor->Count;

}
array<double>^ RawData::getCoor(__int64 i) {

	if (i < getSize()) { return coor[i]; }
	else {

		return nullptr;
	}
}

	double RawData::actualFeedRate(__int64 i, __int64 j) {
		array<double>^ coor1 = getCoor(i);
		array<double>^ coor2 = getCoor(j);

		double dx = coor1[0] - coor2[0];
		double dy = coor1[1] - coor2[1];
		double dz = coor1[2] - coor2[2];

		double dist = Math::Sqrt(Math::Pow(dx, 2) + Math::Pow(dy, 2) + Math::Pow(dz, 2)) / coor2[3];

		//Console::WriteLine("dx {0} dy{1} dz {2}  deltat {3} speed{4}",dx,dy,dz,coor2[3],dist);

		return dist;
	}
	RawData::RawData() {
		read = nullptr;
		sourceFile = nullptr;
		coor = gcnew List<array<double>^>();
	
	}


