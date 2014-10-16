#!/usr/bin/env bash

export ROOT=${PWD}
export PYTHONHOME=${ROOT}/python_kit/intel64
export PYTHONPATH=${PYTHONPATH}:${ROOT}
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ROOT}/python_kit/intel64/lib
