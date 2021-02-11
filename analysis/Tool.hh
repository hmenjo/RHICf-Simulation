#ifndef TOOL_HH
#define TOOL_HH

#include "RHICfSimEvent.hpp"

using namespace std;

vector<Central*> GetPhotonPair(vector<Central*> central, int ipart);
int Pi0Type(vector<Central*> central, int ipart, double rhicfy);
TVector3 GetProjection(TVector3 pos, TLorentzVector mom, double z=17720);
int CheckHit(double xx, double yy, double height, double edge=2.);

///////////////

vector<Central*> GetPhotonPair(vector<Central*> central, int ipart)
/// 0: Photon1, 1: Photon2, 2: Pi0/Eta
{
  vector<Central*> PhotonPair;
  PhotonPair.clear();

  if(central[ipart]->GetPDGcode()==111 || 
     central[ipart]->GetPDGcode()==221) {
    for(unsigned int i=0; i<central.size(); i++) {
      if(central[i]->isBoundary() &&
	 central[i]->GetPDGcode()==22 &&
	 central[i]->GetMotherID()==central[ipart]->GetID())
	PhotonPair.push_back(central[i]);
    }
  }

  if(PhotonPair.size()!=2) PhotonPair.clear();

  PhotonPair.push_back(central[ipart]);

  return PhotonPair;
}

int Pi0Type(vector<Central*> central, int ipart, double rhicfy)
/// 1: TypeI, 2: TypeII, 0: Else
{
  if(central[ipart]->GetPDGcode()!=221 &&
     central[ipart]->GetPDGcode()!=111)
    return 0;
  if(central[ipart]->GetMomentum().Z()<=0)
    return 0;

  int nhit[ntower+1]={};
  for(unsigned int i=0; i<central.size(); i++) {
    if(!central[i]->isBoundary() ||
       central[i]->GetPDGcode()!=22 ||
       central[i]->GetMotherID()!=central[ipart]->GetID())
      continue;

    TVector3 proj=GetProjection(central[i]->GetPosition2(), central[i]->GetMomentum(), 17720);

    nhit[CheckHit(proj.x(),proj.y(),rhicfy,0)]++;
  }

  if(nhit[0]==1 && nhit[1]==1 && nhit[2]==0)      return 1;
  else if(nhit[0]==2 && nhit[1]==0 && nhit[2]==0) return 2;
  else if(nhit[0]==0 && nhit[1]==2 && nhit[2]==0) return 2;
  else                                            return 0;
}

TVector3 GetProjection(TVector3 pos, TLorentzVector mom, double z)
{
  double k=(z-pos.z())/mom.Vect().z();

  return pos+mom.Vect()*k;
}

int CheckHit(double xx, double yy, double height, double edge)
{
  /// 0: Hit in TS, 1: Hit in TL, 2: No hit
  if(yy<+xx+(10.-edge)*sqrt(2.)+height &&
     yy<-xx+(10.-edge)*sqrt(2.)+height &&
     yy>+xx-(10.-edge)*sqrt(2.)+height &&
     yy>-xx-(10.-edge)*sqrt(2.)+height) {
    return 0;
  }else if(yy<+xx+(50.-edge)*sqrt(2.)+5.+height &&
	   yy<-xx+(50.-edge)*sqrt(2.)+5.+height &&
	   yy>+xx+(10.+edge)*sqrt(2.)+5.+height &&
	   yy>-xx+(10.+edge)*sqrt(2.)+5.+height) {
    return 1;
  }

  return 2;
}

#endif
