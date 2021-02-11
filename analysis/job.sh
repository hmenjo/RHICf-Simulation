#!/bin/csh

source ${HOME}/linux5.csh

set II=$argv[1]

cd /ccj/u/RHICf/rhicf10/analysis00

echo "Run ${II}"

set start=${II}00
set last=${II}99
set ifile=${start}


while ( ${ifile} <= ${last} )
    echo ${ifile}
    /usr/bin/rcp ccjnfs17:/ccj/w/data71/RHICf/data-mc/QGSJETII-04_FULL_POS1_reco/run${ifile}.root .

    ./analysis -o test${ifile}.root -i . -f ${ifile} -l ${ifile} -p 0 >& LOG/log${ifile}

    rm run${ifile}.root

    @ res = ${ifile} % 10
    echo "RES ${ifile} ${res}"
    if ( $res == 9 ) then
	echo "HADD ${ifile} ${res}"
        @ ii = ${ifile} / 10
	rm -f test${ii}.root
	hadd test${ii}.root test${ii}[0-9].root
	rm test${ii}[0-9].root
    endif

    @ ifile = ${ifile} + 1
end

rm -f test${II}.root
hadd test${II}.root test${II}[0-9].root
rm test${II}[0-9].root
