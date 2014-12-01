#!/usr/bin/env bash
#run the program locally.

TARGET=gc

# Enter the command line parameters here. --help to get a list of allowed
# options
PROGRAM_OPTIONS=--newGraph=yes
set -x
./charmrun +p4 $TARGET $PROGRAM_OPTIONS ++local
