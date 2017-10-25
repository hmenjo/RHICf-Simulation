#include "G4ParticleTypes.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"

#include "RHICfDetectorConstruction.hh"
#include "RHICfCentralSD.hh"

//////////
/// Constructor and Destructor
RHICfCentralSD::RHICfCentralSD(G4String name): G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname=name);
}

RHICfCentralSD::~RHICfCentralSD()
{
}

//////////
/// Initialize
void RHICfCentralSD::Initialize(G4HCofThisEvent* HCTE)
{
  /// Create hit collection
  hitsColl = new CentralHitsCollection(SensitiveDetectorName, collectionName[0]); 
  
  /// Push H.C. to "Hit Collection of This Event"
  G4int hcid = GetCollectionID(0);
  HCTE->AddHitsCollection(hcid, hitsColl);
}

//////////
/// ProcessHits
G4bool RHICfCentralSD::ProcessHits(G4Step* astep, G4TouchableHistory*)
{

  /// Retrieve track information
  const G4StepPoint* preStepPoint  = astep->GetPreStepPoint();
  const G4StepPoint* postStepPoint = astep->GetPostStepPoint();

  G4double rad=3*CLHEP::cm;

  G4ThreeVector pos1=preStepPoint->GetPosition();
  if(pos1.mag()>rad) return true;

  G4ThreeVector pos2=postStepPoint->GetPosition();

  /// Calculate pos2
  G4bool isBoundary=false;
  if(pos2.mag()>=rad) {
    G4ThreeVector vec12=pos2-pos1;
    G4ThreeVector vec1C=-1*pos1;
    G4double theta1=(vec1C*vec12)/(vec1C.mag()*vec12.mag());
    G4double thetaG=asin(vec1C.mag()*sin(theta1)/rad);
    G4double dis=vec1C.mag()*cos(theta1)+rad*cos(thetaG);

    pos2=pos1+vec12.unit()*dis;

    isBoundary=true;
  }

  G4Track* track=astep->GetTrack();

  RHICfCentralHit* ahit=new RHICfCentralHit();
  ahit->SetTrackID(track->GetTrackID());
  ahit->SetMotherID(track->GetParentID());
  ahit->SetPDGCode(track->GetDynamicParticle()->GetPDGcode());
  ahit->SetEnergy(track->GetTotalEnergy());
  ahit->SetEkinetic(track->GetKineticEnergy());
  //  ahit->SetPosition1(preStepPoint->GetPosition());
  //  ahit->SetPosition2(postStepPoint->GetPosition());
  ahit->SetPosition1(pos1);
  ahit->SetPosition2(pos2);
  ahit->SetMomentum(postStepPoint->GetMomentum());
  /// For GENERATE mode
  if(!isBoundary && 
     preStepPoint->GetPhysicalVolume()->GetName()=="Vol-central_PV" &&
     (!(postStepPoint->GetPhysicalVolume()) ||
      postStepPoint->GetPhysicalVolume()!=preStepPoint->GetPhysicalVolume()))
    isBoundary=true;
  ahit->SetisBoundary(isBoundary);

  hitsColl->insert(ahit);

  /// Kill Tracks, they are already recorded.
  if(isBoundary) {
    if(track->GetKineticEnergy()<ecut) /// both in GeV
      track->SetTrackStatus(fStopAndKill);
    if(opposite) {
      if(fabs(postStepPoint->GetMomentum().eta())<etacut)
	track->SetTrackStatus(fStopAndKill);
    }else{
      if(postStepPoint->GetMomentum().eta()<etacut)
	track->SetTrackStatus(fStopAndKill);
    }
  }

  return true;
}

//////////
/// EndOfEvent
void RHICfCentralSD::EndOfEvent(G4HCofThisEvent* )
{
}

//////////
/// DrawAll
void RHICfCentralSD::DrawAll()
{
}

//////////
/// PrintAll
void RHICfCentralSD::PrintAll()
{
  hitsColl->PrintAllHits();
}

void RHICfCentralSD::SetCuts(G4double aetacut, G4double aecut, bool aopposite)
{
  etacut=aetacut;
  ecut=aecut;
  opposite=aopposite;
}
