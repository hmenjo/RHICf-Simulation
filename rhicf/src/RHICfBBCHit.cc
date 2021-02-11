#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4ParticleTypes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "../include/RHICfBBCHit.hh"
#include <iomanip>



/// allocator
G4Allocator<RHICfBBCHit> RHICfBBCHitAllocator;

//////////
/// Constructor and Destructor
RHICfBBCHit::RHICfBBCHit(): side(-1), bbc(-1), Edep(0.)
{
  side=-1;
  bbc=-1;
  Edep=0;
  Particle.clear();
}

RHICfBBCHit::RHICfBBCHit(G4int aside, G4int abbc, G4double aEdep, std::vector<BBCParticle*> aParticle)
{
  side=aside;
  bbc=abbc;
  Edep=aEdep;
  Particle=aParticle;
}

RHICfBBCHit::~RHICfBBCHit()
{
}

//////////
/// Draw
void RHICfBBCHit::Draw()
{
}

//////////
/// Print
void RHICfBBCHit::Print()
{
}
