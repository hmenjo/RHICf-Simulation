#ifndef RHICFTRAJECTORY_H
#define RHICFTRAJECTORY_H 1

#include <cstdlib>
#include <vector>

#include "G4VTrajectory.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"     
#include "globals.hh" 
#include "G4ParticleDefinition.hh" 
#include "G4TrajectoryPoint.hh"   
#include "G4Track.hh"
#include "G4Step.hh"

typedef std::vector<G4VTrajectoryPoint*> RHICfTrajectoryPointContainer;

class RHICfTrajectory : public G4VTrajectory
{
public:
  RHICfTrajectory(const G4Track* aTrack);
  virtual ~RHICfTrajectory();

  virtual void ShowTrajectory(std::ostream& os=G4cout) const;
  virtual void DrawTrajectory() const;
  virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
  virtual std::vector<G4AttValue>* CreateAttValues() const;
  virtual void AppendStep(const G4Step* aStep);
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  inline void* operator new(size_t);
  inline void  operator delete(void*);
  inline int operator == (const RHICfTrajectory& right) const
  {return (this==&right);} 

  virtual G4int GetTrackID() const { return fTrackID; }
  virtual G4int GetParentID() const { return fParentID; }
  virtual G4String GetParticleName() const { return fParticleName; }
  virtual G4double GetCharge() const { return fPDGCharge; }
  virtual G4int GetPDGEncoding() const { return fPDGEncoding; }
  virtual G4ThreeVector GetInitialMomentum() const { return fMomentum; }
  virtual int GetPointEntries() const { return fPositionRecord->size(); }
  virtual G4VTrajectoryPoint* GetPoint(G4int i) const { return (*fPositionRecord)[i]; }
  virtual G4String GetProduction() const { return fProduction; }
  virtual G4double GetMass() const { return fMass; }

private:
  RHICfTrajectoryPointContainer* fPositionRecord;
  G4int                          fTrackID;
  G4int                          fParentID;
  G4int                          fTrackStatus;
  G4ParticleDefinition*          fParticleDefinition;
  G4String                       fParticleName;
  G4double                       fPDGCharge;
  G4int                          fPDGEncoding;
  G4ThreeVector                  fMomentum;
  G4ThreeVector                  fVertexPosition;
  G4double                       fGlobalTime;
  G4String                       fProduction;
  G4double                       fMass;
};

extern G4ThreadLocal G4Allocator<RHICfTrajectory> * myTrajectoryAllocator;

inline void* RHICfTrajectory::operator new(size_t)
{
  if(!myTrajectoryAllocator)
    myTrajectoryAllocator = new G4Allocator<RHICfTrajectory>;
  return (void*)myTrajectoryAllocator->MallocSingle();
}

inline void RHICfTrajectory::operator delete(void* aTrajectory)
{
  myTrajectoryAllocator->FreeSingle((RHICfTrajectory*)aTrajectory);
}

#endif
