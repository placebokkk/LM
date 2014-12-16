/*
 this lanhuage model database save the gram info in STL map container.the key is gram string and the value is lmdata structure.Notice that the member (double count) of lmdata is used to store the count of gram when we count the corpus.After discounting ,it's also used to store the prob when computing the prob. 
 */
#pragma warning (disable:4786)
#ifndef LM_DB
#define LM_DB
#include <map>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "utils.h"
#define LOGP_INF 10000.0
using namespace std;

typedef struct lmdata
{
double count;//also for prob
double bow;  //backoff weight
}lmdata;


class LMDB
{
	map<string,lmdata> mTable;
	int mTokenNum;
	int mTypeNum;
	//friend class LMDB;
public:
	LMDB(int o):order(o)
	{
	mTokenNum =0;
	mTypeNum =0;
	};
	bool insertItem(string w);
	bool insertItem(string w,double prob,double bow);

	lmdata lookupItem(string w);
	bool calProb(LMDB &db);
	
	bool prune(int thre);

	bool discount(int K);
	

	typedef struct bow
	{
		double sum_cond_prob;
		double sum_bo_prob;
	}bow;
	
	bool computeBOW(LMDB &db);

	double getDCWeight(int count);
	int getTypeNum(){return mTypeNum;};
	bool displayDCWeight();
	bool outputTable(ostream &fout);
	bool outputTableWithoutConv(ostream &fout);
	double lookupWeight(string gram);
	double lookupProb(string gram);


	double dc_w[10];//discount weight for Katz	
	int order;
};

#endif
