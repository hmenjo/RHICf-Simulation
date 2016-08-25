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
#include <iostream>
#include <iomanip>

#include "RHICfEventAction.hh"
#include "RHICfRunAction.hh"
#include "RHICfGSOplateSD.hh"
#include "RHICfGSObarSD.hh"
#include "RHICfCentralSD.hh"
#include "RHICfForwardSD.hh"
#include "RHICfZDCSD.hh"
#include "RHICfSMDSD.hh"
#include "RHICfScinSD.hh"
#include "RHICfPrimaryGeneratorAction.hh"

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
  G4cout << "-------------------------------------" << G4endl;
  G4cout << ">>> Event " << evt->GetEventID() << G4endl;
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

  /// Central
  if(flag.check(bGENERATE)) {
    CentralContainer* centralCont=new CentralContainer();
    centralCont->Clear();

    static G4int idcentral=-1;
    if(idcentral<0) idcentral=SDManager->GetCollectionID("Central");
    CentralHitsCollection* CentralHC=(CentralHitsCollection*)HCTE->GetHC(idcentral);
    for(unsigned int i=0; i<CentralHC->GetSize(); i++) {
      Central* central=new Central();
      central->SetID((*CentralHC)[i]->GetTrackID());
      central->SetMotherID((*CentralHC)[i]->GetMotherID());
      central->SetPDGcode((*CentralHC)[i]->GetPDGCode());
      G4ThreeVector mom3=(*CentralHC)[i]->GetDirection();
      double ekin=(*CentralHC)[i]->GetEkinetic();
      TLorentzVector tmp4;
      tmp4.SetPx(ekin*mom3.x()/CLHEP::GeV);
      tmp4.SetPy(ekin*mom3.y()/CLHEP::GeV);
      tmp4.SetPz(ekin*mom3.z()/CLHEP::GeV);
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
    }
    simEvent->SetCentral(centralCont);
  }

  /// Forward
  if(flag.check(bTRANSPORT)) {
    G4cout << "Forward" << G4endl;
    ForwardContainer* forwardCont=new ForwardContainer();
    forwardCont->Clear();

    static G4int idforward=-1;
    if(idforward<0) idforward=SDManager->GetCollectionID("Forward");
    ForwardHitsCollection* ForwardHC=(ForwardHitsCollection*)HCTE->GetHC(idforward);
    for(unsigned int i=0; i<ForwardHC->GetSize(); i++) {
      Forward* forward=new Forward();
      forward->SetID((*ForwardHC)[i]->GetTrackID());
      if(flag.check(bGENERATE))
	forward->SetMotherID((*ForwardHC)[i]->GetMotherID());
      else{
	RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
	std::multimap<G4int, G4int> CentralID=genAction->GetCentralID();
	std::multimap<G4int, G4int>::iterator itr=CentralID.find((*ForwardHC)[i]->GetMotherID());
	forward->SetMotherID(itr->second);
      }
      forward->SetPDGcode((*ForwardHC)[i]->GetPDGCode());
      G4ThreeVector mom3=(*ForwardHC)[i]->GetDirection();
      double ekin=(*ForwardHC)[i]->GetEkinetic();
      TLorentzVector tmp4;
      tmp4.SetPx(ekin*mom3.x()/CLHEP::GeV);
      tmp4.SetPy(ekin*mom3.y()/CLHEP::GeV);
      tmp4.SetPz(ekin*mom3.z()/CLHEP::GeV);
      tmp4.SetE((*ForwardHC)[i]->GetEnergy()/CLHEP::GeV);
      forward->SetMomentum(tmp4);
      TVector3 tmp3;
      tmp3.SetX((*ForwardHC)[i]->GetPosition().x());
      tmp3.SetY((*ForwardHC)[i]->GetPosition().y());
      tmp3.SetZ((*ForwardHC)[i]->GetPosition().z());
      forward->SetPosition(tmp3);

      forward->SetIsBackground((*ForwardHC)[i]->GetisBackground());

      G4cout << tmp3.x() << " " << tmp3.y() << G4endl;

      forwardCont->Push_back(forward);
    }
    simEvent->SetForward(forwardCont);

    if(!flag.check(bGENERATE)) {
      RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
      simEvent->SetCentral(genAction->GetCentral());
    }
  }

  /// ZDC
  if(flag.check(bRESPONSE_ZDC)) {
    ZDCContainer* zdcCont=new ZDCContainer();
    zdcCont->Reset();

    static G4int idzdc=-1;
    if(idzdc<0) idzdc=SDManager->GetCollectionID("ZDC");
    ZDCHitsCollection* ZDCHC=(ZDCHitsCollection*)HCTE->GetHC(idzdc);
    G4cout << "ZDC " << G4endl;
    for(unsigned int i=0; i<ZDCHC->GetSize(); i++) {
      zdcCont->SetZDC((*ZDCHC)[i]->GetModule(),
      		      (*ZDCHC)[i]->GetEdep());
      zdcCont->SetNphoton((*ZDCHC)[i]->GetModule(),
			  (*ZDCHC)[i]->GetNphoton());
      if((*ZDCHC)[i]->GetEdep()>0)
	G4cout << (*ZDCHC)[i]->GetModule() << " " 
	       << (*ZDCHC)[i]->GetEdep() << " " 
	       << (*ZDCHC)[i]->GetNphoton() << " "
	       << G4endl;
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
      zdcCont->SetZDC((*ScinHC)[i]->GetScin(),
		      (*ScinHC)[i]->GetEdep());
    }

    runAction->SetSimEvent(simEvent);

    if(!flag.check(bTRANSPORT)) {
      RHICfPrimaryGeneratorAction* genAction=(RHICfPrimaryGeneratorAction*)runManager->GetUserPrimaryGeneratorAction();
      simEvent->SetCentral(genAction->GetCentral());
      simEvent->SetForward(genAction->GetForward());
    }
  }

  /// LHCf data
  if(flag.check(bRESPONSE_ARM1)) {
    MCDataContainer* mcdataCont=new MCDataContainer();
    mcdataCont->Reset();

    static G4int idplate=-1;
    if(idplate<0) idplate=SDManager->GetCollectionID("GSOplate");
    GSOplateHitsCollection* GSOplateHC=(GSOplateHitsCollection*)HCTE->GetHC(idplate);
    G4cout << "GSOplate " << G4endl;
    for(unsigned int i=0; i<GSOplateHC->GetSize(); i++) {
      mcdataCont->SetPlate((*GSOplateHC)[i]->GetTower(),
			   (*GSOplateHC)[i]->GetPlate(),
			   (*GSOplateHC)[i]->GetEdep());
      mcdataCont->SetPlateTruth((*GSOplateHC)[i]->GetTower(),
				(*GSOplateHC)[i]->GetPlate(),
				(*GSOplateHC)[i]->GetEdep_truth());
      if((*GSOplateHC)[i]->GetEdep()>0)
	G4cout << (*GSOplateHC)[i]->GetTower() << " "
	       << (*GSOplateHC)[i]->GetPlate() << " "
	       << (*GSOplateHC)[i]->GetEdep() << " "
	       << (*GSOplateHC)[i]->GetEdep_truth() << " "
	       << G4endl;

    }

    static G4int idbar=-1;
    if(idbar<0) idbar=SDManager->GetCollectionID("GSObar");
    GSObarHitsCollection* GSObarHC=(GSObarHitsCollection*)HCTE->GetHC(idbar);
    G4cout << "GSObar " << G4endl;
    for(unsigned int i=0; i<GSObarHC->GetSize(); i++) {
      mcdataCont->SetBarTruth((*GSObarHC)[i]->GetTower(),
			      (*GSObarHC)[i]->GetBelt(),
			      (*GSObarHC)[i]->GetXY(),
			      (*GSObarHC)[i]->GetBar(),
			      (*GSObarHC)[i]->GetEdep(),
			      (*GSObarHC)[i]->GetEdep_truth());
      if((*GSObarHC)[i]->GetEdep()>0)
	G4cout << (*GSObarHC)[i]->GetTower() << " "
	       << (*GSObarHC)[i]->GetBelt() << " "
	       << (*GSObarHC)[i]->GetXY() << " "
	       << (*GSObarHC)[i]->GetBar() << " "
	       << (*GSObarHC)[i]->GetEdep() << " "
	       << (*GSObarHC)[i]->GetEdep_truth() << " "
	       << G4endl;
    }

    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	for(int iunused=0; iunused<nunused; iunused++) {
	  mcdataCont->SetUnused(ibelt,ixy,iunused,0);
	}
      }
    }
    simEvent->SetMC(mcdataCont);
  }

  tevent->Fill();
}
