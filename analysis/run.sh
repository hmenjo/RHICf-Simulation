#!/bin/csh

set start=200000
set last=200139
set ifile=$start

while ( ${ifile} <= ${last} )
    echo ${ifile}
    rm -f /ccj/u/RHICf/rhicf10/analysis00/out${ifile}
    rm -f /ccj/u/RHICf/rhicf10/analysis00/err${ifile}
    bsub -q short -o /ccj/u/RHICf/rhicf10/analysis00/out${ifile} -e /ccj/u/RHICf/rhicf10/analysis00/err${ifile} -s "/ccj/u/RHICf/rhicf10/analysis00/job.sh ${ifile}"
    @ ifile = ${ifile} + 1
end
