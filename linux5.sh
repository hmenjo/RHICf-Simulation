#!/bin/bash

export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/gmp-6.1.1/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/mpfr-3.1.4/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/mpc-1.0.3/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/isl-0.12.2/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/cloog-0.18.4/lib:${LD_LIBRARY_PATH}

export PATH=/ccj/w/r01/RHICf/linux5/local/gcc-4.9.2/bin:${PATH}
alias c++='c++-4.9.2'
alias cpp='cpp-4.9.2'
alias g++='g++-4.9.2'
alias gcc='gcc-4.9.2'
alias gcc='gcc-4.9.2'
alias ar='gcc-ar-4.9.2'
alias nm='gcc-nm-4.9.2'
alias gfortran='gfortran-4.9.2'
alias ranlib='gcc-ranlib-4.9.2'
#export CXX=c++
#export C=gcc
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/gcc-4.9.2/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/local/gcc-4.9.2/lib64:${LD_LIBRARY_PATH}

# ROOT
export LD_LIBRARY_PATH=/ccj/w/r01/RHICf/linux5/opt/gsl-1.16/lib:${LD_LIBRARY_PATH}
export ROOTSYS=/ccj/w/r01/RHICf/linux5/opt/root_v5.34.36
#export ROOTSYS=/ccj/w/r01/satoken/src/root_v5.34.36_build2
export PATH=${ROOTSYS}/bin:${PATH}
source ${ROOTSYS}/bin/thisroot.sh
#alias root 'root -l'

# RHICf
#setenv RHICF_TOP /ccj/u/mhkim/RHICf/simulation/shower_leakage/rhicf10
#setenv RHICF_TOP /ccj/w//r01/satoken/simulation/test/shower_leakage/rhicf10
#setenv RHICF_LIB ${RHICF_TOP}/lib
#setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${RHICF_LIB}

# MC data
export PROJECT_SOURCE_DIR=/ccj/u/satoken/RHICf/convertor
export ROOT_INCLUDE_DIR=/ccj/w/r01/RHICf/linux5/opt/root_v5.34.36/include
export ROOT_LIBRARIES=/ccj/w/r01/RHICf/linux5/opt/root_v5.34.36/lib/root
#export ROOT_INCLUDE_DIR=
#export ROOT_LIBRARIES

export QGS_TS=/ccj/w/data61/RHICf/DATA1/DATA1/SimulationDataForSuzukiCode/QGS/SATO_TS
#export QGS_TS_OUT=/home/satoken/Simulation_DATA/MC_DATA_CONV/QGS/TS

# cmake
alias cmake=/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/cmake
alias ccmake=/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/ccmake
alias ctest=/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/ctest
alias cpack=/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/cpack

export CCJW=/ccj/w/r01/satoken
export CCJD=/ccj/w/data61/RHICf/DATA1/DATA1/satoken

export LHCf_LIBDIR=/ccj/u/satoken/RHICf/general
export PATH=$LHCf_LIBDIR/bin:$PATH