#!/bin/bash

ifile=$1
OUTDIR=$2
LOGDIR=$3

perl -pe "s/_nrun_/${ifile}/g;s/_out_/${OUTDIR}/g" input.dat > input${ifile}.dat
./rhicf input${ifile}.dat >& ${LOGDIR}/log${ifile}
rm -rf input${ifile}.dat