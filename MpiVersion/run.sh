#!/usr/bin/env bash

TARGET=graphcoloring
NP=4

#for file in $( find graph_files/ -name "*.col" ); do
for file in $( cat  graph_files/filelist ); do
  echo =================================
  echo Testing $file
  echo ================================
  mpirun -np $NP $TARGET  $file $file.output
  tail -n 1 graph_files/$file.output
done

