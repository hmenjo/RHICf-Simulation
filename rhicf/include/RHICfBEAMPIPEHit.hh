#ifndef RHICFBEAMPIPEHIT_H
#define RHICFBEAMPIPEHIT_H 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4ios.hh"

#include "../../lib/BBCParticle.hpp"

class RHICfBEAMPIPEHit: public G4VHit
{
public:
  RHICfBEAMPIPEHit();
  RHICfBEAMPIPEHit(G4int aside, G4int abp, G4double aEdep, std::vector<BBCParticle*> aParticle);
  virtual ~RHICfBEAMPIPEHit();

  /// Copy constructor & assignment operator
  RHICfBEAMPIPEHit(const RHICfBEAMPIPEHit& right);
  const RHICfBEAMPIPEHit& operator=(const RHICfBEAMPIPEHit& right);
  G4int operator==(const RHICfBEAMPIPEHit& right) const;
 
  /// new/delete operators
  void* operator new(size_t);
  void operator delete(void* aHit);
  
  /// set/get functions
  void SetSide(G4int aside) {side=aside;};
  G4int GetSide() {return side;};
  void SetBEAMPIPE(G4int abp) {bp=abp;};
  G4int GetBEAMPIPE() {return bp;};
  void SetEdep(G4double aEdep) {Edep=aEdep;};
  G4double GetEdep() const {return Edep;};
  void SetHit(std::vector<BBCParticle*> aParticle) {Particle=aParticle;};
  std::vector<BBCParticle*> GetHit() const {return Particle;};

  /// Methods
  virtual void Draw();
  virtual void Print();

private:
  G4int side;
  G4int bp;
  G4double Edep;
  std::vector<BBCParticle*> Particle;
};

/////////
/// inline functions
inline RHICfBEAMPIPEHit::RHICfBEAMPIPEHit(const RHICfBEAMPIPEHit& right): G4VHit()
{
  side=right.side;
  bp=right.bp;
  Edep=right.Edep;
  Particle=right.Particle;
}

inline const RHICfBEAMPIPEHit& RHICfBEAMPIPEHit::operator=(const RHICfBEAMPIPEHit& right)
{
  side=right.side;
  bp=right.bp;
  Edep=right.Edep;
  Particle=right.Particle;

  return *this;
}

inline G4int RHICfBEAMPIPEHit::operator==(const RHICfBEAMPIPEHit& right) const 
{
   return (this==&right) ? 1 : 0; 
}

typedef G4THitsCollection<RHICfBEAMPIPEHit> BEAMPIPEHitsCollection;
extern G4Allocator<RHICfBEAMPIPEHit> RHICfBEAMPIPEHitAllocator; 

inline void* RHICfBEAMPIPEHit::operator new(size_t)
{
  void* aHit = (void*)RHICfBEAMPIPEHitAllocator.MallocSingle();
  return aHit;
}

inline void RHICfBEAMPIPEHit::operator delete(void* aHit)
{
  RHICfBEAMPIPEHitAllocator.FreeSingle((RHICfBEAMPIPEHit*) aHit);
}

#endif
