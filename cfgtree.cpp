#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

#include "pin.H"
using namespace std;


//KNOB<string> KnobHelp(KNOB_MODE_WRITEONCE, "pintool", "h", NULL, NULL);
ofstream OutFile;
map<string, string> bblAddress;
vector<string> pointToAddress;

VOID traceMain(TRACE trace, VOID* v){
	stringstream insBuilder;
	stringstream tmpBuilder;


	for( BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl) ){
		cout << "*****" << endl;

		ADDRINT headAddressBbl = BBL_Address(bbl);

		insBuilder.str(string());
		for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)){
			cout << "\t" << hex << INS_Address(ins) << "\t" << INS_Disassemble(ins) << endl;
			insBuilder << "\t" << hex << INS_Address(ins) << "\t" << INS_Disassemble(ins) << "\n";
		}

		string tmp = insBuilder.str();
		int pos = tmp.rfind("0x")+2;
		pointToAddress.push_back(tmp.substr(pos, pos+12));

		tmpBuilder << hex << headAddressBbl;

		bblAddress[tmpBuilder.str()] = insBuilder.str();
		insBuilder.str(std::string());
		tmpBuilder.str(std::string());
	}
}

VOID mainIMG(IMG img, VOID* v){
	if(!IMG_IsMainExecutable(img)) return;

	for(SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)){
		if(SEC_Name(sec).find(".text") != string::npos){
			TRACE_AddInstrumentFunction(traceMain, NULL);
			//cout << SEC_Name(sec) << endl;
		}
	}
}

INT32 Usage(){
	cerr << "wrong" << endl;
	return -1;
}

VOID Fini(INT32 code, VOID *v){
	//for(auto x : bblAddress){
	//	cout << x.first << endl;
	//}
	OutFile.setf(ios::showbase);

	for(auto x : bblAddress /*pointToAddress*/){
		OutFile << x.second << endl;
		OutFile << "--" << endl;
		//if(x.find(x.first) != 0){
		//}
	}

	OutFile.close();
}


int main(int argc, char* argv[]){

	PIN_InitSymbols();
	if(PIN_Init(argc,argv)) return Usage();

	OutFile.open("data.out");

	IMG_AddInstrumentFunction(mainIMG, NULL);
	PIN_AddFiniFunction(Fini, NULL);

	PIN_StartProgram();

	return 0;
}
