#include "G4Run.hh"
#include "G4RunManager.hh"

#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

#include <boost/random.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"
#include "HepMC/IO_GenEvent.h"

#include "RHICfRunAction.hh"
#include "RHICfPrimaryGeneratorAction.hh"
#include "HepMCG4AsciiReader.hh"

namespace pt=boost::posix_time;
namespace rn=boost::random;
using namespace Pythia8;
RHICfRunAction::RHICfRunAction(RHICfDetectorConstruction* det):G4UserRunAction(), fTimer(0), fDetector(det)
{
}

RHICfRunAction::~RHICfRunAction()
{
  fout->Close();

  delete trun;
  delete tevent;

  /// Timer stop
  fTimer->Stop();
  fElapsedTime+=fTimer->GetRealElapsed();
  G4cout << "Timer: " << fElapsedTime << " sec." << G4endl;
}

void RHICfRunAction::BeginOfRunAction(const G4Run* aRun)
{  
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  /// Timer start
  fTimer=new G4Timer();
  fTimer->Start();
  fElapsedTime=0.;

  G4RunManager* runManager = G4RunManager::GetRunManager();
  RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
  flag_detector=fDetector->GetDetectorFlag();

  if(flag_detector.check(bGENERATE)) {
    std::stringstream srun;
    srun << nrun;
    ftmp="Run"+srun.str()+".hepmc";
    SetHepMCFile(ftmp);

    /// Create HepMC file with generator
    if(genname=="EPOS_LHC"    || genname=="EPOS_1.99" ||
       genname=="QGSJETII-04" || genname=="QGSJETII-03" ||
       genname=="DPMJet3.0-6") {  /// Call CRMC with system function
      if(genname=="EPOS_LHC")         model="0";
      else if(genname=="EPOS_1.99")   model="1";
      else if(genname=="QGSJETII-04") model="7";
      else if(genname=="QGSJETII-03") model="11";
      else if(genname=="DPMJet3.0-6")  model="12";

      std::stringstream sevent;
      sevent << nevent;
      string fnevent=" -n "+sevent.str();
      string fproj=" -i 2212 -p 255 ";
      string ftarg=" -I 2212 -P -255 ";
      string fhepmc=" -o hepmc -f "+ftmp;
      string fmodel=" -m "+model;

      if(seed2<0) {
	pt::ptime date_ms=pt::microsec_clock::local_time();
	long ms=date_ms.time_of_day().total_milliseconds();
	rn::mt19937 rng( static_cast<unsigned long>(ms));
	rn::uniform_int_distribution<> seed2_crmc(0, 1000000000);
	rn::variate_generator< rn::mt19937, rn::uniform_int_distribution<> > mt(rng, seed2_crmc);
	seed2=mt();
      }
      std::stringstream sseed2;
      sseed2 << seed2;
      std::string fseed=" -s "+sseed2.str();
      std::string command=fcrmc+fnevent+fproj+ftarg+fhepmc+fmodel+fseed;

      system(command.c_str());
    }else if(genname=="Pythia8.215") {
      Pythia8::Pythia pythia;

      // Read in commands from external file.
      std::stringstream sevent;
      sevent << nevent;
      pythia.readString(("Main:numberOfEvents = "+sevent.str()).c_str());
      pythia.readString("Main:timesAllowErrors = 10");
      pythia.readString("Init:showChangedSettings = on");
      pythia.readString("Init:showChangedParticleData = off");
      pythia.readString("Next:numberCount = 10");
      pythia.readString("Next:numberShowInfo = 0");
      pythia.readString("Next:numberShowProcess = 0");
      pythia.readString("Next:numberShowEvent = 0");
      pythia.readString("Beams:frameType = 3");
      pythia.readString("Beams:idA = 2212");
      pythia.readString("Beams:idB = 2212");
      pythia.readString("Beams:pzA = 255.");
      pythia.readString("Beams:pzB = -255.");
      //    pythia.readString("SoftQCD:all = on");
      pythia.readString("SoftQCD:elastic = off");
      pythia.readString("SoftQCD:singleDiffractive = on");
      pythia.readString("SoftQCD:doubleDiffractive = on");
      pythia.readString("SoftQCD:centralDiffractive = on");
      pythia.readString("SoftQCD:nonDiffractive = on");
      pythia.readString("SoftQCD:inelastic = on");
      pythia.readString("Diffraction:PomFlux = 5");
      pythia.readString("MultipartonInteractions:pT0Ref = 1.");
      pythia.readString("MultipartonInteractions:ecmRef = 100.");
      pythia.readString("MultipartonInteractions:ecmPow = 0.16");
      pythia.readString("MultipartonInteractions:pTmin = 0.01");
      pythia.readString("MultipartonInteractions:bProfile = 3");
      //    pythia.readString("MultipartonInteractions:coreRadius = 0.4");
      //    pythia.readString("MultipartonInteractions:coreFraction = 0.5");
      pythia.readString("MultipartonInteractions:expPow = 1.");
      pythia.readString("ParticleDecays:limitTau0 = on");
      pythia.readString("ParticleDecays:tau0Max = 0");
      pythia.readString("Check:abortIfVeto = off");

      if(seed2==-1) {
	pt::ptime date_ms=pt::microsec_clock::local_time();
	long ms=date_ms.time_of_day().total_milliseconds();
	rn::mt19937 rng(static_cast<unsigned long>(ms));
	rn::uniform_int_distribution<> seed2_pythia(1, 900000000);
	rn::variate_generator< rn::mt19937, rn::uniform_int_distribution<> > mt(rng, seed2_pythia);
	seed2 =mt();
      }
      //      SetPythia8Seed(seed2);
      pythia.readString("Random:setSeed = on");
      ostringstream Seed;
      Seed<<"Random:seed = "<<seed2;
      pythia.readString(Seed.str());

      int nEvent=pythia.mode("Main:numberOfEvents");
      int nAbort=pythia.mode("Main:timesAllowErrors");

      /// HepMC::IO_GenEvent ascii_io("test.hepmc", std::ios::out); /// This does not work, I do not know why.
      std::ofstream output(ftmp.c_str());
      HepMC::IO_GenEvent ascii_io(output);

      pythia.init();

      int iAbort=0;
      for(int iEvent=0; iEvent<nEvent; ++iEvent) {
	if(!pythia.next()) {
	  if(pythia.info.atEndOfFile()) {
	    G4cout << " Aborted since reached end of Les Houches Event File" << G4endl;
	    break;
	  }
	  if(++iAbort < nAbort) continue;
	  G4cout << " Event generation aborted prematurely, owing to error!" << G4endl;
	  break;
	}
	HepMC::GenEvent* hepmcevt=new HepMC::GenEvent();
	HepMC::Pythia8ToHepMC ToHepMC;
	ToHepMC.fill_next_event(pythia, hepmcevt, iEvent);
	ascii_io << hepmcevt;
	//      ascii_io.print();
	delete hepmcevt;
      }
      pythia.stat();
    }

    genAction->SetGenerator("Generate");
    HepMCG4AsciiReader* hepMC=dynamic_cast<HepMCG4AsciiReader*>(genAction->GetGenerator());
    hepMC->SetFileName(ftmp);
    hepMC->Initialize();

    flag_merged=flag_detector;
  }
  runInfo=new RunInfo();
  simEvent=new RHICfSimEvent();

  /// Define ROOT File and TTree
  std::string froot=fdir;
  fout=new TFile(froot.c_str(), "recreate");
  trun=new TTree("RunInfo", "RunInfo");
  tevent=new TTree("EventInfo", "EventInfo");

  /// Define branches
  /// Run Tree
  trun->Branch("RunInfo", &runInfo);
  runInfo->SetRunNumber(nrun);
  runInfo->SetNumberOfEvents(nevent);
  runInfo->SetModel(genname);
  runInfo->SetSeed1(seed1);
  runInfo->SetSeed2(seed2);
  for(FLAGS ff=bGENERATE; ff<=bBEAMTEST; ff=static_cast<FLAGS>(ff+1)) 
    if(flag_original.check(ff)) flag_merged.set(ff);
  runInfo->SetFlag(flag_merged);
  ///
  tevent->Branch("SimEvent", &simEvent);
}

void RHICfRunAction::EndOfRunAction(const G4Run*)
{
  trun->Fill();

  trun->Print();
  trun->Write();
  tevent->Print();
  tevent->Write();

  std::stringstream srun;
  srun << nrun;
  std::string rmtmp="rm -f Run"+srun.str()+".hepmc";
  system(rmtmp.c_str());
}
