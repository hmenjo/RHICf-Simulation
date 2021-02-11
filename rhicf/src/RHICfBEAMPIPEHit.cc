#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4ParticleTypes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "RHICfBEAMPIPEHit.hh"
#include <iomanip>

/// allocator
G4Allocator<RHICfBEAMPIPEHit> RHICfBEAMPIPEHitAllocator;

//////////
/// Constructor and Destructor
RHICfBEAMPIPEHit::RHICfBEAMPIPEHit(): side(-1), bp(-1), Edep(0.)
{
  side=-1;
  bp=-1;
  Edep=0;
  Particle.clear();
}

RHICfBEAMPIPEHit::RHICfBEAMPIPEHit(G4int aside, G4int abp, G4double aEdep, std::vector<BBCParticle*> aParticle)
{
  side=aside;
  bp=abp;
  Edep=aEdep;
  Particle=aParticle;
}

RHICfBEAMPIPEHit::~RHICfBEAMPIPEHit()
{
}

//////////
/// Draw
void RHICfBEAMPIPEHit::Draw()
{
}

//////////
/// Printc
void RHICfBEAMPIPEHit::Print()
{
}
