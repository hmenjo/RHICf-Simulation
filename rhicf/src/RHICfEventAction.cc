#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

#include "TROOT.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include <fstream>
#include <iomanip>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "RHICfEventAction.hh"
#include "RHICfRunAction.hh"
#include "RHICfGSOplateSD.hh"
#include "RHICfGSObarSD.hh"
#include "RHICfFCSD.hh"
#include "RHICfCentralSD.hh"
#include "RHICfForwardSD.hh"
#include "RHICfZDCSD.hh"
#include "RHICfSMDSD.hh"
#include "RHICfScinSD.hh"
#include "RHICfBBCSD.hh"
#include "RHICfPrimaryGeneratorAction.hh"

namespace pt=boost::posix_time;

//#define DUMPTRACKINFO

//////////
/// Constructor and Destructor
RHICfEventAction::RHICfEventAction()
{
}

RHICfEventAction::~RHICfEventAction()
{
}

//////////
/// BeginOfEventAction
void RHICfEventAction::BeginOfEventAction(const G4Event* evt)
{
  if(evt->GetEventID()%10==0) {
    G4cout << ">>> Event " << evt->GetEventID() << G4endl;
  }
}

//////////
/// EndOfEventAction
void RHICfEventAction::EndOfEventAction(const G4Event* evt)
{
  G4HCofThisEvent* HCTE=evt->GetHCofThisEvent();
  if(!HCTE) return;

  G4RunManager* runManager=G4RunManager::GetRunManager();
  RHICfRunAction* runAction=(RHICfRunAction*)runManager->GetUserRunAction();
  TFile* fout=runAction->GetOutputROOTFile();
  fout->cd();
  TTree* tevent=runAction->GetEventTree();

  Flag flag=runAction->GetFlag_Detector();
  Flag flag_original=runAction->GetFlag_Original();

  /// Get pointer to SDmanager
  G4SDManager* SDManager= G4SDManager::GetSDMpointer();

  RHICfSimEvent* simEvent=new RHICfSimEvent();
  tevent->SetBranchAddress("SimEvent", &simEvent);

  if(flag.check(bGENERATE)) {
    CentralContainer* centralCont=new CentralContainer();
    centralCont->Clear();

    RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
    centralCont->SetProcess(genAction->GetProcess());

    G4double sumE=0;
    static G4int idcentral=-1;
    if(idcentral<0) idcentral=SDManager->GetCollectionID("Central");
    CentralHitsCollection* CentralHC=(CentralHitsCollection*)HCTE->GetHC(idcentral);
    for(unsigned int i=0; i<CentralHC->GetSize(); i++) {
      Central* central=new Central();
      central->SetID((*CentralHC)[i]->GetTrackID());
      central->SetMotherID((*CentralHC)[i]->GetMotherID());
      central->SetPDGcode((*CentralHC)[i]->GetPDGCode());
      G4ThreeVector mom3=(*CentralHC)[i]->GetMomentum();
      //      double ekin=(*CentralHC)[i]->GetEkinetic();
      TLorentzVector tmp4;
      tmp4.SetPx(mom3.x()/CLHEP::GeV);
      tmp4.SetPy(mom3.y()/CLHEP::GeV);
      tmp4.SetPz(mom3.z()/CLHEP::GeV);
      tmp4.SetE((*CentralHC)[i]->GetEnergy()/CLHEP::GeV);
      central->SetMomentum(tmp4);
      TVector3 tmp3;
      tmp3.SetX((*CentralHC)[i]->GetPosition1().x());
      tmp3.SetY((*CentralHC)[i]->GetPosition1().y());
      tmp3.SetZ((*CentralHC)[i]->GetPosition1().z());
      central->SetPosition1(tmp3);
      tmp3.SetX((*CentralHC)[i]->GetPosition2().x());
      tmp3.SetY((*CentralHC)[i]->GetPosition2().y());
      tmp3.SetZ((*CentralHC)[i]->GetPosition2().z());
      central->SetPosition2(tmp3);
      central->SetBoundary((*CentralHC)[i]->GetisBoundary());
      centralCont->Push_back(central);

      if((*CentralHC)[i]->GetisBoundary())
	sumE+=(*CentralHC)[i]->GetEnergy()/CLHEP::GeV;
    }
    simEvent->SetCentral(centralCont);
  }

  /// Forward
  if(flag.check(bTRANSPORT)) {
    ForwardContainer* forwardCont=new ForwardContainer();
    forwardCont->Clear();

    static G4int idforward=-1;
    if(idforward<0) idforward=SDManager->GetCollectionID("Forward");
    ForwardHitsCollection* ForwardHC=(ForwardHitsCollection*)HCTE->GetHC(idforward);
    Forward* intermediate=new Forward();
    G4bool hasIntermediate=false;
    for(unsigned int i=0; i<ForwardHC->GetSize(); i++) {
      Forward* forward=new Forward();
      if(flag.check(bGENERATE))
	forward->SetMotherID((*ForwardHC)[i]->GetMotherID());
      else{
	RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
	std::multimap<G4int, G4int> CentralID=genAction->GetCentralID();
	std::multimap<G4int, G4int>::iterator itr=CentralID.find((*ForwardHC)[i]->GetMotherID());
	forward->SetMotherID(itr->second);
      }
      forward->SetPDGcode((*ForwardHC)[i]->GetPDGCode());
      G4ThreeVector mom3=(*ForwardHC)[i]->GetMomentum();
      //      double ekin=(*ForwardHC)[i]->GetEkinetic();
      TLorentzVector tmp4;
      tmp4.SetPx(mom3.x()/CLHEP::GeV);
      tmp4.SetPy(mom3.y()/CLHEP::GeV);
      tmp4.SetPz(mom3.z()/CLHEP::GeV);
      tmp4.SetE((*ForwardHC)[i]->GetEnergy()/CLHEP::GeV);
      forward->SetMomentum(tmp4);
      TVector3 tmp3;
      tmp3.SetX((*ForwardHC)[i]->GetPosition().x());
      tmp3.SetY((*ForwardHC)[i]->GetPosition().y());
      tmp3.SetZ((*ForwardHC)[i]->GetPosition().z());
      forward->SetPosition(tmp3);

      forward->SetIsBackground((*ForwardHC)[i]->GetisBackground());

      if(hasIntermediate) {
	forward->SetIntermediate(intermediate);
	intermediate=new Forward();
	hasIntermediate=false;
      }else{
	forward->SetIntermediate(NULL);
      }
      /// Fill into the next component
      if((*ForwardHC)[i]->GetisIntermediate()) {
	intermediate=forward;
	hasIntermediate=true;
      }else{
	forwardCont->Push_back(forward);
      }
    }

    simEvent->SetForward(forwardCont);

    if(!flag.check(bGENERATE)) {
      RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
      simEvent->SetCentral(genAction->GetCentral());
    }

    /// BBC
    BBCContainer* bbcCont=new BBCContainer();
    bbcCont->Reset();

    static G4int idbbc=-1;
    if(idbbc<0) idbbc=SDManager->GetCollectionID("BBC");
    BBCHitsCollection* BBCHC=(BBCHitsCollection*)HCTE->GetHC(idbbc);
    
    for(unsigned int i=0; i<BBCHC->GetSize(); i++) {
      BBC* bbc=new BBC();
      int iside=(*BBCHC)[i]->GetSide();
      int ibbc=(*BBCHC)[i]->GetBBC();
      bbc->SetEdep((*BBCHC)[i]->GetEdep());
      //      bbc->SetHit((*BBCHC)[i]->GetHit());
      bbcCont->SetBBC(iside, ibbc, bbc);
    }
    simEvent->SetBBC(bbcCont);

    if(!flag.check(bGENERATE)) {
      RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
      simEvent->SetCentral(genAction->GetCentral());
    }
  }

  /// BeamTest (Beam information)
  if(flag.check(bBEAMTEST)) {
    RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
    ForwardContainer* forwardCont=new ForwardContainer();
    forwardCont->Clear();

    Forward* forward=genAction->GetBeam();
    forwardCont->Push_back(forward);

    simEvent->SetForward(forwardCont);
  }

  bool phit=false;

  /// ZDC
  if(flag.check(bRESPONSE_ZDC)) {
    ZDCContainer* zdcCont=new ZDCContainer();
    zdcCont->Reset();

    static G4int idzdc=-1;
    if(idzdc<0) idzdc=SDManager->GetCollectionID("ZDC");
    ZDCHitsCollection* ZDCHC=(ZDCHitsCollection*)HCTE->GetHC(idzdc);
    for(unsigned int i=0; i<ZDCHC->GetSize(); i++) {
      //      zdcCont->SetZDC((*ZDCHC)[i]->GetModule(),
      //      		      (*ZDCHC)[i]->GetEdep());
      zdcCont->SetNphoton((*ZDCHC)[i]->GetModule(),
			  (*ZDCHC)[i]->GetNphoton());
      if((*ZDCHC)[i]->GetEdep()>0) phit=true;
    }

    static G4int idsmd=-1;
    if(idsmd<0) idsmd=SDManager->GetCollectionID("SMD");
    SMDHitsCollection* SMDHC=(SMDHitsCollection*)HCTE->GetHC(idsmd);
    for(unsigned int i=0; i<SMDHC->GetSize(); i++) {
      zdcCont->SetSMD((*SMDHC)[i]->GetXY(),
		      (*SMDHC)[i]->GetSMD(),
		      (*SMDHC)[i]->GetEdep());
    }

    static G4int idscin=-1;
    if(idscin<0) idscin=SDManager->GetCollectionID("Scin");
    ScinHitsCollection* ScinHC=(ScinHitsCollection*)HCTE->GetHC(idscin);
    for(unsigned int i=0; i<ScinHC->GetSize(); i++) {
      zdcCont->SetScin((*ScinHC)[i]->GetScin(),
		       (*ScinHC)[i]->GetEdep());
    }

    simEvent->SetZDC(zdcCont);

    if(!flag.check(bTRANSPORT) && !flag.check(bBEAMTEST)) {
      RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
      simEvent->SetCentral(genAction->GetCentral());
      simEvent->SetForward(genAction->GetForward());
      simEvent->SetBBC(genAction->GetBBC());
    }
  }

  /// LHCf data
  if(flag.check(bRESPONSE_ARM1)) {
    MCDataContainer* mcdataCont=new MCDataContainer();
    mcdataCont->Reset();

    static G4int idplate=-1;
    if(idplate<0) idplate=SDManager->GetCollectionID("GSOplate");
    GSOplateHitsCollection* GSOplateHC=(GSOplateHitsCollection*)HCTE->GetHC(idplate);
    for(unsigned int i=0; i<GSOplateHC->GetSize(); i++) {
      mcdataCont->SetPlate((*GSOplateHC)[i]->GetTower(),
			   (*GSOplateHC)[i]->GetPlate(),
			   (*GSOplateHC)[i]->GetEdep());
      //      mcdataCont->SetPlateTruth((*GSOplateHC)[i]->GetTower(),
      //				(*GSOplateHC)[i]->GetPlate(),
      //				(*GSOplateHC)[i]->GetEdep_truth());
      if((*GSOplateHC)[i]->GetEdep()>0) phit=true;
    }

    static G4int idbar=-1;
    if(idbar<0) idbar=SDManager->GetCollectionID("GSObar");
    GSObarHitsCollection* GSObarHC=(GSObarHitsCollection*)HCTE->GetHC(idbar);
    //    G4cout << "GSObar " << G4endl;
    for(unsigned int i=0; i<GSObarHC->GetSize(); i++) {
      //      mcdataCont->SetBarTruth((*GSObarHC)[i]->GetTower(),
      //			      (*GSObarHC)[i]->GetBelt(),
      //			      (*GSObarHC)[i]->GetXY(),
      //			      (*GSObarHC)[i]->GetBar(),
      //			      (*GSObarHC)[i]->GetEdep_truth());
      mcdataCont->SetBar((*GSObarHC)[i]->GetTower(),
			 (*GSObarHC)[i]->GetBelt(),
			 (*GSObarHC)[i]->GetXY(),
			 (*GSObarHC)[i]->GetBar(),
			 (*GSObarHC)[i]->GetEdep());
      //      if((*GSObarHC)[i]->GetEdep()>0)
      if(0)
	G4cout << (*GSObarHC)[i]->GetTower() << " "
	       << (*GSObarHC)[i]->GetBelt() << " "
	       << (*GSObarHC)[i]->GetXY() << " "
	       << (*GSObarHC)[i]->GetBar() << " "
	       << (*GSObarHC)[i]->GetEdep() << " "
	  //	       << (*GSObarHC)[i]->GetEdep_truth() << " "
	       << G4endl;
    }

    static G4int idfc=-1;
    if(idfc<0) idfc=SDManager->GetCollectionID("FC");
    FCHitsCollection* FCHC=(FCHitsCollection*)HCTE->GetHC(idfc);
    for(unsigned int i=0; i<FCHC->GetSize(); i++) {
      mcdataCont->SetFC((*FCHC)[i]->GetTower(),
			(*FCHC)[i]->GetEdep());
    }

    simEvent->SetMC(mcdataCont);
  }

#ifdef DUMPTRACKINFO
  if(phit) {
    pt::ptime date_ms=pt::microsec_clock::local_time();
    long ms=date_ms.time_of_day().total_milliseconds();

    char fname[256];
    sprintf(fname,"TrkInfo_%09ld.log",ms);
    std::ofstream of(fname, std::ios::out | std::ios::app );

    of << "E " << tevent->GetCurrentFile()->GetName() << " "
       << evt->GetEventID()  << G4endl;
    G4TrajectoryContainer* container=
      G4RunManager::GetRunManager()->GetCurrentEvent()->GetTrajectoryContainer();
    size_t nTraj=container->size();
    for(size_t i=0;i<nTraj;i++) {
      RHICfTrajectory* tr1=(RHICfTrajectory*)((*container)[i]);
      of << "T "
	 << tr1->GetTrackID() << " "
	 << tr1->GetParentID() << " "
	 << tr1->GetPDGEncoding() << " "
	 << tr1->GetInitialMomentum().x()/CLHEP::GeV << " "
	 << tr1->GetInitialMomentum().y()/CLHEP::GeV << " "
	 << tr1->GetInitialMomentum().z()/CLHEP::GeV << " "
	 << G4endl;
      for(int j=0; j<tr1->GetPointEntries(); j++) {
	of << "P " 
	   << tr1->GetPoint(j)->GetPosition().x() << " "
	   << tr1->GetPoint(j)->GetPosition().y() << " "
	   << tr1->GetPoint(j)->GetPosition().z() << " "
	   << G4endl;
      }
    }
  }
#endif


  runAction->SetSimEvent(simEvent);
  tevent->Fill();
}
