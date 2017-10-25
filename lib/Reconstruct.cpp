#include <iostream>

#include "Reconstruct.hpp"

Reconstruct::Reconstruct()
{
}

Reconstruct::~Reconstruct()
{
}

void Reconstruct::Reset()
{
  fc.clear();
  fc.resize(ntower);

  plate.clear();
  plate.resize(ntower);
  for(int itower=0; itower<ntower; itower++) {
    plate[itower].resize(nplate);
  }

  bar.clear();
  bar_raw.clear();
  bar_err.clear();
  bar.resize(ntower);
  bar_raw.resize(ntower);
  bar_err.resize(ntower);
  for(int itower=0; itower<ntower; itower++) {
    bar[itower].resize(nbelt);
    bar_raw[itower].resize(nbelt);
    bar_err[itower].resize(nbelt);
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      bar[itower][ibelt].resize(nxy);
      bar_raw[itower][ibelt].resize(nxy);
      bar_err[itower][ibelt].resize(nxy);
      for(int ixy=0; ixy<nxy; ixy++) {
	bar[itower][ibelt][ixy].resize(nbar[itower]);
	bar_raw[itower][ibelt][ixy].resize(nbar[itower]);
	bar_err[itower][ibelt][ixy].resize(nbar[itower]);
      }
    }
  }

  position.resize(ntower);
  rho1.resize(ntower);
  rho2.resize(ntower);
  norm.resize(ntower);
  frac.resize(ntower);
  for(int itower=0; itower<ntower; itower++) {
    position[itower].resize(nbelt);
    rho1[itower].resize(nbelt);
    rho2[itower].resize(nbelt);
    norm[itower].resize(nbelt);
    frac[itower].resize(nbelt);
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      position[itower][ibelt].resize(nxy);
      rho1[itower][ibelt].resize(nxy);
      rho2[itower][ibelt].resize(nxy);
      norm[itower][ibelt].resize(nxy);
      frac[itower][ibelt].resize(nxy);
      for(int ixy=0; ixy<nxy; ixy++) {
	position[itower][ibelt][ixy].clear();
	rho1[itower][ibelt][ixy].clear();
	rho2[itower][ibelt][ixy].clear();
	norm[itower][ibelt][ixy].clear();
	frac[itower][ibelt][ixy].clear();
      }
    }
  }
}

void Reconstruct::SetBarRaw(std::vector<std::vector<std::vector<std::vector<double> > > > abar_raw)
{ bar_raw=abar_raw; }
void Reconstruct::SetBarErr(std::vector<std::vector<std::vector<std::vector<double> > > > abar_err)
{ bar_err=abar_err; }

std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetBarRaw()
{ return bar_raw; }
std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetBarErr()
{ return bar_err; }

void Reconstruct::SetPosition(std::vector<std::vector<std::vector<std::vector<double> > > > aposition)
{ position=aposition; }
void Reconstruct::SetRho1(std::vector<std::vector<std::vector<std::vector<double> > > > arho1)
{ rho1=arho1; }
void Reconstruct::SetRho2(std::vector<std::vector<std::vector<std::vector<double> > > > arho2)
{ rho2=arho2; }
void Reconstruct::SetFrac(std::vector<std::vector<std::vector<std::vector<double> > > > afrac)
{ frac=afrac; }
void Reconstruct::SetNorm(std::vector<std::vector<std::vector<std::vector<double> > > > anorm)
{ norm=anorm; }
std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetPosition()
{ return position; }
std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetRho1()
{ return rho1; }
std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetRho2()
{ return rho2; }
std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetFrac()
{ return frac; }
std::vector<std::vector<std::vector<std::vector<double> > > > Reconstruct::GetNorm()
{ return norm; }



bool Reconstruct::IsTriggered(int atower, double athreshold=20., int anlay=3)
{
  int btrg[nplate]={};
  for(int iplate=0; iplate<nplate; iplate++)
    if(plate[atower][iplate]>athreshold)
      btrg[iplate]=1;

  for(int iplate=0; iplate<nplate-anlay+1; iplate++) {
    int coin=0;
    for(int ialay=0; ialay<anlay; ialay++) 
      coin+=btrg[iplate+ialay];
    if(coin==anlay) return true;
  }

  return false;
}

double Reconstruct::GetSumdE(int atower, int start=1, int last=nplate, bool isweight=true)
{
  double sum=0;
  for(int iplate=start; iplate<last; iplate++) {
    if(isweight)
      sum+=plate[atower][iplate]*weight[iplate];
    else
      sum+=plate[atower][iplate];
  }

  return sum;
}

double Reconstruct::GetLXX(int atower, double xx)
{
  double sum0=0;
  double sum1=plate[atower][0];
  double d0=0;
  double d1=weight[0]*2.;
  for(int iplate=0; iplate<nplate-1; iplate++) {
    sum0+=plate[atower][iplate];
    sum1+=plate[atower][iplate+1];
    d0+=weight[iplate]*2.;
    d1+=weight[iplate+1]*2.;
    if(sum1/GetSumdE(atower)>xx) {
      double a=(sum1-sum0)/(d1-d0);
      double b=sum1-a*d1;
      return (GetSumdE(atower)*xx-b)/a;
    }
  }
  return 99;
}

double Reconstruct::GetL20(int atower)
{
  return GetLXX(atower, 0.2);
}

double Reconstruct::GetL90(int atower)
{
  return GetLXX(atower, 0.9);
}

bool Reconstruct::HasHit(int atower, int abelt, int axy)
{
  if(position[atower][abelt][axy].size()!=0)
    return true;

  return false;
}

bool Reconstruct::IsMultiHit(int atower, int abelt, int axy)
{
  if(position[atower][abelt][axy].size()>1)
    return true;

  return false;
}
