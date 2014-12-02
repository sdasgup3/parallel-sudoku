#!/usr/bin/env bash
#run the program locally.

TARGET=gc

# Enter the command line parameters here. --help to get a list of allowed
# options
PROGRAM_OPTIONS='--newGraph=yes --grain-size=5'
#set -x
runreg=false

while getopts ":r" opt; do
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
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

if [ $runreg = "false" ] ; then
./charmrun +p4 $TARGET $PROGRAM_OPTIONS ++local
fi
