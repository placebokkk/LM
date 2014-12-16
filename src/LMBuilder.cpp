#include "LMBuilder.h"

//-> count the file
//-> compute discount weight
//-> prune the low count
//-> compute the probability
//-> compute the backoff weight
bool LMBuilder::BuildLM(string fname,int dis,int thre)
{	
	cout<<"Start Building LM"<<endl;
	string sBuffer;
	ifstream fin(fname.c_str());
	bool ret = true;
	int i=0;
	while(getline(fin, sBuffer))
	{
		lines ++;
		ret&=handleLine("<BOS> "+sBuffer+" <EOS>");
		if(lines % 1000 ==0)
			cout<<"process "<<lines<<" lines"<<endl;
	}
	cout<<"Finish counting the number"<<endl;
	cout<<"Start calculate prob"<<endl;
	for(i = order-1;i>0;i--)
	{
		mDB[i]->discount(dis);
		mDB[i]->prune(thre);
	}

	for( i = order-1;i>0;i--)
	{
	cout<<"Start calculating "<<i+1<<" gram"<<" prob"<<endl;
	
	mDB[i]->calProb(*mDB[i-1]);
	mDB[i]->displayDCWeight();

//	mDB[i]->outputTable();
	}
	cout<<"Start calculating "<<"unigram"<<" prob"<<endl;

	mDB[0]->calProb(*mDB[0]);
	//mDB[0]->displayDCWeight();

	for(i = order-1;i>0;i--)
	{
		mDB[i]->computeBOW(*mDB[i-1]);
	}
//	mDB[i]->outputTable();
	return ret;
}

bool LMBuilder::handleLine(string l)
{
	vector<string> vWord;
	bool ret =true;
	int ord  = 0;
	vWord = splitLine(l);
	if(vWord.size()<order)
		ord= vWord.size();
	else
		ord = order;

	vector<string>::iterator  iter;
	for(int i=0;i<ord;i++)
	{
		for(iter = vWord.begin(); (iter+i) != vWord.end(); iter++)
		{	  
			ret&=handleWord( getNGram( iter, i),i );
		//cout<<iter->c_str()<<endl;
		}
	
	}
	return ret;
}


bool LMBuilder::handleWord(string w,int ord)
{
	return mDB[ord]->insertItem(w);

}

vector<string> LMBuilder::splitLine(string l)
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


inline string LMBuilder::getNGram(vector<string>::iterator  iter, int order)
{

	string str;
	for (int i=0;i<order;i++)
	{
		str+=(iter+i)->c_str();
		str+=' ';
	}
	str+=(iter+order)->c_str();
	return str;
}


bool  LMBuilder::outputDB(ostream &fout)
{
	
	fout << "\\data\\"<<endl;
	for(int i=0;i<order;i++)
	{
	    fout <<"ngram " <<i+1<<" = "<<mDB[i]->getTypeNum()<<endl;
	}
	for(int i=0;i<order;i++)
	{
		fout <<"\\"<<i+1<<"-ngrams:" <<endl;
		mDB[i]->outputTable(fout);
	}
	return 1;
}
