#ifndef LM_ENGINE
#define LM_ENGINE
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "LMDB.h"
using namespace std;
class LMDB;
class  LMEngine
{
    int order;
    double OOVProb;//the OOV prob 
    LMDB **mDB;//different order gram was store in separated database.
    
    public :
	LMEngine(int o):order(o)
	{	
	    OOVProb=0;
	    mDB = new LMDB*[o];
	    for(int i=0;i<o;i++)
		*(mDB+i) = new LMDB(i+1);
    	};
	bool readARPALM(string fname);
	bool outputDB(ostream &fout=cout);
	double lookupLogP(string gram);
	double lookupLogP(string gram, int order);
		
};
#endif
