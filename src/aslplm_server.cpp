#include <iostream>
#include "LMEngine.h"
using namespace std;

int main(int args,char * argv[])
{
	int order;
	string gram;
	order = argv[2][0]-'0';
	LMEngine testEng(order);
	cout << "start "<<endl;
	string fname(argv[1]);
	cout << "start reading file"<<endl;
	testEng.readARPALM(fname);
	cout << "finish reading file"<<endl;
//	testEng.outputDB();
	while(getline(cin , gram))
	{
	cout << gram <<' '<<testEng.lookupLogP(gram)<<endl;
	}
	return 1;
}
