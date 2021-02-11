#include <iomanip>

#include "G4ParticleTypes.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "RHICfBEAMPIPESD.hh"
#include "RHICfDetectorConstruction.hh"


//////////
/// Constructor and Destructor
RHICfBEAMPIPESD::RHICfBEAMPIPESD(G4String name): G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname=name);
}

RHICfBEAMPIPESD::~RHICfBEAMPIPESD()
{
}

//////////
/// Initialize
void RHICfBEAMPIPESD::Initialize(G4HCofThisEvent* HCTE)
{
  /// Create hit collection
  hitsColl=new BEAMPIPEHitsCollection(SensitiveDetectorName, collectionName[0]); 
  
  /// Push H.C. to "Hit Collection of This Event"
  G4int hcid=GetCollectionID(0);
  HCTE->AddHitsCollection(hcid, hitsColl);
  
  /*
  /// Clear energy deposit and hit particles
  edep.clear();
  part.clear();
  edep.resize(nside);
  part.resize(nside);
  for(int iside=0; iside<nside; iside++) {
    edep[iside].resize(nbbc);
    part[iside].resize(nbbc);
    for(int ibbc=0; ibbc<nbbc; ibbc++) {
      part[iside][ibbc].clear();
    }
  }
  */
}

//////////
/// ProcessHits
G4bool RHICfBEAMPIPESD::ProcessHits(G4Step* astep, G4TouchableHistory*)
{
  /// Get step information from "PreStepPoint"
  const G4StepPoint* preStepPoint=astep->GetPreStepPoint();
  const G4StepPoint* postStepPoint=astep->GetPostStepPoint();
  const G4VProcess* CurrentProcess=preStepPoint->GetProcessDefinedStep();
  G4VPhysicalVolume* prePV=preStepPoint->GetPhysicalVolume();

  G4TouchableHistory* touchable=(G4TouchableHistory*)(preStepPoint->GetTouchable());
  G4String pv = prePV->GetName();
  //G4cerr << pv << G4endl;
  if (CurrentProcess != 0) {
    const G4String StepProcessName = CurrentProcess->GetProcessName();
    if(StepProcessName== "Transportation"){
      if((pv=="Vol-3in-pipe-beryllium_PV") || (pv=="Vol-3in-pipe-beryllium_PV") ||(pv=="Vol-3in-pipe-Iron_PV") ||(pv=="Vol-central_PV") ||(pv=="Vol-3in-flange1_PV") ||(pv=="Vol-3in-flange2_PV") ||(pv=="Vol-3in-flange3_PV") ||(pv=="Vol-3in-cone_PV") ||(pv=="Vol-bbc-tile-small_PV") ||(pv=="Vol-bbc-tile-large_PV") ||(pv=="Vol-BBC_PV") ||(pv=="Vol-5in-pipe_PV") ||(pv=="Vol-5in-flange1_PV") ||(pv=="Vol-5in-flange2_PV") ||(pv=="Vol-5in-flange3_PV") ||(pv=="Vol-5in-flange4_PV") ||(pv=="Vol-5in-flange5_PV") ||(pv=="Vol-5in-flange6_PV") ||(pv=="Vol-DXmagnet-5in-pipe1_PV") ||(pv=="Vol-DXmagnet-5in-pipe2_PV") ||(pv=="Vol-DXmagnet-5in-pipe3_PV") ||(pv=="Vol-DXmagnet-5in-cone_PV") ||(pv=="Vol-DXmagnet-5.5in-pipe_PV") ||(pv=="Vol-DXmagnet-5.5in-cone_PV") ||(pv=="Vol-DXmagnet-12in-pipe_PV") ||(pv=="Vol-DXmagnet-flange1_PV") ||(pv=="Vol-DXmagnet-flange2_PV") ||(pv=="Vol-DXvessel-front-flange_PV") ||(pv=="Vol-DXvessel-front_PV") ||(pv=="Vol-DXvessel-rear-flange_PV")||(pv=="Vol-DXvessel-rear_PV") ||(pv=="Vol-DXvessel-inner_PV") ||(pv=="Vol-DXvessel-outer_PV")||(pv=="Vol-DXvessel-cap_PV")||(pv=="Vol-DXcoil_PV")||(pv=="Vol-DXyoke_PV")||(pv=="Vol-Bellows_PV")||(pv=="Vol-Bellows-flange1_PV")||(pv=="Vol-Bellows-flange2_PV")){
    
	//G4cerr << "Hit " << G4endl;
	G4Track* track=astep->GetTrack();
	//G4cerr << "Hit " << G4endl;
	track->SetTrackStatus(fStopAndKill);
      }
    }
  }
  
  //G4cout << "kokomade" << G4endl;
 
  return true;
}

//////////
/// EndOfEvent
void RHICfBEAMPIPESD::EndOfEvent(G4HCofThisEvent* )
{
  /// Make hits and push them to "Hit Coleltion"
  G4cout << "BEAMPIPE EndOfEvent" << G4endl;
  for(int iside=0; iside<nside; iside++) {
    for(int ibbc=0; ibbc<nbbc; ibbc++) {
      //RHICfBEAMPIPEHit* ahit=new RHICfBEAMPIPEHit(iside, ibbc, edep[iside][ibbc], part[iside][ibbc]);
      //hitsColl->insert(ahit);
    }
  }
}

//////////
/// DrawAll
void RHICfBEAMPIPESD::DrawAll()
{
}

//////////
/// PrintAll
void RHICfBEAMPIPESD::PrintAll()
{
  hitsColl->PrintAllHits();
}
