#!/bin/csh

foreach file (`ls *.gdml`)
    echo ${file}
    cp ${file} tmp
#sed -e 's/\/home\/Users\/stak\/g4work\/rhicf/\/ccj\/u\/RHICf\/rhicf8/g' ${file} > tmp
#sed -e 's/http\:\/\/service-spi.web.cern.ch\/service-spi\/app\/releases\/GDML/\/ccj\/u\/RHICf\/rhicf8\/geometry\/GDML_3_1_4/g' ${file} > tmp
    mv tmp ${file}
end

# set vars=( `ls -1 *.gdml` )
# echo ${vars}

# foreach file ( ${vars} )
#   echo ${file}
# #
# #  
# #  
# end
