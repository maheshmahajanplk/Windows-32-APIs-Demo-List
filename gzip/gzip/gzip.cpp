// gzip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CkByteData.h"
//#pragma unmanaged
#include "CkGzip.h"
#include "CkMultiByteBase.h"
//#pragma managed
void ChilkatSample(void);

int main()
{
	try{
		ChilkatSample();
	}
	catch (...){
	}
	
	return 0;
}
void ChilkatSample(void)
{

	CkGzip gzip;
	
	bool success;
	CkByteData data;
	//  Any string unlocks the component for the 1st 30-days.
	success = gzip.UnlockComponent("Anything for 30-day trial");
	
	if (success != true) {
		//printf("%s\n", gzip.lastErrorText());
		return;
	}

	//  File-to-file GZip:
	//  Compress "hamlet.xml" to create "hamlet.xml.gz"
	char buffer[100] = { 0 };
	success = gzip.CompressString("hello tanvi", "utf-8",data);
	data.appendStr(buffer);

	
	if (success != true) {
		//printf("%s\n", gzip.lastErrorText());
		return;

	}
	//  File-to-file ungzip
	 // Decompress "hamlet.xml.gz" to create "hamletOut.xml"
	success = gzip.UncompressFile("wpasset_N_53216_20152207152812.txt.gz", "wpasset_N_53216_20152207152812.txt");
	if (success != true) {
		//printf("%s\n", gzip.lastErrorText());
		return;
	}
}