#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <utility>
#include <omp.h>
#include <fstream>

#include <boost/filesystem.hpp>

#include "TCanvas.h"
#include "TStyle.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TSpectrum.h"
#include "TFitResult.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnPrint.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TVectorD.h"

#include "RHICfReconstruction.hh"

//#define DRAW

RHICfReconstruction::RHICfReconstruction(TFile* ainput, TFile* aoutput, fs::path atables)
{
  finput=ainput;
  ftables=atables;

  TTree *trun=(TTree*)finput->Get("RunInfo");
  RunInfo* runInfo=new RunInfo();
  trun->SetBranchAddress("RunInfo",&runInfo);
  trun->SetMakeClass(1);
  trun->GetEntry(0);
  //  if(!(runInfo->GetFlag()).check(bBEAMTEST_ARM1) &&
  //     !(runInfo->GetFlag()).check(bRESPONSE_ARM1)) {
  if(0) {
    std::cerr << "No ARM1 data!" << std::endl;
    exit(1);
  }

  int nevent=runInfo->GetNumberOfEvents();

  CentralContainer* centralCont=new CentralContainer();
  ForwardContainer* forwardCont=new ForwardContainer();
  BBCContainer* bbcCont=new BBCContainer();
  MCDataContainer* mcCont=new MCDataContainer();
  ZDCContainer* zdcCont=new ZDCContainer();

  TTree *tevent=(TTree*)finput->Get("EventInfo");

  tevent->SetMakeClass(1);


  if((runInfo->GetFlag()).check(bGENERATE))
    tevent->SetBranchAddress("central", &centralCont);
  if((runInfo->GetFlag()).check(bTRANSPORT)) {
    tevent->SetBranchAddress("forward", &forwardCont);
    tevent->SetBranchAddress("bbc", &bbcCont);
  }
  if((runInfo->GetFlag()).check(bRESPONSE_ARM1))
    tevent->SetBranchAddress("mc", &mcCont);
  if((runInfo->GetFlag()).check(bRESPONSE_ZDC))
    tevent->SetBranchAddress("zdc", &zdcCont);
  if((runInfo->GetFlag()).check(bBEAMTEST))
    tevent->SetBranchAddress("forward", &forwardCont);

  foutput=aoutput;

  TTree *trun_out=(TTree*)foutput->Get("RunInfo");
  RunInfo* runInfo_out=new RunInfo();
  trun_out->SetBranchAddress("RunInfo", &runInfo_out);

  TTree *tevent_out=(TTree*)foutput->Get("EventInfo");
  RHICfSimEvent* simEvent_out=new RHICfSimEvent();
  tevent_out->SetBranchAddress("SimEvent", &simEvent_out);


  for(int i=0; i<nevent; i++) {
    std::cout << ">>> " << i << " " << std::endl;

    tevent->GetEntry(i);

    bar_mc=mcCont->GetBar();
    plate_mc=mcCont->GetPlate();
    fc_mc=mcCont->GetFC();
    zdc_mc=zdcCont->GetZDC();
    nphoton_mc=zdcCont->GetNphoton();
    smd_mc=zdcCont->GetSMD();

    reconstruct=new ReconstructContainer();
    reconstruct->Reset();

    bar_raw=reconstruct->GetBarRaw();
    bar_err=reconstruct->GetBarErr();
    plate=reconstruct->GetPlate();
    bar=reconstruct->GetBar();
    fc=reconstruct->GetFC();
    nphoton=reconstruct->GetNphoton();
    smd=reconstruct->GetSMD();

    Position=reconstruct->GetPosition();
    Rho1=reconstruct->GetRho1();
    Rho2=reconstruct->GetRho2();
    Norm=reconstruct->GetNorm();
    Frac=reconstruct->GetFrac();

    deconv.resize(ntower);
    for(int itower=0; itower<ntower; itower++) {
      deconv[itower].resize(nbelt);
      for(int ixy=0; ixy<nxy; ixy++) {
	deconv[itower][ixy].resize(nbar[itower]);
      }
    }
    nPeaks.clear();
    isMultiHit.clear();
    PeakPosition.clear();
    PeakHeight.clear();

    nPeaks.resize(ntower);
    isMultiHit.resize(ntower);
    PeakPosition.resize(ntower);
    PeakHeight.resize(ntower);
    for(int itower=0; itower<ntower; itower++) {
      nPeaks[itower].resize(nbelt);
      isMultiHit[itower].resize(nbelt);
      PeakPosition[itower].resize(nbelt);
      PeakHeight[itower].resize(nbelt);
      for(int ibelt=0; ibelt<nbelt; ibelt++) {
	nPeaks[itower][ibelt].resize(nxy);
	isMultiHit[itower][ibelt].resize(nxy);
	PeakPosition[itower][ibelt].resize(nxy);
	PeakHeight[itower][ibelt].resize(nxy);
	for(int ixy=0; ixy<nxy; ixy++) {
	  nPeaks[itower][ibelt][ixy]=0;
	  isMultiHit[itower][ibelt][ixy]=false;
	  PeakPosition[itower][ibelt][ixy].clear();
	  PeakHeight[itower][ibelt][ixy].clear();
	}
      }
    }

    CrossTalk();
    PedestalFluctuation();
    CrossTalkCorrection();
    PeakSearch();
    LateralFitting();

    //    foutput->cd();
    for(int itower=0; itower<ntower; itower++)
      for(int iplate=0; iplate<nplate; iplate++)
	reconstruct->SetPlate(itower,iplate,plate[itower][iplate]);
    for(int itower=0; itower<ntower; itower++)
      for(int ibelt=0; ibelt<nbelt; ibelt++)
	for(int ixy=0; ixy<nxy; ixy++)
	  for(int ibar=0; ibar<nbar[itower]; ibar++)
	    reconstruct->SetBar(itower,ibelt,ixy,ibar,bar[itower][ibelt][ixy][ibar]);
    for(int itower=0; itower<ntower; itower++)
      reconstruct->SetFC(itower,fc[itower]);
    for(int izdc=0; izdc<nzdc; izdc++) 
      reconstruct->SetZDC(izdc,zdc[izdc]);
    for(int ixy=0; ixy<nxy; ixy++) 
      for(int ismd=0; ismd<nsmd[ixy]; ismd++) 
	reconstruct->SetSMD(ixy,ismd,smd[ixy][ismd]);
    reconstruct->SetBarErr(bar_err);
    reconstruct->SetBarRaw(bar_raw);
    reconstruct->SetPosition(Position);
    reconstruct->SetRho1(Rho1);
    reconstruct->SetRho2(Rho2);
    reconstruct->SetNorm(Norm);
    reconstruct->SetFrac(Frac);

    simEvent_out->SetReconstruct(reconstruct);

    if((runInfo->GetFlag()).check(bGENERATE)) {
      simEvent_out->SetCentral(centralCont);
    }
    if((runInfo->GetFlag()).check(bTRANSPORT)) {
      simEvent_out->SetForward(forwardCont);
      simEvent_out->SetBBC(bbcCont);
    }
    //    if((runInfo->GetFlag()).check(bRESPONSE_ARM1))
    //      simEvent_out->SetMC(mcCont);
    //    if((runInfo->GetFlag()).check(bRESPONSE_ZDC))
    //      simEvent_out->SetZDC(zdcCont);
    //    if((runInfo->GetFlag()).check(bBEAMTEST))
    //      simEvent_out->SetForward(forwardCont);

    if(forwardCont->GetContainer().size()!=0) 
      tevent_out->Fill();
  }

  runInfo_out=runInfo;

  trun_out->Fill();
  finput->Close();
}

