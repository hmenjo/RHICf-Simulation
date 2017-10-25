#ifndef RHICFSIMEVENT_HPP
#define RHICFSIMEVENT_HPP

#include "TObject.h"

#include "CentralContainer.hpp"
#include "ForwardContainer.hpp"
#include "MCDataContainer.hpp"
#include "RecoDataContainer.hpp"
#include "ZDCContainer.hpp"
#include "BBCContainer.hpp"
#include "RomanPotContainer.hpp"
#include "ReconstructContainer.hpp"

class RHICfSimEvent: public TObject
{
public:
  RHICfSimEvent();
  ~RHICfSimEvent();

  void SetForward(ForwardContainer *aforward);
  void SetCentral(CentralContainer *acentral);
  void SetMC(MCDataContainer *amc);
  void SetReco(RecoDataContainer *areco);
  void SetZDC(ZDCContainer *azdc);
  void SetBBC(BBCContainer *abbc);
  void SetRomanPot(RomanPotContainer *aromanpot);
  void SetReconstruct(ReconstructContainer *areconstruct);

  ForwardContainer* GetForward();
  CentralContainer* GetCentral();
  MCDataContainer* GetMC();
  RecoDataContainer* GetReco();
  ZDCContainer* GetZDC();
  BBCContainer* GetBBC();
  RomanPotContainer* GetRomanPot();
  ReconstructContainer* GetReconstruct();

private:
  ForwardContainer *forward;
  CentralContainer *central;
  MCDataContainer *mc;
  RecoDataContainer *reco;
  ZDCContainer *zdc;
  BBCContainer *bbc;
  RomanPotContainer *romanpot;
  ReconstructContainer *reconstruct;

  ClassDef(RHICfSimEvent, 2)
};

#endif
