#ifndef RECODATACONTAINER_HPP
#define RECODATACONTAINER_HPP

#include <vector>

#include "TObject.h"

#include "RHICfParam.hpp"

class RecoDataContainer: public TObject
{
public:
  RecoDataContainer();
  virtual ~RecoDataContainer();
  virtual void Reset();
  int GetMaxBar(int atower, int abelt, int axy);
  double GetBeltSum(int atower, int abelt, int axy);
  double GetLayerSum(int atower, int abelt);
  int GetMaxLayerSum(int atower,double thr);

  void SetPlate(int atower, int aplate, double aedep);
  void SetBar(int atower, int abelt, int axy, int abar, double aedep);
  //  void SetUnused(int abelt, int axy, int aunused, double aedep);
  std::vector<std::vector<double> > GetPlate();
  std::vector<std::vector<std::vector<std::vector<double> > > > GetBar();
  //  std::vector<std::vector<std::vector<double> > > GetUnused();
  void SetFC(int atower, double aedep);
  std::vector<double> GetFC();
private:

protected:
  std::vector<std::vector<double> > plate;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar;
  //  std::vector<std::vector<std::vector<double> > > unused;
  std::vector<double> fc;

  ClassDef(RecoDataContainer, 1)
};

#endif
