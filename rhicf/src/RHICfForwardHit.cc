#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4ParticleTypes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

#include "RHICfForwardHit.hh"

/// allocator
G4Allocator<RHICfForwardHit> RHICfForwardHitAllocator;

//////////
/// Constructor and Destructor
RHICfForwardHit::RHICfForwardHit()
{
  MotherID=0;
  PDGCode=0;
  Energy=0;
  Ekinetic=0;
  Position=G4ThreeVector();
  Momentum=G4ThreeVector();
  isBackground=false;
  isIntermediate=false;
}

RHICfForwardHit::~RHICfForwardHit()
{
}

//////////
/// Draw
void RHICfForwardHit::Draw()
{
}

//////////
/// Printc
void RHICfForwardHit::Print()
{
}
