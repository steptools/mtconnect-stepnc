#include "MtConnector.h"


MtConnector::MtConnector()
{
	startedPush=false;
	server= gcnew String("http://okuma-matata:5000");
	wc= gcnew WebClient();
	//wc->ResponseHeaders->Add("Content-Type","multipart/x-mixed-replace");
	
	
	//wc->Headers->Add("Content-Type","multipart/x-mixed-replace");
	wc->Headers->Add("Content-Type","text/xml");


	
	query1=gcnew String("/current?path=//Path/DataItems/DataItem[@category=\"SAMPLE\"]");
	//
	query2=gcnew String("/current?path=//Path");

	doc= gcnew XmlDocument();

}

void MtConnector::EventHandler(Object^ sender, OpenReadCompletedEventArgs^ e){
	try{
	Console::WriteLine ("fired event");

	Stream ^ data=e->Result;
	StreamReader^ reader= gcnew StreamReader(data);
	String^ str;
	while ((str = reader->ReadLine()) != nullptr) 
                {
                   // Console::WriteLine(str);
                }
	//Console::WriteLine (reader->ReadToEnd ());
	}

	catch(Exception ^e){
		Console::WriteLine(e);
	}



}
void MtConnector::getRequest(String^ str){
	
	UriBuilder^ build=gcnew UriBuilder(str);
	WebRequest^ request = WebRequest::Create(build->Uri);
	WebResponse ^ response=request->GetResponse();
	//response->Headers->Add("Content-Type","multipart/x-mixed-replace;boundary=--");
	//response->Headers->Add("Content-Type","text/xml");
	//response->Headers->Add("Transfer-Encoding","chunked");
	for each (String ^ key in response->Headers->AllKeys){
		Console::WriteLine(key);
			
		for each (String ^ val in response->Headers->GetValues(key)){
			Console::WriteLine(val);
		
		}
	}
	//response->Headers->Clear();
	//Console::WriteLine(response->Headers->AllKeys);
	Encoding^ encode = System::Text::Encoding::GetEncoding( "utf-8" );
	StreamReader^ reader= gcnew StreamReader(response->GetResponseStream(),encode);
	String^ line;
	String^ content=response->Headers->GetValues("Content-Type")[0];
	String^ bound="--"+content->Substring(content->IndexOf("=")+1);
	array<wchar_t>^ buff;
	int size=0;
	Console::WriteLine(bound);
	while(!(reader->EndOfStream)){
	
	size=getMessageSize(bound,reader);
	
	Console::WriteLine(size);
	//Console::WriteLine(line);
    buff=getMessage(size,reader);    
	printXMLData(buff);

	}
}
void MtConnector::printXMLData(array<wchar_t>^ buff){
	Console::WriteLine(buff);


}
int MtConnector::getMessageSize(String^bound,StreamReader^ reader){
int size=0;
int count=0;
String ^ line=nullptr;
line=reader->ReadLine();
	while((line!=nullptr)&& line!=bound){
		line=reader->ReadLine();
	}
	if (line!=nullptr){
		for (int i=0;i<3;i++){
			line=reader->ReadLine();
			
			if (i==1&& line!=nullptr){
				size=System::Convert::ToInt32((line->Substring(line->IndexOf(":")+1)));
			
			
			}
		
		}

			
	}
	 

	return size;


}
array<wchar_t>^ MtConnector::getMessage(int size,StreamReader ^ reader){
	array<wchar_t>^ buf=gcnew array<wchar_t>(size);
	reader->ReadBlock(buf,0,size);
	return buf;

}


Boolean MtConnector::startPush(String ^ str){
	try{
	wc->OpenReadCompleted+= gcnew OpenReadCompletedEventHandler(this,&MtConnector::EventHandler);
	UriBuilder^ build=gcnew UriBuilder(str);
	wc->OpenReadAsync(build->Uri);
	
	
	Console::WriteLine("started request with url :");
	Console::WriteLine(build->Uri);
	}
	catch(Exception^ e){
		Console::WriteLine(e);
	return false;
	}
	return true;
}

void MtConnector::printData(){
		String ^str;
	XmlNode ^root;
		XmlNamespaceManager ^nsmgr;
		XmlNodeList^ pathList;
try{
	doc->Load(server+query2);
	
		
		root = doc->DocumentElement;
		
		nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		
		
		pathList = root->SelectNodes("//base:DataItem", nsmgr);
		for each(XmlNode^  node in pathList){
		
			Console::WriteLine(node->InnerText);
		
		}
		}
	catch (Exception ^ e){
		Console::WriteLine(e);
		
	}


}
array<double>^  MtConnector::getPos(){
	String ^str;
	XmlNode ^root;
	array<double> ^coor;	
	XmlNamespaceManager ^nsmgr;
		XmlNodeList^ pathList;
		array<String^>^ arr;

		//	Console::WriteLine(server+request1);
		try{
	doc->Load(server+query1);
	
		
		root = doc->DocumentElement;
		
		nsmgr = gcnew XmlNamespaceManager(doc->NameTable);
		nsmgr->AddNamespace("base", root->NamespaceURI);

		
		
		pathList = root->SelectNodes("//base:PathPosition", nsmgr);
		str=pathList[0]->InnerText;
		}
	catch (Exception ^ e){
		Console::WriteLine(e);
		return nullptr;
	}

		
		if(str!="UNAVAILABLE"){
			//Console::WriteLine(str);
						arr=str->Split(' ');
			coor=gcnew array<double>(3);
			try
			{
			coor[0]=Convert::ToDouble(arr[0]);
			coor[1]=Convert::ToDouble(arr[1]);
			coor[2]=Convert::ToDouble(arr[2]);
			//Console::WriteLine(arr[0]);
			Console::WriteLine(coor[0]);
			return coor;
			}
			catch (Exception^ e){
			//Console::WriteLine(e);
			
			}


			


}
		return nullptr;
}