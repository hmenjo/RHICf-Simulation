#ifndef RECONSTRUCT_HPP
#define RECONSTRUCT_HPP

#include <vector>

#include "TObject.h"

#include "RHICfParam.hpp"

class Reconstruct: public TObject
{
public:
  Reconstruct();
  ~Reconstruct();
  void Reset();

  void SetBarErr(std::vector<std::vector<std::vector<std::vector<double> > > > abar_err);
  void SetBarRaw(std::vector<std::vector<std::vector<std::vector<double> > > > abar_raw);
  std::vector<std::vector<std::vector<std::vector<double> > > > GetBarRaw();
  std::vector<std::vector<std::vector<std::vector<double> > > > GetBarErr();

  void SetPosition(std::vector<std::vector<std::vector<std::vector<double> > > > aposition);
  void SetRho1(std::vector<std::vector<std::vector<std::vector<double> > > > arho1);
  void SetRho2(std::vector<std::vector<std::vector<std::vector<double> > > > arho2);
  void SetFrac(std::vector<std::vector<std::vector<std::vector<double> > > > afrac);
  void SetNorm(std::vector<std::vector<std::vector<std::vector<double> > > > anorm);
  std::vector<std::vector<std::vector<std::vector<double> > > > GetPosition();
  std::vector<std::vector<std::vector<std::vector<double> > > > GetRho1();
  std::vector<std::vector<std::vector<std::vector<double> > > > GetRho2();
  std::vector<std::vector<std::vector<std::vector<double> > > > GetFrac();
  std::vector<std::vector<std::vector<std::vector<double> > > > GetNorm();

  bool IsTriggered(int atower, double threshold, int nlay);
  double GetSumdE(int atower, int start, int last, bool weight);
  double GetLXX(int atower, double frac);
  double GetL20(int atower);
  double GetL90(int atower);
  bool HasHit(int atower, int abelt, int ixy);
  bool IsMultiHit(int atower, int abelt, int ixy);

protected:
  std::vector<std::vector<std::vector<std::vector<double> > > > bar_raw;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar_err;

  std::vector<std::vector<std::vector<std::vector<double> > > > position;
  std::vector<std::vector<std::vector<std::vector<double> > > > rho1;
  std::vector<std::vector<std::vector<std::vector<double> > > > rho2;
  std::vector<std::vector<std::vector<std::vector<double> > > > frac;
  std::vector<std::vector<std::vector<std::vector<double> > > > norm;

private:
  std::vector<double> fc;
  std::vector<std::vector<double> > plate;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar;

  ClassDef(Reconstruct,1)
};

#endif
