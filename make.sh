#!/bin/sh
cd src
g++ -o ../bin/aslplm_server aslplm_server.cpp LMEngine.cpp LMDB.cpp
g++ -o ../bin/aslplm_train aslplm_train.cpp LMBuilder.cpp LMDB.cpp
g++ -o ../bin/aslplm_perplex LMEngine.cpp aslplm_perplex.cpp LMDB.cpp
