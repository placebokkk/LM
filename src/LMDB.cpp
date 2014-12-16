#include "LMDB.h"
#include "utils.h"
bool LMDB::insertItem(string w)
{
	if (order == 1)
		mTokenNum++;

	if (mTable.find(w)== mTable.end()) //new gram
	{
		mTable[w].count=0.0;
		mTable[w].bow = 0.0;
		mTypeNum++;
	}
	mTable[w].count+=1.0;
	return 1;
};
bool LMDB::insertItem(string w,double prob,double bow)
{

	mTable[w].count = prob;
	mTable[w].bow = bow;
	return 1;
}

lmdata LMDB::lookupItem(string w)
{
	return mTable[w];
};

bool LMDB::calProb(LMDB &db)
{
	
	map<string,lmdata>::iterator  iter;
	string str;
	string hisstr;
	double prob = 0;
	int count;
	for(iter = mTable.begin(); iter != mTable.end(); iter++)
	{
		str = iter->first;
		if (str.find(' ')==-1)//unigram
			prob = iter->second.count / mTokenNum ;
		else
		{
			hisstr = str.substr(0,str.rfind(' '));
			count = (int)iter->second.count;
			//calculate the prob c(W1..Wn)/c(W1..Wn-1)
			prob = getDCWeight(count) * count / db.lookupItem(hisstr).count;	
		}	
		iter->second.count = prob;
	}
	return 1;
};


//pruning the gram whose count is not larger than thre.
bool LMDB::prune(int thre)
{
	string str;
	int c;
	map<string,lmdata>::iterator  iter;
	for(iter = mTable.begin(); iter != mTable.end(); )
	{
		str = iter->first;
		c = (int)iter->second.count;
		iter++;
		if (c <= thre)
		{
			mTable.erase(str);
		}

	}
	
	return 1;
}

//Katz discount,the gram whose count is not larger than K will be discounted.
//c* = (c+1) * n(c+1)/n(c) where n(i) is the count of gram that has i count
//d = ((c*/c) - cTerm) / (1 - cTerm)
//where cTerm = (K+1)*n(K+1)/n(1)
bool LMDB::discount(int K)
{
	map<string,lmdata>::iterator  iter;
	int count=0;
	int coc[10];//count of count;
	int i=0;
	for (i=0;i<10;i++)
	{
		coc[i]=0;
		dc_w[i]=1;
	}
	for(iter = mTable.begin(); iter != mTable.end(); iter++)
	{
		count = (int)iter->second.count;
		if(count <=K+1)
		{
			coc[count]++;
		}			
	}
	double cTerm=(K+1)*coc[K+1]/coc[1];
	cout <<"term"<<cTerm<<endl;
	for (i=1;i<=K;i++)
	{
		cout << i+1<<" "<<coc[i+1]<<i<<" "<<coc[i]<<endl;
		if(coc[i]==0)
			dc_w[i]=1.0;
		else	
		dc_w[i]=(((i+1)*(double)coc[i+1])/(i*(double)coc[i]) - cTerm ) / (1.0 - cTerm);
	}
	return 1;
}

//compute the Backoff Weight
//db: lower order gram db
//bow(W1Wn-1)=(1 - Sum P(W1Wn) )/(1 - Sum P(W2Wn))
bool LMDB::computeBOW(LMDB &db)
{
	string gram,prefix,suffix;
	size_t preidx,sufidx;
	map<string,bow> *bowTable = new map<string,bow>();
	map<string,lmdata>::iterator  iter;
	double condp,bop;
	double numerator,denominator;
	for(iter = mTable.begin(); iter != mTable.end(); iter++)
	{
		gram = iter->first;	
		sufidx = gram.find(' ')+1;
		preidx = gram.rfind(' ');
		suffix = gram.substr(sufidx,gram.size() - sufidx);
		prefix =  gram.substr(0,preidx);
		
		condp = iter->second.count;
		bop = db.lookupItem(suffix).count;
		if (bowTable->find(prefix)==bowTable->end())
		{
			(*bowTable)[prefix].sum_cond_prob= condp;
			(*bowTable)[prefix].sum_bo_prob= bop;
		}
		else
		{
			(*bowTable)[prefix].sum_cond_prob+= condp;
			(*bowTable)[prefix].sum_bo_prob+= bop;
		}
		
	}
	map<string,bow>::iterator  boiter;
	for(boiter = bowTable->begin(); boiter != bowTable->end(); boiter++)
	{
		string str= boiter->first;
		numerator = (1 - (boiter->second).sum_cond_prob);
		denominator = (1 - (boiter->second).sum_bo_prob);
		if(numerator < 0)
			cerr << "bow numerator is negative!"<<endl;
		if(denominator < 0)
			cerr << "bow denominator is  negative!"<<endl;
		if(denominator ==0)
			db.mTable[str].bow = 1;
		else

		db.mTable[str].bow = numerator/denominator;
	}
	return 1;
}

bool LMDB::outputTable(ostream &fout)
{
	double lprob,lbow;
	map<string,lmdata>::iterator  iter;
	for(iter = mTable.begin(); iter != mTable.end(); iter++)
	{
		lprob = probToLogP(iter->second.count);
		if (iter->second.bow == 0.0 )
			fout<<lprob<<"\t"<<iter->first<<endl;
		else
		{
			lbow = probToLogP(iter->second.bow);
			fout<< lprob<<"\t"<<iter->first<<"\t"<< lbow<<endl; 
		}
	}
	return 1;
}

double LMDB::getDCWeight(int count)
{
	if(count > 5)
		return 1.0;
	return dc_w[count];
}


bool LMDB::displayDCWeight()
{
	cout << "Order " << order << " discount weight:"<<endl; 
	for(int i=0;i<=9;i++)
		cout<<" count "<<i<<" : "<<dc_w[i]<<endl;
	return 1;
}


double LMDB::lookupWeight(string gram)
{
    if (mTable.find(gram)== mTable.end()) // gram
	return 0.0;
    else
	return mTable[gram].bow;
}
double LMDB::lookupProb(string gram)
{
    if (mTable.find(gram)== mTable.end()) // gram
	return LOGP_INF;
    else
	return mTable[gram].count;

}
bool LMDB::outputTableWithoutConv(ostream &fout)
{
	double lprob,lbow;
	map<string,lmdata>::iterator  iter;
	for(iter = mTable.begin(); iter != mTable.end(); iter++)
	{
		lprob = iter->second.count;
		if (iter->second.bow == 0.0 )
			fout<<lprob<<"\t"<<iter->first<<endl;
		else
		{
			lbow = iter->second.bow;
			fout<< lprob<<"\t"<<iter->first<<"\t"<< lbow<<endl; 
		}
	}
	return 1;
}
