#ifndef RHICFRECONSTRUCITON_HH
#define RHICFRECONSTRUCITON_HH

#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMatrixD.h"
#include "Minuit2/FCNBase.h"

#include "../../lib/RHICfSimEvent.hpp"
#include "../../lib/RunInfo.hpp"

#define STATIC_CHECK(expr) { char unnamed[(expr) ? 1 : 0]; }

using namespace ROOT::Minuit2;

namespace fs=boost::filesystem;

class RHICfReconstruction
{
public:
  RHICfReconstruction(TFile* ainput, TFile* aoutput, fs::path atables);
  ~RHICfReconstruction();
  void CrossTalk();
  void PedestalFluctuation();
  void CrossTalkCorrection();
  void PeakSearch();
  void LateralFitting();
  void SingleHitFitting(int atower, int abelt, int axy);
  void MultiHitFitting(int atower, int abelt, int axy);

  template<typename type> double CheckValue(type data1, type data2)
  {
    STATIC_CHECK(sizeof(data1)==sizeof(data2));
    if(data1>-1. || data1<1.) return data1;
    else                      return data2;
  }

private:
  TFile *finput;
  TFile *foutput;
  fs::path ftables;

  ReconstructContainer* reconstruct;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar_mc;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar_nocorr;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar_raw;
  std::vector<std::vector<std::vector<std::vector<double> > > > bar_err;

  std::vector<std::vector<double> > plate_mc;
  std::vector<std::vector<double> > plate;

  std::vector<double> fc_mc;
  std::vector<double> fc;

  std::vector<double> zdc;
  std::vector<double> zdc_mc;
  std::vector<int> nphoton_mc;
  std::vector<int> nphoton;

  std::vector<std::vector<double> > smd_mc;
  std::vector<std::vector<double> > smd;

  std::vector<std::vector<std::vector<double> > > deconv;

  std::vector<std::vector<std::vector<bool> > > isMultiHit;
  std::vector<std::vector<std::vector<int> > > nPeaks;
  std::vector<std::vector<std::vector<std::vector<double> > > > PeakPosition;
  std::vector<std::vector<std::vector<std::vector<double> > > > PeakHeight;

  std::vector<std::vector<std::vector<std::vector<double> > > > Position;
  std::vector<std::vector<std::vector<std::vector<double> > > > Rho1;
  std::vector<std::vector<std::vector<std::vector<double> > > > Rho2;
  std::vector<std::vector<std::vector<std::vector<double> > > > Norm;
  std::vector<std::vector<std::vector<std::vector<double> > > > Frac;
};

namespace ROOT
{
  namespace Minuit2
  {
    /// ShowerLateralCore
    class ShowerLateralCore
    {
      std::vector<double> fMeasurements;
      std::vector<double> fMVariances;
      std::vector<double> fPositions;
      std::vector<double> fpar;
      double *fval;

    public:
      void operator() (const std::vector<double>& r) const
      {
	for(unsigned int i=0; i<r.size(); i++) {
	  double edep=fMeasurements[i];
	  double err =fMVariances[i];
	  double pos =fPositions[i];
	  double func=fpar[2]*
	    (0.5*    fpar[4] *fpar[0]/pow((pow(pos-fpar[1],2.)+fpar[0]),1.5)+
	     0.5*(1.-fpar[4])*fpar[3]/pow((pow(pos-fpar[1],2.)+fpar[3]),1.5));
	  fval[i] = pow((func-edep)/err, 2.);
	}
      }
      ShowerLateralCore(const std::vector<double> &Measurements,
			const std::vector<double> &MVariances,
			const std::vector<double> &Positions,
			const std::vector<double> &par, double *my_fval):
	fMeasurements(Measurements),
	fMVariances(MVariances),
	fPositions(Positions),
	fpar(par), fval(my_fval)
      {
      }
    };

    /// ShowerLateralFunction
    class ShowerLateralFunction: public FCNBase
    {
    public:
      ShowerLateralFunction(const std::vector<double>& meas,
			    const std::vector<double>& pos,
			    const std::vector<double>& mvar):
	fMeasurements(meas),
	fPositions(pos),
	fMVariances(mvar)
      {
      }

      virtual ~ShowerLateralFunction()
      {
      }

      virtual double operator() (const std::vector<double>& par) const
      {
	double fval = 0.;
	for(unsigned int i=0; i<fMeasurements.size(); i++) {
	  double adc  = fMeasurements[i];
	  double err  = fMVariances[i];
	  double pos  = fPositions[i];
	  double func = par[2]*
	    (0.5*    par[4] *par[0]/pow((pow(pos-par[1],2.)+par[0]),1.5)+
	     0.5*(1- par[4])*par[3]/pow((pow(pos-par[1],2.)+par[3]),1.5));
	  fval += pow((func-adc)/err, 2.);
	}
	if(0) std::cout << "fval=" << fval << std::endl;

	return fval;
      }

      virtual double Up() const
      {return 1.;}

    private:
      std::vector<double> fMeasurements;
      std::vector<double> fPositions;
      std::vector<double> fMVariances;
    };

    /// ShowerLateralFunction2
    class ShowerLateralFunction2: public FCNBase
    {
    public:
      ShowerLateralFunction2(const std::vector<double>& meas,
			     const std::vector<double>& pos,
			     const std::vector<double>& mvar):
	fMeasurements(meas),
	fPositions(pos),
	fMVariances(mvar)
      {}

      virtual ~ShowerLateralFunction2()
      {}

      virtual double operator() (const std::vector<double>& par) const
      {
	double fval = 0.;
	for(unsigned int i=0; i<fMeasurements.size(); i++) {
	  double adc  = fMeasurements[i];
	  double err  = fMVariances[i];
	  double pos  = fPositions[i];
	  double func =
	    par[2]*
	    (0.5*   par[4] *par[0]/pow((pow(pos-par[1],2.)+par[0]),1.5)+
	     0.5*(1-par[4])*par[3]/pow((pow(pos-par[1],2.)+par[3]),1.5))+
	    par[7]*
	    (0.5*   par[9] *par[5]/pow((pow(pos-par[6],2.)+par[5]),1.5)+
	     0.5*(1-par[9])*par[8]/pow((pow(pos-par[6],2.)+par[8]),1.5));
	  fval += pow((func-adc)/err, 2.);
	}
	return fval;
      }

      virtual double Up() const
      {return 1.;}

    private:
      std::vector<double> fMeasurements;
      std::vector<double> fPositions;
      std::vector<double> fMVariances;
    };
  }
}


#endif
