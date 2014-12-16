#include <iostream>
#include "LMBuilder.h"
#include "LMEngine.h"
using namespace std;
void LMUsage()
{
    cout << "Usage:\n\taslplm_train corpus_file model_file order discount_n prune_thre"<<endl;
    cout << "Example:\n\taslplm_train train.data gram.lm 3 7 1"<<endl;
    cout << "\ttrigram,GTdiscount for count 1-7,prune the gram which occurs only one time"<<endl;
}
int main(int args,char * argv[])
{
//	LMDB db;
	int order;
	int dis_n;
	int prune_n;
	    cout <<args <<endl;
	if(args!=6)
	{
	    LMUsage();
	    return 1;
	}
	order = argv[3][0]-'0';
	dis_n = argv[4][0]-'0';
	prune_n = argv[5][0]-'0';
	if(order <1 || order > 6)
	    cout << "order error" <<endl;
	LMBuilder testBdr(order);
	string fname(argv[1]);
	string outname(argv[2]);
	ofstream fout(outname.c_str());
	testBdr.BuildLM(fname,dis_n,prune_n);
	testBdr.outputDB(fout);
	return 1;
}