RHICfReconstruction::~RHICfReconstruction()
{
}

void RHICfReconstruction::CrossTalk()
{
  TFile *fxtalk=new TFile((ftables.string()+"/GSObarXtalk.root").c_str());

  char cc[256];
  TGraphErrors* xtalk[ntower][nbelt][nxy][nbar[1]];
  for(int itower=0; itower<ntower; itower++) {
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	for(int ibar=0; ibar<nbar[itower]; ibar++) {
	  sprintf(cc,"Xtalk_Tower%d_Lay%d_XY%d_Ch%02d", itower,ibelt,ixy,ibar);
	  xtalk[itower][ibelt][ixy][ibar]=(TGraphErrors*)fxtalk->Get(cc);
	}
      }
    }
  }

  for(int itower=0; itower<ntower; itower++) {
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	for(int ibar=0; ibar<nbar[itower]; ibar++) {
	  for(int jtower=0; jtower<ntower; jtower++) {
	    for(int jbar=0; jbar<nbar[jtower]; jbar++) {
	      if(itower==jtower && ibar==jbar) {
		bar_raw[jtower][ibelt][ixy][jbar]+=
		  bar_mc[itower][ibelt][ixy][ibar];
	      }else{
		TRandom3 r(0);
		if(jtower==0) {
		  bar_raw[jtower][ibelt][ixy][jbar]+=
		    bar_mc[itower][ibelt][ixy][ibar]*
		    r.Gaus(xtalk[itower][ibelt][ixy][ibar]->GetY()[jbar],
			   xtalk[itower][ibelt][ixy][ibar]->GetEY()[jbar]);
		}else{
		  bar_raw[jtower][ibelt][ixy][jbar]+=
		    bar_mc[itower][ibelt][ixy][ibar]*
		    r.Gaus(xtalk[itower][ibelt][ixy][ibar]->GetY()[jbar+nbar[0]],
			   xtalk[itower][ibelt][ixy][ibar]->GetEY()[jbar+nbar[0]]);
		}
	      }
	    }
	  }
	}
      }
    }
  }

  double mip=0.746/5.; /// 1MIP=746keV/5p.e.
  for(int itower=0; itower<ntower; itower++) {
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	for(int ibar=0; ibar<nbar[itower]; ibar++) {
	  TRandom3 p(0),g(0);
	  double npe=bar_raw[itower][ibelt][ixy][ibar]/mip;
	  if(npe<20) {
	    while(npe<0) {
	      npe=p.Poisson(npe);
	      npe=g.Gaus(npe,sqrt(npe)*0.3);
	    }
	  }
	  bar_raw[itower][ibelt][ixy][ibar]=npe*mip;
	}
      }
    }
  }

  fxtalk->Close();
}

void RHICfReconstruction::PedestalFluctuation()
{
  for(int itower=0; itower<ntower; itower++) {
    for(int iplate=0; iplate<nplate; iplate++) {
      TRandom3 r(0);
      plate[itower][iplate]=r.Gaus(plate_mc[itower][iplate],15);
    }
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	for(int ibar=0; ibar<nbar[itower]; ibar++) {
	  TRandom3 r(0);
	  double tmp=r.Gaus(bar_raw[itower][ibelt][ixy][ibar],0.2);
	  bar_raw[itower][ibelt][ixy][ibar]=tmp;
	}
      }
    }
    TRandom3 r(0);
    fc[itower]=r.Gaus(fc_mc[itower],0.1);
  }

  for(int izdc=0; izdc<nzdc; izdc++) {
    TRandom3 r(0);
    zdc.push_back(r.Gaus(nphoton_mc[izdc],20));
  }
  for(int ixy=0; ixy<nxy; ixy++) {
    for(int ismd=0; ismd<nsmd[ixy]; ismd++) {
      TRandom3 r(0);
      smd[ixy][ismd]=r.Gaus(smd_mc[ixy][ismd],10);
    }
  }
}

