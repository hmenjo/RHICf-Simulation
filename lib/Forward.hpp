#ifndef FORWARD_HPP
#define FORWARD_HPP

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class Forward: public TObject
{
public:
  Forward();
  ~Forward();

  void SetMotherID(int amotherid);
  void SetPDGcode(int apdgcode);
  void SetMomentum(TLorentzVector amomentum);
  void SetPosition(TVector3 aposition);
  void SetIsBackground(bool abackground);
  void SetIntermediate(Forward* aintermediate);

  int GetMotherID();
  int GetPDGcode();
  TLorentzVector GetMomentum();
  TVector3 GetPosition();
  bool IsBackground();
  Forward* GetIntermediate();

private:
  int motherid;
  int pdgcode;
  TLorentzVector momentum;
  TVector3 position;
  bool isbackground;
  Forward* intermediate;

  ClassDef(Forward, 2)
};

#endif
