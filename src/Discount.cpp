#include "Discount.h"

Good_turing::Good_turing(countT max_count)
    : _max_count(max_count), _coeffs((unsigned int)max_count + 1, 0.0)
{
    _coeffs[0] = 1.0;
}

double 
Good_turing::discount(countT count, countT total_count, countT vocab_size)
{
    if (count <= 0)
    {
	return 1.0;
    }
    else if (count > _max_count)
    {
	return 1.0;
    }
    else
    {
	return _coeffs[(unsigned int)count];
    }
}

bool 
Good_turing::estimate_para(LMDB& lm, int order)
{
    vector<countT> count_of_counts(/*(unsigned int )_max_count +*/ 2, 0);
    LMDB_itr itr(lm, order);
    lmdata* data;
    int i;
    while ((data = itr.next()) != NULL)
    {
	if (data->count <= _max_count + 1)
	    count_of_counts[(unsigned int)data->count]++;
    }

    double term = (_max_count + 1)*(double)
	count_of_counts[(unsigned int)_max_count + 1]/count_of_counts[1];

    for(i = 1; i <= _max_count; ++i)
    {
	double coeff;
	if (count_of_counts[i] == 0)
	{
	    coeff = 1.0;
	}
	else
	{
	    double co = (i + 1)*count_of_counts[i+1]
		    / (i*count_of_counts[i]);
	    coeff = (co - term) / (1.0 - term);
	    if (co > 1.0)
		coeff = 1.0;
	}
	_coeffs[i] = coeff;
    }
    return true;
}

/*Eric Ristad's Natural Law of Sucession
  The discount factor d is identical for all counts,
  d = (n(n + 1) + q(1-q))/
       (n^2 + n + 2q)
  where n is the total number of gram, q is the number of
  observed vocab. if q equals the vocab size, no discounting is performed.
 */
double
Natural_discount::discount(countT count, countT total_count, countT vocab_size)
{
    double n = total_count;
    double q = vocab_size;
    if (q == _vocab_size)
	return 1.0;
    else
	return (n*(n+1) + q*(1-q))/(n*(n+1)+2*q);
}

bool
Natural_discount::estimate_para(LMDB& lm, int order)
{
    _vocab_size = lm.vocab_size();
    //XXX: lm need compliment the function  vocab_size()
    return true;
}

double
KN_discount::discount(countT count, countT total_count,
			countT vocab_size)
{
    if (count <= 0)
	return 1.0;
    else if (count < _min_count)
	return 0.0;
    else
	return (count - discount1)/count;
}

double
KN_discount::lower_order_weight(countT total_count, countT  vocab_size, countT min2vocab, countT min3vocab)
{
    return (discount1*vocab_size/total_count);
}

bool
KN_discount::estimate_para(LMDB& lm, int order)
{
    unsigned int n1;
    unsigned int n2;

    LMDB_itr itr(lm, order);
    lmdata* data;

    while ((data = itr.next()) != NULL)
    {
	if ((unsigned int)(data->count) == 1)
	    n1 += 1;
	else if ((unsigned int)(data->count) == 2)
	    n2 += 1;
    }
    if (n1 == 0 || n2 == 0)
	return false;
    discount1 = n1/((double)n1 + 2*n2);
    return true;
}

/*
   Modified Kneser-Ney discounting
    as described in "an empirical study of smoothing
    techniques for languange modeling", 1998
 */
double 
modKN_discount::discount(countT count, countT total_count,
			countT vocab_size)
{
    if (count <= 0)
	return 1.0;
    else if (count < _min_count)
	return 0.0;
    else if ((unsigned int)count == 1)
	return (count - discount1)/count;
    else if ((unsigned int)count == 2)
	return (count - discount2)/count;
    else
	return (count - discount3plus)/count;
}

double
modKN_discount::lower_order_weight(countT total_count, countT vocab_size, countT min2vocab, countT min3vocab)
{
    double t;
    t = (discount1*(vocab_size - min2vocab) + 
	    discount2*(min2vocab - min3vocab) +
	    discount3plus*min3vocab) / total_count;
    return t;
}

bool
modKN_discount::estimate_para(LMDB& lm, int order)
{
    //tabulat count of counts
    unsigned int n1 = 0;
    unsigned int n2 = 0;
    unsigned int n3 = 0;
    unsigned int n4 = 0;

    LMDB_itr itr(lm, order);
    lmdata* data;

    while((data = itr.next()) != NULL)
    {
	unsigned int c = (unsigned int)(data->count);
	if (c == 1)
	    n1++;
	else if (c == 2)
	    n2++;
	else if (c == 3)
	    n3++;
	else if (c == 4)
	    n4++;
    }
    if (n1 == 0 || n2 == 0 ||
	    n3 == 0 || n4 == 0)
	return false;

    double Y = (double)n1/(n1 + 2*n2);

    discount1 = 1 - 2*Y*n2/n1;
    discount2 = 2 - 3*Y*n3/n2;
    discount3plus = 3 - 4*Y*n4/n3;

    if (discount1 < 0.0 || discount2 < 0.0 || discount3plus< 0.0)
	return false;

    return true;
}
