#!/bin/bash
cd bin
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./Spark-Ts-Tools &
cd ..