void RHICfReconstruction::CrossTalkCorrection()
{
  TFile *fcorr=new TFile((ftables.string()+"/GSObarXtalk_Correction.root").c_str());

  char cc[256];
  TMatrixD *mcorr[nbelt][nxy];
  for(int ibelt=0; ibelt<nbelt; ibelt++) {
    for(int ixy=0; ixy<nxy; ixy++) {
      sprintf(cc, "XtalkCorr_Lay%d_XY%d",ibelt,ixy);
      mcorr[ibelt][ixy]=(TMatrixD*)fcorr->Get(cc);
    }
  }

  TVectorD *v[nbelt][nxy];
  double tmp[64];
  for(int ibelt=0; ibelt<nbelt; ibelt++) {
    for(int ixy=0; ixy<nxy; ixy++) {
      for(int i=0; i<nbar[0]; i++)
	tmp[i]=bar_raw[0][ibelt][ixy][i];
      for(int i=0; i<nbar[1]; i++)
	tmp[i+nbar[0]]=bar_raw[1][ibelt][ixy][i];
      for(int i=0; i<nunused; i++)
	tmp[i+nbar[0]+nbar[1]]=0;
      v[ibelt][ixy]=new TVectorD(64,tmp);
    }
  }

  double ped[ntower]={0.5,0.5};
  double npe[ntower]={5,5};

  for(int ibelt=0; ibelt<nbelt; ibelt++) {
    for(int ixy=0; ixy<nxy; ixy++) {
      for(int ibar=0; ibar<64; ibar++) {
	double err2=0.;
	for(int jbar=0; jbar<64; jbar++) {
	  double err=0.;
	  if(jbar<20)      err=std::max((*v[ibelt][ixy])[jbar],0.)+pow(ped[0],2.);
	  else if(jbar<60) err=std::max((*v[ibelt][ixy])[jbar],0.)+pow(ped[1],2.);
	  else             err=std::max((*v[ibelt][ixy])[jbar],0.)+pow(ped[0],2.);
	  err2+=pow((*mcorr[ibelt][ixy])[ibar][jbar],2.)*err;
	}
	if(ibar<20)      bar_err[0][ibelt][ixy][ibar]   =sqrt(err2);
	else if(ibar<60) bar_err[1][ibelt][ixy][ibar-20]=sqrt(err2);
      }
    }
  }

  for(int ibelt=0; ibelt<nbelt; ibelt++)
    for(int ixy=0; ixy<nxy; ixy++)
      (*v[ibelt][ixy])*=(*mcorr[ibelt][ixy]);

  for(int itower=0; itower<ntower; itower++) 
    for(int ibelt=0; ibelt<nbelt; ibelt++) 
      for(int ixy=0; ixy<nxy; ixy++) 
	for(int ibar=0; ibar<nbar[itower]; ibar++) 
	  bar[itower][ibelt][ixy][ibar]=(*v[ibelt][ixy])(nbar[0]*itower+ibar);


  for(int ibelt=0; ibelt<nbelt; ibelt++) {
    for(int ixy=0; ixy<nxy; ixy++) {
      v[ibelt][ixy]->Delete();
      mcorr[ibelt][ixy]->Delete();
    }
  }
  fcorr->Close();
}

void RHICfReconstruction::PeakSearch()
{
  // Peak search
  TSpectrum *s = new TSpectrum();

  typedef std::pair<int, double> value_type;
  std::vector< value_type > peaks;


  for(int itower = 0; itower < ntower; itower++) {
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	double maxbar=0.;
	for(int ibar=0; ibar<nbar[itower];ibar++) {
	  if(maxbar<bar[itower][ibelt][ixy][ibar])
	    maxbar=bar[itower][ibelt][ixy][ibar];
	}
	if(maxbar<10) continue;

	TH1F *htmp=new TH1F("htmp","htmp",nbar[itower],0,nbar[itower]);
	htmp->Reset("M");
	for(int ibar=0; ibar<nbar[itower]; ibar++)
	  htmp->SetBinContent(ibar+1, bar[itower][ibelt][ixy][ibar]);

	// Peak finding by TSpectrum
	const int first=htmp->GetXaxis()->GetFirst();
	const int last =htmp->GetXaxis()->GetLast();
	const int size =last-first+1;
	int npeak;
	float *source=new float[size];
	float *dest  =new float[size];
	for(int i=0; i<size; i++)
	  source[i]=htmp->GetBinContent(i+first);
	double sigma=1.;
	npeak=s->SearchHighRes(source,dest,size,sigma,10,
			       kTRUE,3,kTRUE,3);
	for(int i=0; i<size; i++)
	  deconv[itower][ixy][i]=dest[i];

	delete [] source;
	delete [] dest;

	const int nfound=s->Search(htmp, 1, "", 0.10);

	peaks.clear();
	for(int ibin=0; ibin<nfound; ibin++) {
	  double x=(s->GetPositionX())[ibin];
	  double y=htmp->GetBinContent((int)x+1);
	  peaks.push_back(std::make_pair((int)x+1, y));
	}
	sort(peaks.begin(), peaks.end(), bind(&value_type::second, _2) < bind(&value_type::second, _1));

	/// If the peak is at the edge but not registered
	if(nfound>0) {
	  if(htmp->GetMaximumBin()==1 ||
	     htmp->GetMaximumBin()==nbar[itower]) {
	    if(peaks[0].first+1!=htmp->GetMaximumBin()) {
	      peaks.push_back(std::make_pair(htmp->GetMaximumBin()-1,
					     htmp->GetMaximum()));
	      sort(peaks.begin(), peaks.end(), bind(&value_type::second, _2) < bind(&value_type::second, _1));
	    }
	  }
	}

	// Copy to global variables
	nPeaks[itower][ibelt][ixy]=nfound;
	if(nfound>1) isMultiHit[itower][ibelt][ixy]=true;
	else         isMultiHit[itower][ibelt][ixy]=false;
	for(int i=0; i<std::min(2, nfound); i++) {
	  if((double)(peaks[i].second)<10) continue;
	  PeakPosition[itower][ibelt][ixy].push_back((double)(peaks[i].first));
	  PeakHeight[itower][ibelt][ixy].push_back((double)(peaks[i].second));
	}

	if(0) {
	  std::cout << "Tow" << itower << " Lay" << ibelt << " XY" << ixy 
		    << " nPeaks=" << peaks.size() 
		    << " Sum=" << htmp->GetSumOfWeights()
		    << std::endl;
	  for(unsigned int i=0; i<PeakPosition[itower][ibelt][ixy].size(); i++) {
	    std::cout << i << " "
		      << PeakPosition[itower][ibelt][ixy][i] << " "
		      << PeakHeight[itower][ibelt][ixy][i] << " "
		      << std::endl;
	  }
	}

	delete htmp;
      } // XY
    } // Belt
  } // Tower

  delete s;
}

