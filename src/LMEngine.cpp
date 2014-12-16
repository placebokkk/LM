#include "LMEngine.h"
#include <stdlib.h>
bool LMEngine::readARPALM(string fname)
{
    ifstream lmf(fname.c_str());
    string sBuffer;
    int i = 1 ;
    int gramNum[10];
    double prob,bow;
    getline(lmf, sBuffer);
    if(sBuffer!="\\data\\")
	cout << "format error!"<<endl;
    for(i=1;i <= order;i++)
    {
	string num;
	size_t idx;
	getline(lmf, sBuffer);
	idx = sBuffer.rfind('=');
	idx +=2;
	num = sBuffer.substr(idx ,sBuffer.size()-idx);
	cout << num <<endl;
	gramNum[i] = atoi(num.c_str());
    }
    for(i=1;i<=order;i++)
    {
	int j=0;
	getline(lmf, sBuffer);
	for(j=0;j<gramNum[i];j++)
	{
	    size_t f_sep,s_sep,b_size;
	    string t_str,gram;

	    istringstream iss(sBuffer.c_str());
	    getline(lmf,sBuffer);
	    //insert a new gram here
	    //bsize = sBuffer.size();
	    //fsep = sBuffer.find('\t');
	    //ssep = sBuffer.rfind('\t');
	    //if(fsep == ssep)//no backoff weight
	//	prob = 
	    getline(iss,t_str,'\t');
	    prob = atof(t_str.c_str());
	    getline(iss,gram,'\t');
	    if(getline(iss,t_str,'\t'))
		bow = atof(t_str.c_str());
	    else
		bow = -0.398;
	    mDB[i-1]->insertItem(gram,prob,bow);
	    OOVProb = (i==1&&prob < OOVProb) ?prob:OOVProb;//use the lowest prob for est value of oov
	}
    }
    return 1;
}
double LMEngine::lookupLogP(string gram)
{
        int order;
	string str;
	vector<string> vWord;
	istringstream iss(gram.c_str());
	while(iss >> str)
	    vWord.push_back(str);
	order = vWord.size();
	return lookupLogP(gram,order);

	
}
double LMEngine::lookupLogP(string gram, int order)
{
	string lgram,history;
	size_t preidx,sufidx;
	double prob,bow;
	prob =	mDB[order-1]->lookupProb(gram);
	if(prob== LOGP_INF)
	{
	    if(order == 1 )
		return OOVProb;
	    else
	    {

	    sufidx = gram.find(' ')+1;
	    preidx = gram.rfind(' ');
	    lgram = gram.substr(sufidx,gram.size() - sufidx);
	    history =  gram.substr(0,preidx);
	    bow = mDB[order-2]->lookupWeight(history);//if no history count,log weight = 0.0
	    prob = lookupLogP(lgram,order-1);//if no history count,log weight = 0.0
	//    cout <<gram<< " weight: "<<bow<<"prob: "<<prob<<endl;
	    return bow+prob;//if no history count,log weight = 0.0
	    }
	}
	else
	    return prob;
	
}
bool  LMEngine::outputDB(ostream &fout)
{
	
	fout << "\\data\\"<<endl;
	for(int i=0;i<order;i++)
	{
	    fout <<"ngram " <<i+1<<" = "<<mDB[i]->getTypeNum()<<endl;
	}
	for(int i=0;i<order;i++)
	{
		fout <<"\\"<<i+1<<"-ngrams:" <<endl;
		mDB[i]->outputTableWithoutConv(fout);
	}
	return 1;
}
