#ifndef DISCOUNT_H
#define DISCOUNT_H
#include <cassert>
#include <vector>
#include "LMDB.h"
using namespace std;

typedef double countT;

class LMDB_itr {
    public:
	LMDB_itr(LMDB& lm, int order)
	{ _itr = lm.mTable.begin(); _end_itr = lm.mTable.end();  _order = order;}
	lmdata* next() 
	{ 
	    lmdata *p;
	    if (_itr != _end_itr) 
	    {
		p = &_itr->second;
		_itr++;
		return p;
	    }
	    else
		return NULL;
	}
    private:
	map<string, lmdata>::iterator _itr;
	map<string, lmdata>::iterator _end_itr;
	int _order;
};

class Discount {
    public:
	Discount() { }
	virtual ~Discount() { }
	virtual double discount(countT count, countT total_count, countT vocab_size)
	{ return 1.0; }
	virtual double lower_order_weight(countT total_count,
			    countT vocab_size, countT min2vocab, countT min3vocab)
	{ return 0.0; }
	virtual bool estimate_para(LMDB& lm, int order)
	{ return true; }
};

class Good_turing : public Discount
{
    public:
	Good_turing(countT max_count);
	virtual ~Good_turing();
	double discount(countT count, countT total_count, countT vocab_size);
	bool estimate_para(LMDB& lm, int order);
    protected:
	vector<double> _coeffs;//discount coefficients
	countT _max_count;//when the count of a ngram >= max_count, do not do the discount
};

class Add_discount : public Discount
{
    public:
	Add_discount(double delta) : _delta(delta) { }
	double discount(countT count, countT total_count, countT vocab_size)
	{
	    double t = (count <= 0.0)? 1.0 : (1.0 + _delta/count)/
						    (1.0 + _vocab_size/total_count);
	    return t;
	}
	bool estimate_para(LMDB& lm, int order)
	{
	    _vocab_size = lm.vocab_size();//FIX: XXX need add this function
	    return true;
	}
    protected:
	double _delta;
	countT _vocab_size;
};

class Natural_discount : public Discount
{
    public:
	Natural_discount(countT min_count)
	    : _vocab_size(0), _min_count(min_count) {}
	double discount(countT count, countT total_count, countT vocab_size);
	bool estimate_para(LMDB& lm, int order);

    protected:
	countT _vocab_size;
	countT _min_count;
};

//Kneser-Ney discounting
class KN_discount : public Discount
{
    public:
	KN_discount(countT min_count)
	    : _min_count(min_count) { }
	virtual double discount(countT count, countT total_count, countT vocab_size);
	virtual double lower_order_weight(countT total_count, countT vocab_size,
					    countT min2vocab, countT min3vocab);
	virtual bool estimate_para(LMDB& lm, int order);
    protected:
	countT _min_count;// set to 0 if count below this
	double discount1;
};

class modKN_discount : public KN_discount
{
    public:
	modKN_discount(countT min_count)
	    : KN_discount(min_count), discount2(0.0), discount3plus(0.0)
	{ } 
	double discount(countT count, countT total_count, countT vocab_size);
	double lower_order_weight(countT total_count, countT vocab_size,
				    countT min2vocab, countT min3vocab);
	bool estimate_para(LMDB& lm, int order);
    protected:
	double discount2;
	double discount3plus;
};

#endif