void RHICfReconstruction::LateralFitting()
{
  for(int itower = 0; itower < ntower; itower++) {
    for(int ibelt=0; ibelt<nbelt; ibelt++) {
      for(int ixy=0; ixy<nxy; ixy++) {
	if(PeakPosition[itower][ibelt][ixy].size()==0)
	  continue;
	else if(PeakPosition[itower][ibelt][ixy].size()==1)
	  SingleHitFitting(itower,ibelt,ixy);
	else if(PeakPosition[itower][ibelt][ixy].size()>1)
	  MultiHitFitting(itower,ibelt,ixy);
      }
    }
  }
}

void RHICfReconstruction::SingleHitFitting(int atower, int abelt, int axy)
{
  std::cout << "SingleHitReconstruction" << std::endl;

  std::vector<double> positions;
  std::vector<double> measurements;
  std::vector<double> var;
  std::vector<double> yvalvec;
  double tmp = 0;

  char cc[256];

  positions.clear();
  measurements.clear();
  var.clear();
  yvalvec.clear();

  /// ROOT-embedded fitting(1st fitting)
  double *x   =new double[nbar[atower]];
  double *xerr=new double[nbar[atower]];
  double *y   =new double[nbar[atower]];
  double *yerr=new double[nbar[atower]];
  double barsum=0.;
  for(int ibar=0; ibar<nbar[atower]; ibar++) {
    x[ibar]=ibar+0.5;
    xerr[ibar]=0.5;
    y[ibar]=bar[atower][abelt][axy][ibar];
    yerr[ibar]=bar_err[atower][abelt][axy][ibar];
    barsum+=bar[atower][abelt][axy][ibar];
  }
  TGraphErrors *barspec=new TGraphErrors(nbar[atower],x,y,xerr,yerr);
  TF1 *func=new TF1("elemag","[2]*(0.5*[4]*[0]/pow((pow(x-[1],2.)+[0]),1.5)+0.5*(1-[4])*[3]/pow((pow(x-[1],2.)+[3]),1.5))",-1.,nbar[atower]+1.);
  barspec->Fit(func,"QNMC0","");

  double pmin=0.;
  double pmax=0.;

  func->SetParameter(0, 3.0);
  func->SetParLimits(0, 0.01, 5.);
  func->SetParameter(1, PeakPosition[atower][abelt][axy][0]);
  pmin=std::max(PeakPosition[atower][abelt][axy][0]-2.5, -1.);
  pmax=std::min(PeakPosition[atower][abelt][axy][0]+2.5, nbar[atower]+1.);
  func->SetParLimits(1, pmin, pmax);
  func->SetParameter(2, barsum*0.5);
  pmin=barsum*0.25;
  pmax=barsum*2.00;
  func->SetParLimits(2, pmin, pmax);
  func->SetParameter(3, 10.);
  func->SetParLimits(3, 5., 50.);
  func->SetParameter(4, 0.8);
  func->SetParLimits(4, 0., 1.);

  TFitResultPtr r = barspec->Fit(func, "QRNME0S");

  if(0) {
    std::cout << "Peak: " << std::endl;
    std::cout << "  Position: "
	      << PeakPosition[atower][abelt][axy][0]
	      << " Peak: " 
	      << PeakHeight[atower][abelt][axy][0]
	      << std::endl;
    for(int i=0; i<5; i++) 
      std::cout << "p" << i << "= " << func->GetParameter(i)
		<< " " <<  func->GetParError(i)
		<< std::endl;
    std::cout << "Chi2= " << r->Chi2() << std::endl;
    std::cout << "Edm = " << r->Edm()  << std::endl;
  }

  /// Minuit2, 1st turn
  std::cout << " Minuit2 1st turn" << std::endl;
  for(int ibar=0; ibar<nbar[atower]; ibar++) {
    positions.push_back(ibar+0.5);
    measurements.push_back(bar[atower][abelt][axy][ibar]);
    var.push_back(bar_err[atower][abelt][axy][ibar]);
  }
  ShowerLateralFunction fFCN(measurements, positions, var);
  MnUserParameters upar;
  upar.Add("p0", func->GetParameter(0), .01);
  upar.Add("p1", func->GetParameter(1), 0.1);
  upar.Add("p2", func->GetParameter(2), 1.0);
  upar.Add("p3", func->GetParameter(3), 1.0);
  upar.Add("p4", func->GetParameter(4), .01);

  // if an value is invalid, return 0.
  tmp=this->CheckValue(func->GetParameter(0), 0.);
  pmin=std::max(tmp*0.5, 0.01);
  pmax=std::max(tmp*1.5, 5.0);
  upar.SetLimits("p0", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(1), 0.);
  pmin=std::max(tmp-2.5, -1.);
  pmax=std::min(tmp+2.5, nbar[atower]+1.);
  upar.SetLimits("p1", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(2), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::max(tmp*1.5, 10.);
  upar.SetLimits("p2", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(3), 0.);
  pmin=std::min(tmp*0.5, this->CheckValue(func->GetParameter(0), 0.));
  pmax=std::max(tmp*1.5, 50.);
  upar.SetLimits("p3", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(4), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::min(tmp*1.5+1.e-3, 1.);
  upar.SetLimits("p4", pmin, pmax);

  MnMigrad migrad(fFCN, upar);
  FunctionMinimum min0=migrad();
  bool validmin0=false;
  if(min0.IsValid() && !isnan(min0.Edm())) {
    std::cout << "=== minimum0  --> " << min0 << std::endl;
    validmin0=true;
  }

  if(1) {
    std::cout << "min0 is valid: " << min0.IsValid() << std::endl;
    if(min0.IsValid()) {
      for(int i=0; i<5; i++) {
	std::cout << "p" << i << "= " << min0.UserState().Value(i) << std::endl;
      }
      std::cout << "Chi2= " << min0.UserState().Fval() << std::endl;
      std::cout << "Edm = " << min0.UserState().Edm()  << std::endl;
    }
  }

  /// Minuit2, 2nd turn
  std::cout << " Minuit2 2nd turn" << std::endl;
  upar.Add("p0", min0.UserState().Value(0), .01);
  upar.Add("p1", min0.UserState().Value(1), 0.1);
  upar.Add("p2", min0.UserState().Value(2), 1.0);
  upar.Add("p3", min0.UserState().Value(3), 1.0);
  upar.Add("p4", min0.UserState().Value(4), .01);

  tmp=this->CheckValue(min0.UserState().Value(0), 0.);
  pmin=std::max(tmp*0.5, 0.01);
  pmax=std::max(tmp*1.5, 5.0);
  upar.SetLimits("p0", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(1), 0.);
  pmin=std::max(tmp-2.5, -1.);
  pmax=std::min(tmp+2.5, nbar[atower]+1.);
  upar.SetLimits("p1", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(2), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::max(tmp*1.5, 10.);
  upar.SetLimits("p2", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(3), 0.);
  pmin=std::min(tmp*0.5, CheckValue(min0.UserState().Value(0), 0.));
  pmax=std::max(tmp*1.5, 50.);
  upar.SetLimits("p3", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(4), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::min(tmp*1.5+1.e-3, 1.);
  upar.SetLimits("p4", pmin, pmax);

  migrad.Fix("p0");
  FunctionMinimum min1=migrad();

  migrad.Release("p0");
  migrad.Fix("p1");
  FunctionMinimum min2=migrad();

  migrad.Release("p1");
  migrad.Fix("p2");
  FunctionMinimum min3=migrad();

  migrad.Release("p2");
  migrad.Fix("p3");
  FunctionMinimum min4=migrad();

  migrad.Release("p3");
  migrad.Fix("p4");
  FunctionMinimum min5=migrad();

  migrad.Release("p4");
  migrad.RemoveLimits("p0");
  migrad.RemoveLimits("p1");
  migrad.RemoveLimits("p2");
  migrad.RemoveLimits("p3");
  migrad.RemoveLimits("p4");
  FunctionMinimum min6=migrad();
  bool validmin6=false;

  if(1) {
    if(min6.IsValid()) {
      std::cout << "min6 is valid: " << min6.IsValid() << std::endl;
      for(int i=0; i<5; i++) {
	std::cout << "p" << i << "= " << min6.UserState().Value(i) << std::endl;
      }
      std::cout << "Chi2= " << min6.UserState().Fval() << std::endl;
      std::cout << "Edm = " << min6.UserState().Edm()  << std::endl;
    }
  }
  barspec->Fit(func, "QNMC0", "");

  if(validmin6 &&
     min6.UserState().Value(0) > 0. &&
     min6.UserState().Value(2) > 0. &&
     min6.UserState().Value(3) > 0. &&
     min6.UserState().Value(4) > 0.) {
    // use min6
    std::cout << "  Use min6" << std::endl;

    Position[atower][abelt][axy].push_back(min6.UserState().Value(1));
    Rho1[atower][abelt][axy].push_back(min6.UserState().Value(0));
    Rho2[atower][abelt][axy].push_back(min6.UserState().Value(3));
    Norm[atower][abelt][axy].push_back(min6.UserState().Value(2));
    Frac[atower][abelt][axy].push_back(min6.UserState().Value(4));
  }else{
    if(validmin0 &&
       min0.UserState().Value(0) > 0. &&
       min0.UserState().Value(2) > 0. &&
       min0.UserState().Value(3) > 0. &&
       min0.UserState().Value(4) > 0.) {
      // use min0
      std::cout << "  Use min0" << std::endl;

      Position[atower][abelt][axy].push_back(min0.UserState().Value(1));
      Rho1[atower][abelt][axy].push_back(min0.UserState().Value(0));
      Rho2[atower][abelt][axy].push_back(min0.UserState().Value(3));
      Norm[atower][abelt][axy].push_back(min0.UserState().Value(2));
      Frac[atower][abelt][axy].push_back(min0.UserState().Value(4));
    }else{
      // use root
      std::cout << "  Use ROOT" << std::endl;
      Position[atower][abelt][axy].push_back(func->GetParameter(1));
      Rho1[atower][abelt][axy].push_back(func->GetParameter(0));
      Rho2[atower][abelt][axy].push_back(func->GetParameter(3));
      Norm[atower][abelt][axy].push_back(func->GetParameter(2));
      Frac[atower][abelt][axy].push_back(func->GetParameter(4));
    }
  }
  delete func;
  delete barspec;

  delete[] x;
  delete[] xerr;
  delete[] y;
  delete[] yerr;
}

void RHICfReconstruction::MultiHitFitting(int atower, int abelt, int axy)
{
  std::cout << "MultiHitReconstruction" << std::endl;

  std::vector<double> positions;
  std::vector<double> measurements;
  std::vector<double> var;
  std::vector<double> yvalvec;
  double tmp = 0;

  char cc[256];

  positions.clear();
  measurements.clear();
  var.clear();
  yvalvec.clear();

  /// ROOT-embedded fitting(1st fitting)
  double *x   =new double[nbar[atower]];
  double *xerr=new double[nbar[atower]];
  double *y   =new double[nbar[atower]];
  double *yerr=new double[nbar[atower]];
  double barsum=0.;
  for(int ibar=0; ibar<nbar[atower]; ibar++) {
    x[ibar]=ibar+0.5;
    xerr[ibar]=0.5;
    y[ibar]=bar[atower][abelt][axy][ibar];
    yerr[ibar]=bar_err[atower][abelt][axy][ibar];
    barsum+=bar[atower][abelt][axy][ibar];
  }
  TGraphErrors *barspec=new TGraphErrors(nbar[atower],x,y,xerr,yerr);
  TF1 *func=new TF1("elemag2","[2]*(0.5*[4]*[0]/pow((pow(x-[1],2.)+[0]),1.5)+0.5*(1-[4])*[3]/pow((pow(x-[1],2.)+[3]),1.5))+[7]*(0.5*[9]*[5]/pow((pow(x-[6],2.)+[5]),1.5)+0.5*(1-[9])*[8]/pow((pow(x-[6],2.)+[8]),1.5))",-1.,nbar[atower]+1.);
  barspec->Fit(func,"QNMC0","");

  double pmin=0.;
  double pmax=0.;

  func->SetParameter(0, 3.0);
  func->SetParLimits(0, 0.01, 5.);
  func->SetParameter(1, PeakPosition[atower][abelt][axy][0]);
  pmin=std::max(PeakPosition[atower][abelt][axy][0]-2.5, -1.);
  pmax=std::min(PeakPosition[atower][abelt][axy][0]+2.5, nbar[atower]+1.);
  func->SetParLimits(1, pmin, pmax);
  func->SetParameter(2, barsum*0.25);
  pmin=barsum*0.25;
  pmax=barsum*3.00;
  func->SetParLimits(2, pmin, pmax);
  func->SetParameter(3, 10.);
  func->SetParLimits(3, 5., 50.);
  func->SetParameter(4, 0.8);
  func->SetParLimits(4, 0., 1.);

  func->SetParameter(5, 3.0);
  func->SetParLimits(5, 0.01, 5.);
  func->SetParameter(6, PeakPosition[atower][abelt][axy][1]);
  pmin=std::max(PeakPosition[atower][abelt][axy][1]-2.5, -1.);
  pmax=std::min(PeakPosition[atower][abelt][axy][1]+2.5, nbar[atower]+1.);
  func->SetParLimits(6, pmin, pmax);
  func->SetParameter(7, barsum*0.25);
  pmin=barsum*0.05;
  pmax=barsum*1.50;
  func->SetParLimits(7, pmin, pmax);
  func->SetParameter(8, 10.);
  func->SetParLimits(8, 5., 50.);
  func->SetParameter(9, 0.8);
  func->SetParLimits(9, 0., 1.);

  TFitResultPtr r = barspec->Fit(func, "QRNME0S");

  if(1) {
    std::cout << "Peaks: " << std::endl;
    std::cout << "  Position: "
	      << PeakPosition[atower][abelt][axy][0]
	      << " Peak: " 
	      << PeakHeight[atower][abelt][axy][0]
	      << std::endl;
    for(int i=0; i<5; i++) 
      std::cout << "p" << i << "= " << func->GetParameter(i)
		<< " " <<  func->GetParError(i)
		<< std::endl;
    std::cout << "  Position: "
	      << PeakPosition[atower][abelt][axy][1]
	      << " Peak: " 
	      << PeakHeight[atower][abelt][axy][1]
	      << std::endl;
    for(int i=5; i<10; i++) 
      std::cout << "p" << i << "= " << func->GetParameter(i)
		<< " " <<  func->GetParError(i)
		<< std::endl;
    std::cout << "Chi2= " << r->Chi2() << std::endl;
    std::cout << "Edm = " << r->Edm()  << std::endl;
  }

  /// Minuit2, 1st turn
  std::cout << " Minuit2 1st turn" << std::endl;
  for(int ibar=0; ibar<nbar[atower]; ibar++) {
    positions.push_back(ibar+0.5);
    measurements.push_back(bar[atower][abelt][axy][ibar]);
    var.push_back(bar_err[atower][abelt][axy][ibar]);
  }
  ShowerLateralFunction2 fFCN(measurements, positions, var);
  MnUserParameters upar;
  upar.Add("p0", func->GetParameter(0), .01);
  upar.Add("p1", func->GetParameter(1), 0.1);
  upar.Add("p2", func->GetParameter(2), 1.0);
  upar.Add("p3", func->GetParameter(3), 1.0);
  upar.Add("p4", func->GetParameter(4), .01);
  upar.Add("p5", func->GetParameter(5), .01);
  upar.Add("p6", func->GetParameter(6), 0.1);
  upar.Add("p7", func->GetParameter(7), 1.0);
  upar.Add("p8", func->GetParameter(8), 1.0);
  upar.Add("p9", func->GetParameter(9), .01);

  // if an value is invalid, return 0.
  tmp=this->CheckValue(func->GetParameter(0), 0.);
  pmin=std::max(tmp*0.5, 0.01);
  pmax=std::max(tmp*1.5, 5.0);
  upar.SetLimits("p0", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(1), 0.);
  pmin=std::max(tmp-2.5, -1.);
  pmax=std::min(tmp+2.5, nbar[atower]+1.);
  upar.SetLimits("p1", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(2), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::max(tmp*1.5, 10.);
  upar.SetLimits("p2", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(3), 0.);
  pmin=std::min(tmp*0.5, this->CheckValue(func->GetParameter(0), 0.));
  pmax=std::max(tmp*1.5, 50.);
  upar.SetLimits("p3", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(4), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::min(tmp*1.5+1.e-3, 1.);
  upar.SetLimits("p4", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(5), 0.);
  pmin=std::max(tmp*0.5, 0.01);
  pmax=std::max(tmp*1.5, 5.0);
  upar.SetLimits("p5", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(6), 0.);
  pmin=std::max(tmp-2.5, -1.);
  pmax=std::min(tmp+2.5, nbar[atower]+1.);
  upar.SetLimits("p6", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(7), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::max(tmp*1.5, 10.);
  upar.SetLimits("p7", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(8), 0.);
  pmin=std::min(tmp*0.5, this->CheckValue(func->GetParameter(5), 0.));
  pmax=std::max(tmp*1.5, 50.);
  upar.SetLimits("p8", pmin, pmax);

  tmp=this->CheckValue(func->GetParameter(4), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::min(tmp*1.5+1.e-3, 1.);
  upar.SetLimits("p9", pmin, pmax);

  MnMigrad migrad(fFCN, upar);
  FunctionMinimum min0=migrad();
  bool validmin0=false;
  if(min0.IsValid() && !isnan(min0.Edm())) {
    std::cout << "=== minimum0  --> " << min0 << std::endl;
    validmin0=true;
  }

  if(1) {
    std::cout << "min0 is valid: " << min0.IsValid() << std::endl;
    if(min0.IsValid()) {
      for(int i=0; i<10; i++) {
	std::cout << "p" << i << "= " << min0.UserState().Value(i) << std::endl;
      }
      std::cout << "Chi2= " << min0.UserState().Fval() << std::endl;
      std::cout << "Edm = " << min0.UserState().Edm()  << std::endl;
    }
  }

  /// Minuit2, 2nd turn
  std::cout << " Minuit2 2nd turn" << std::endl;
  upar.Add("p0", min0.UserState().Value(0), .01);
  upar.Add("p1", min0.UserState().Value(1), 0.1);
  upar.Add("p2", min0.UserState().Value(2), 1.0);
  upar.Add("p3", min0.UserState().Value(3), 1.0);
  upar.Add("p4", min0.UserState().Value(4), .01);
  upar.Add("p5", min0.UserState().Value(5), .01);
  upar.Add("p6", min0.UserState().Value(6), 0.1);
  upar.Add("p7", min0.UserState().Value(7), 1.0);
  upar.Add("p8", min0.UserState().Value(8), 1.0);
  upar.Add("p9", min0.UserState().Value(9), .01);

  tmp=this->CheckValue(min0.UserState().Value(0), 0.);
  pmin=std::max(tmp*0.5, 0.01);
  pmax=std::max(tmp*1.5, 5.0);
  upar.SetLimits("p0", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(1), 0.);
  pmin=std::max(tmp-2.5, -1.);
  pmax=std::min(tmp+2.5, nbar[atower]+1.);
  upar.SetLimits("p1", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(2), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::max(tmp*1.5, 10.);
  upar.SetLimits("p2", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(3), 0.);
  pmin=std::min(tmp*0.5, CheckValue(min0.UserState().Value(0), 0.));
  pmax=std::max(tmp*1.5, 50.);
  upar.SetLimits("p3", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(4), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::min(tmp*1.5+1.e-3, 1.);
  upar.SetLimits("p4", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(5), 0.);
  pmin=std::max(tmp*0.5, 0.01);
  pmax=std::max(tmp*1.5, 5.0);
  upar.SetLimits("p5", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(6), 0.);
  pmin=std::max(tmp-2.5, -1.);
  pmax=std::min(tmp+2.5, nbar[atower]+1.);
  upar.SetLimits("p6", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(7), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::max(tmp*1.5, 10.);
  upar.SetLimits("p7", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(8), 0.);
  pmin=std::min(tmp*0.5, CheckValue(min0.UserState().Value(5), 0.));
  pmax=std::max(tmp*1.5, 50.);
  upar.SetLimits("p8", pmin, pmax);

  tmp=this->CheckValue(min0.UserState().Value(9), 0.);
  pmin=std::max(tmp*0.5, 0.);
  pmax=std::min(tmp*1.5+1.e-3, 1.);
  upar.SetLimits("p9", pmin, pmax);

  migrad.Fix("p0");
  FunctionMinimum min1=migrad();

  migrad.Release("p0");
  migrad.Fix("p1");
  FunctionMinimum min2=migrad();

  migrad.Release("p1");
  migrad.Fix("p2");
  FunctionMinimum min3=migrad();

  migrad.Release("p2");
  migrad.Fix("p3");
  FunctionMinimum min4=migrad();

  migrad.Release("p3");
  migrad.Fix("p4");
  FunctionMinimum min5=migrad();

  migrad.Release("p4");

  migrad.Fix("p5");
  FunctionMinimum min6=migrad();

  migrad.Release("p5");
  migrad.Fix("p6");
  FunctionMinimum min7=migrad();

  migrad.Release("p6");
  migrad.Fix("p7");
  FunctionMinimum min8=migrad();

  migrad.Release("p7");
  migrad.Fix("p8");
  FunctionMinimum min9=migrad();

  migrad.Release("p8");
  migrad.Fix("p9");
  FunctionMinimum min10=migrad();

  migrad.Release("p9");

  migrad.RemoveLimits("p0");
  migrad.RemoveLimits("p1");
  migrad.RemoveLimits("p2");
  migrad.RemoveLimits("p3");
  migrad.RemoveLimits("p4");
  migrad.RemoveLimits("p5");
  migrad.RemoveLimits("p6");
  migrad.RemoveLimits("p7");
  migrad.RemoveLimits("p8");
  migrad.RemoveLimits("p9");
  FunctionMinimum min11=migrad();
  bool validmin6=false;

  if(1) {
    if(min6.IsValid()) {
      std::cout << "min6 is valid: " << min6.IsValid() << std::endl;
      for(int i=0; i<10; i++) {
	std::cout << "p" << i << "= " << min6.UserState().Value(i) << std::endl;
      }
      std::cout << "Chi2= " << min6.UserState().Fval() << std::endl;
      std::cout << "Edm = " << min6.UserState().Edm()  << std::endl;
    }
  }
  barspec->Fit(func, "QNMC0", "");

  if(validmin6 &&
     min6.UserState().Value(0) > 0. &&
     min6.UserState().Value(2) > 0. &&
     min6.UserState().Value(3) > 0. &&
     min6.UserState().Value(4) > 0.) {
    // use min6
    std::cout << "  Use min6" << std::endl;

    Position[atower][abelt][axy].push_back(min6.UserState().Value(1));
    Rho1[atower][abelt][axy].push_back(min6.UserState().Value(0));
    Rho2[atower][abelt][axy].push_back(min6.UserState().Value(3));
    Norm[atower][abelt][axy].push_back(min6.UserState().Value(2));
    Frac[atower][abelt][axy].push_back(min6.UserState().Value(4));
    Position[atower][abelt][axy].push_back(min6.UserState().Value(6));
    Rho1[atower][abelt][axy].push_back(min6.UserState().Value(5));
    Rho2[atower][abelt][axy].push_back(min6.UserState().Value(8));
    Norm[atower][abelt][axy].push_back(min6.UserState().Value(7));
    Frac[atower][abelt][axy].push_back(min6.UserState().Value(9));
  }else{
    if(validmin0 &&
       min0.UserState().Value(0) > 0. &&
       min0.UserState().Value(2) > 0. &&
       min0.UserState().Value(3) > 0. &&
       min0.UserState().Value(4) > 0.) {
      // use min0
      std::cout << "  Use min0" << std::endl;

      Position[atower][abelt][axy].push_back(min0.UserState().Value(1));
      Rho1[atower][abelt][axy].push_back(min0.UserState().Value(0));
      Rho2[atower][abelt][axy].push_back(min0.UserState().Value(3));
      Norm[atower][abelt][axy].push_back(min0.UserState().Value(2));
      Frac[atower][abelt][axy].push_back(min0.UserState().Value(4));
      Position[atower][abelt][axy].push_back(min0.UserState().Value(6));
      Rho1[atower][abelt][axy].push_back(min0.UserState().Value(5));
      Rho2[atower][abelt][axy].push_back(min0.UserState().Value(8));
      Norm[atower][abelt][axy].push_back(min0.UserState().Value(7));
      Frac[atower][abelt][axy].push_back(min0.UserState().Value(9));
    }else{
      // use root
      std::cout << "  Use ROOT" << std::endl;
      Position[atower][abelt][axy].push_back(func->GetParameter(1));
      Rho1[atower][abelt][axy].push_back(func->GetParameter(0));
      Rho2[atower][abelt][axy].push_back(func->GetParameter(3));
      Norm[atower][abelt][axy].push_back(func->GetParameter(2));
      Frac[atower][abelt][axy].push_back(func->GetParameter(4));
      Position[atower][abelt][axy].push_back(func->GetParameter(6));
      Rho1[atower][abelt][axy].push_back(func->GetParameter(5));
      Rho2[atower][abelt][axy].push_back(func->GetParameter(8));
      Norm[atower][abelt][axy].push_back(func->GetParameter(7));
      Frac[atower][abelt][axy].push_back(func->GetParameter(9));
    }
  }
  delete func;
  delete barspec;

  delete[] x;
  delete[] xerr;
  delete[] y;
  delete[] yerr;
}
