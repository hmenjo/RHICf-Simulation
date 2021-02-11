#!/bin/bash

BEAM=neutron200_beamtest1

start=1000000
maxfile=`expr $start + 1000`

OUTDIR=output_${BEAM}
LOGDIR=LOG_${BEAM}

mkdir ${LOGDIR}

ifile=$start

mkdir  ${OUTDIR} ${LOGDIR}

irun=0
while [ $ifile -lt $maxfile ]
  do
    if [ ${irun} -eq 15 ]; then
	bash job.sh ${ifile} ${OUTDIR} ${LOGDIR}
	irun=0
    else
	bash job.sh ${ifile} ${OUTDIR} ${LOGDIR} &
    fi
    irun=`expr $irun + 1`
    ifile=`expr $ifile + 1`
done