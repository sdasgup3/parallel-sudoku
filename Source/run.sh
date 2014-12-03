#!/usr/bin/env bash
#run the program locally.

TARGET=gc

# Enter the command line parameters here. --help to get a list of allowed
# options
PROGRAM_OPTIONS='--newGraph=yes --grain-size=1 --do-priority=false'
#set -x
runreg=false

while getopts ":r:f" opt; do
  case $opt in
    r)
      for file in $( find ../Tests/ -name "*json" ); do
        echo =================================
        echo Testing $file
        echo ================================
        cp $file latestGraph.json
        ./charmrun +p4 $TARGET --newGraph=no ++local
      done
      runreg=true  
      ;;
    f)
        echo =================================
        echo Testing $2
        echo ================================
        cp $2 latestGraph.json
        ./charmrun +p4 $TARGET --newGraph=no ++local
        runreg=true
      ;;
   \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

if [ $runreg = "false" ] ; then
./charmrun +p4 $TARGET $PROGRAM_OPTIONS ++local
fi
