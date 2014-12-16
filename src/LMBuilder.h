#pragma warning (disable:4786)
#ifndef LM_BUILDER
#define LM_BUILDER
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "LMDB.h"
using namespace std;
class LMDB;
class  LMBuilder
{
	int order;
	int lines;
	LMDB **mDB;//different order gram was store in separated database.
    
	bool handleLine(string line);
	bool handleWord(string word,int ord);
	vector<string> splitLine(string line);
	string getNGram(vector<string>::iterator  iter, int order);
	public :
		LMBuilder(int o):order(o)
		{	
			lines = 0;
			mDB = new LMDB*[o];
			for(int i=0;i<o;i++)
				*(mDB+i) = new LMDB(i+1);
		};
		bool BuildLM(string fname,int p,int t);
		bool outputDB(ostream &fout=cout);
	
};

#endif
