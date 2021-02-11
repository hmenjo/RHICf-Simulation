#!/bin/csh

alias emacs 'emacs -nw'

# For GCC
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/gmp-6.1.1/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/mpfr-3.1.4/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/mpc-1.0.3/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/isl-0.12.2/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/cloog-0.18.4/lib

setenv PATH ${PATH}:/ccj/w/r01/RHICf/linux5/local/gcc-4.9.2/bin
alias c++      'c++-4.9.2'
alias cpp      'cpp-4.9.2'
alias g++      'g++-4.9.2'
alias gcc      'gcc-4.9.2'
alias gcc      'gcc-4.9.2'
alias ar       'gcc-ar-4.9.2'
alias nm       'gcc-nm-4.9.2'
alias gfortran 'gfortran-4.9.2'
alias ranlib   'gcc-ranlib-4.9.2'
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/gcc-4.9.2/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/local/gcc-4.9.2/lib64

# cmake
alias cmake    '/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/cmake'
alias ccmake   '/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/ccmake'
alias ctest    '/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/ctest'
alias cpack    '/ccj/w/r01/RHICf/linux5/local/cmake-3.7.0-rc2/bin/cpack'

# ROOT
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/opt/gsl-1.16/lib
setenv ROOTSYS /ccj/w/r01/RHICf/linux5/opt/root_v5.34.36
setenv PATH ${PATH}:${ROOTSYS}/bin
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${ROOTSYS}/lib/root
alias root 'root -l'   

# Geant4
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/ccj/w/r01/RHICf/linux5/opt/xerces-c-3.1.4/lib
setenv PATH ${PATH}:/ccj/w/r01/RHICf/linux5/opt/xerces-c-3.1.4/bin
alias g4build 'cmake -DCMAKE_C_COMPILER=gcc-4.9.2 -DCMAKE_CXX_COMPILER=g++-4.9.2 -DGeant4_DIR=/ccj/w/r01/RHICf/linux5/opt/geant4.10.02.p02/lib64/Geant4-10.2.2'

setenv G4DATA /ccj/w/r01/RHICf/linux5/opt/geant4.10.02.p02/share/Geant4-10.2.2/data
setenv G4NEUTRONHPDATA ${G4DATA}/G4NDL4.5
setenv G4LEVELGAMMADATA ${G4DATA}/PhotonEvaporation3.2
setenv G4LEDATA ${G4DATA}/G4EMLOW6.48
setenv G4RADIOACTIVEDATA ${G4DATA}/RadiativeDecay4.3.1
setenv G4ABLADATA ${G4DATA}/G4ABLA3.0
setenv G4REALSURFACEDATA ${G4DATA}/RealSurface1.0
setenv G4SAIDXSDATA ${G4DATA}/G4SAIDDATA1.1
setenv G4NEUTRONXSDATA ${G4DATA}/G4NEUTRONXS1.4
setenv G4ENSDFSTATEDATA ${G4DATA}/G4ENSDFSTATE1.2.3

# Boost
setenv BOOST_ROOT /ccj/w/r01/RHICf/linux5/opt/boost_1_61_0
setenv BOOST_INCLUDE ${BOOST_ROOT}/include
setenv BOOST_LIBRARY ${BOOST_ROOT}/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${BOOST_LIBRARY}

# HepMC
setenv HEPMC_ROOT /ccj/w/r01/RHICf/linux5/opt/HepMC-2.06.09
setenv HEPMC_INCLUDE ${HEPMC_ROOT}/include
setenv HEPMC_LIBRARY ${HEPMC_ROOT}/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${HEPMC_LIBRARY}

# Pythia8
setenv PYTHIA8_ROOT /ccj/w/r01/RHICf/linux5/opt/pythia8219
setenv PYTHIA8_INCLUDE ${PYTHIA8_ROOT}/include
setenv PYTHIA8_LIBRARY ${PYTHIA8_ROOT}/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${PYTHIA8_LIBRARY}

# RHICf
setenv RHICF_TOP /ccj/u/satoken/RHICf/simulation/Photon/full/rhicf11
#setenv RHICF_TOP /ccj/u/RHICf/rhicf10/lib
setenv RHICF_LIB ${RHICF_TOP}/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${RHICF_LIB}

setenv RHICF_GENE /ccj/u/mhkim/RHICf/simulation/generator/pi0/rhicf10

setenv RHICF_TOP2 /ccj/u/RHICf/rhicf10/lib
setenv RHICF_LIB2 ${RHICF_TOP2}/lib

set QGS_TS=/ccj/w/data61/RHICf/DATA1/DATA1/SimulationDataForSuzukiCode/QGS/SATO_TS
set QGS_TS_OUT=/ccj/w/r01/satoken/con_simdata/qgs/middle


set CCJW=/ccj/w/r01/satoken
set CCJD=/ccj/w/data61/RHICf/DATA1/DATA1/satoken

setenv LHCf_LIBDIR /crhome/sato.kenta/RHICf/Analysis/Op2017/general
setenv PATH $LHCf_LIBDIR/bin:$PATH
