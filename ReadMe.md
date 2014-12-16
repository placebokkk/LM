These codes implement the basic LM training.
Katz smoothing.
Only support backoff.
Support the ARPA format.
The other smoothing method and interpolate will be implement later.
written by yangchao,niujianwei aslp@nwpu
2011.

Usage:
aslplm_train corpus_file model_file order discount_n prune_thre

an Example:
./bin/aslplm_train data/train.data data/trigram.lm 3 7 1
