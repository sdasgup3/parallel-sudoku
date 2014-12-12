#!/usr/bin/env bash
#run the program locally.

TARGET=gc

# Enter the command line parameters here. --help to get a list of allowed
# options
NG='--newGraph=yes'
#PROGRAM_OPTIONS='--timeout=5 --grain-size=90 --do-priority=true --num-colors=5'
PROGRAM_OPTIONS='--timeout=5  --do-priority=true --do-subgraph=true'  
#set -x
runreg=false

#opt m: for running mpi cases

while getopts ":r:f:m" opt; do
  case $opt in
    r)
      for file in $( find ../Tests/ -name "*json" ); do
        echo =================================
        echo Testing $file
        echo ================================
        cp $file latestGraph.json
        ./charmrun +p4 $TARGET --newGraph=no $PROGRAM_OPTIONS ++local
      done
      runreg=true  
      ;;
    f)
        echo =================================
        echo Testing $2
        echo ================================
        cp $2 latestGraph.json
        ./charmrun +p4 $TARGET --newGraph=no $PROGRAM_OPTIONS ++local
        runreg=true
      ;;
    m)
      for file in $(cat ../MpiVersion/graph_files/filelist); do
        echo =================================
        echo Testing $file
        echo ================================
        ./charmrun +p4 $TARGET --newGraph=no $PROGRAM_OPTIONS --filename=../MpiVersion/graph_files/$file ++local
      done
        runreg=true  
      ;;
      
   \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

if [ $runreg = "false" ] ; then
./charmrun +p4 $TARGET $NG $PROGRAM_OPTIONS ++local
fi
