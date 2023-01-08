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
vector<string> nextInstructionAddress;


/*static void traceCallIfBefore(){
	cout << "!!!CALL IF BEFORE!!!" << endl;
}*/


VOID traceMain(TRACE trace, VOID* v){
	stringstream insBuilder;
	stringstream tmpBuilder;

	ADDRINT bblhead = TRACE_Address(trace);
	ADDRINT nextInstructionAddress = TRACE_Address(trace);
	string cfIns = "";

	IMG img = IMG_FindByAddress(TRACE_Address(trace));
	RTN rtn = TRACE_Rtn(trace);

	if(!IMG_Valid(img) || !(IMG_IsMainExecutable(img))) return;

	//cout << TRACE_NumBbl(trace) << endl;
	//TRACE_InsertCall(trace, IPOINT_BEFORE, (AFUNPTR)traceCallIfBefore, IARG_END);
	//if (!TRACE_HasFallThrough(trace)) cout << "!!!FALL THROUGH!!!" << endl;

	RTN_Open(rtn);
	for( BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl) ){
		
		
		cout << " |" << endl;
		cout << " |" << endl;

		if(bblhead == nextInstructionAddress){
			cout << "true: " << cfIns << endl;

		}else{
			cout << "false: " << cfIns << endl; 
		}
		
		cout << " |" << endl;
		cout << " v" << endl;

		bblhead = BBL_Address(bbl);
		cout << "--- BBL Head Address: " << hex << bblhead << " ---" << endl;

		cout << RTN_Name(rtn) << endl;
		for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) ){
			cout << "\t" << " IP: " << hex << INS_Address(ins) << " IP+1: "  << INS_NextAddress(ins) << "\n\t\t << " << INS_Disassemble(ins) << endl;
			if(INS_IsControlFlow(ins)){
				if(!INS_IsIndirectControlFlow(ins)){

				}
				nextInstructionAddress = INS_NextAddress(ins);
				cfIns = INS_Disassemble(ins);
			}
		} 


	}
	RTN_Close(rtn);




	/*for( BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl) ){
		cout << "*****" << endl;

		ADDRINT headAddressBbl = BBL_Address(bbl);

		insBuilder.str(string());
		for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins)){
			cout << "\t" << hex << INS_Address(ins) << "\t" << INS_Disassemble(ins) << endl;
			insBuilder << "\t" << hex << INS_Address(ins) << "\t" << INS_Disassemble(ins) << "\n";
		}

		string tmp = insBuilder.str();
		int pos = tmp.rfind("0x")+2;
		nextInstructionAddress.push_back(tmp.substr(pos, pos+12));

		tmpBuilder << hex << headAddressBbl;

		bblAddress[tmpBuilder.str()] = insBuilder.str();
		insBuilder.str(std::string());
		tmpBuilder.str(std::string());
	}*/
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

	for(auto x : bblAddress /*nextInstructionAddress*/){
		//OutFile << x.second << endl;
		//OutFile << "--" << endl;
		//if(x.find(x.first) != 0){
		//}
	}

	OutFile.close();
}


int main(int argc, char* argv[]){

	PIN_InitSymbols();
	InitRegTables();
	if(PIN_Init(argc,argv)) return Usage();

	OutFile.open("data.out");

	IMG_AddInstrumentFunction(mainIMG, NULL);
	PIN_AddFiniFunction(Fini, NULL);

	PIN_StartProgram();

	return 0;
}
