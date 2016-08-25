#include "Forward.hpp"

Forward::Forward()
{
  id=0;
  motherid=0;
  pdgcode=0;
  momentum=TLorentzVector();
  position=TVector3();
  isbackground=true;
}

Forward::~Forward()
{;}

void Forward::SetID(int aid)
{ id=aid; }
void Forward::SetMotherID(int amotherid)
{ motherid=amotherid; }
void Forward::SetPDGcode(int apdgcode)
{ pdgcode=apdgcode; }
void Forward::SetMomentum(TLorentzVector amomentum)
{ momentum=amomentum; }
void Forward::SetPosition(TVector3 aposition)
{ position=aposition; }
void Forward::SetIsBackground(bool aisbackground)
{ isbackground=aisbackground; }

int Forward::GetID()
{ return id; }
int Forward::GetMotherID()
{ return motherid; }
int Forward::GetPDGcode()
{ return pdgcode; }
TLorentzVector Forward::GetMomentum()
{ return momentum; }
TVector3 Forward::GetPosition()
{ return position; }
bool Forward::IsBackground()
{ return isbackground; }
