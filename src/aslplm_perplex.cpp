#include <iostream>
#include "LMEngine.h"
using namespace std;
vector<string> splitLine(string l)
{	
	string str;
	vector<string> vWord;
	istringstream iss(l.c_str());
	while(iss >> str)
	{
		vWord.push_back(str);
	}
	return vWord;
}


inline string getNGram(vector<string>::iterator  iter, int order)
{

	string str;
	for (int i=0;i<order-1;i++)
	{
		str+=(iter+i)->c_str();
		str+=' ';
	}
	str+=(iter+order-1)->c_str();
	return str;
}

int main(int args,char * argv[])
{
	int order,ord;
	string line;
	order = argv[3][0]-'0';
	LMEngine testEng(order);
	cout << "start "<<endl;
	string lmfile(argv[1]);
	fstream senfile(argv[2]);
	cout << "start reading model file"<<endl;
	testEng.readARPALM(lmfile);
	cout << "finish reading model file"<<endl;
//	testEng.outputDB();
	while(getline(senfile , line))
	{
	    cout << line<<endl; 
	    double logP;
	    logP = 0;
	    vector<string> vWord;
	    vWord = splitLine(line);
	    if(vWord.size()<order)
		ord= vWord.size();
	    else
		ord = order;
	//    cout << "split"<<endl;
	    vector<string>::iterator  iter;

	    for(iter = vWord.begin(); (iter+(ord-1)) != vWord.end(); iter++)
	    {	  
	//	cout << "start lookup"<<endl;
	//	cout << *iter <<endl;
	//	cout <<getNGram( iter, ord)<<endl;
	        logP+=testEng.lookupLogP(getNGram( iter, order));
	//	cout << "finish lookup"<<endl;
		//cout<<iter->c_str()<<endl;
	    }

	    cout<<logP<<endl;
	}
	return 1;
}
