#!/usr/bin/env bash

TARGET=graphcoloring
NP=4

for file in $( find graph_files/ -name "*.col" ); do
  echo =================================
  echo Testing $file
  echo ================================
  mpirun -np $NP $TARGET  $file $file.output
done

