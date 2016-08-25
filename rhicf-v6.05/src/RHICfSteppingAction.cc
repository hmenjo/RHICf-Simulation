#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UnitsTable.hh"

#include "RHICfSteppingAction.hh"
#include "RHICfTrackInformation.hh"

//////////
/// Constructor and Destructor
RHICfSteppingAction::RHICfSteppingAction()
{
}

RHICfSteppingAction::~RHICfSteppingAction()
{
}

//////////
/// UserSteppingAction
void RHICfSteppingAction::UserSteppingAction(const G4Step* astep)
{
}
