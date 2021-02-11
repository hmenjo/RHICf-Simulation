#ifndef RHICFBEAMPIPESD_H
#define RHICFBEAMPIPESD_H 1

#include "G4VSensitiveDetector.hh"

#include <vector>

#include "RHICfBEAMPIPEHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class RHICfBEAMPIPESD: public G4VSensitiveDetector
{
public:
  RHICfBEAMPIPESD(const G4String name);
  virtual ~RHICfBEAMPIPESD();

  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  virtual void Initialize(G4HCofThisEvent* HCTE);
  virtual void EndOfEvent(G4HCofThisEvent* HCTE);

  virtual void DrawAll();
  virtual void PrintAll(); 

private:
  BEAMPIPEHitsCollection*  hitsColl;

  std::vector<std::vector<double> > edep;
  std::vector< std::vector<std::vector<BBCParticle*> > > part;
};

#endif
