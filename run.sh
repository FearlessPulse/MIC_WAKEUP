#!/bin/bash
cd ./samples/ivw_record_sample/
make
rm *.o
cd ../../bin
export LD_LIBRARY_PATH=../libs:$LD_LIBRARY_PATH
./ivw_record_sample
