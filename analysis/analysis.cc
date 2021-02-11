#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TDatabasePDG.h"

#include "RHICfSimEvent.hpp"
#include "Tool.hh"

namespace fs=boost::filesystem;

using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::parse_command_line;
using boost::program_options::notify;
using boost::lexical_cast;

typedef boost::char_separator< char > separator;
typedef boost::tokenizer< separator >::iterator iterator;

using namespace std;

int main(int argc, char **argv)
{
  options_description opt("For Help");
  opt.add_options()
    ("help,h", "Display help")
    ("output,o",   value<string>(),   "Output file")
    ("indir,i",    value<fs::path>(), "Input directory")
    ("first,f",    value<int>(),      "First file number")
    ("last,l",     value<int>(),      "Last file number")
    ("position,p", value<double>(),   "Detector position")
    ;

  variables_map vm;
  store(parse_command_line(argc, argv, opt), vm);
  notify(vm);

  if(vm.count("help")   || !vm.count("position") ||
     !vm.count("first") || !vm.count("last") ||
     !vm.count("indir") || !vm.count("indir")) {
    cerr << opt << endl;
    exit(1);
  }
  
  string outname = vm["output"].as<string>();
  fs::path indir = vm["indir"].as<fs::path>();
  int first      = vm["first"].as<int>();
  int last       = vm["last"].as<int>();
  double height  = vm["position"].as<double>();

  TFile* fout=new TFile(outname.c_str(), "recreate");

  char hname[256];

  TH1D *h_eventcounter=new TH1D("eventcounter","eventcounter",10,0,10);

  TH1D *h_pdg=new TH1D("pdg","pdg",20000,-10000,10000);

  TH1D *h_energy_pi0[3];
  TH1D *h_energy_photon[3];
  TH1D *h_energy_neutron[3];
  TH1D *h_pt_pi0[3];
  TH1D *h_pt_photon[3];
  TH1D *h_pt_neutron[3];
  for(int i=0; i<3; i++) {
    sprintf(hname, "energy_pi0_%d", i);
    h_energy_pi0[i]=new TH1D(hname,hname,300,0,300);
    sprintf(hname, "pt_pi0_%d", i);
    h_pt_pi0[i]=new TH1D(hname,hname,300,0,3);
    sprintf(hname, "energy_photon_%d", i);
    h_energy_photon[i]=new TH1D(hname,hname,300,0,300);
    sprintf(hname, "pt_photon_%d", i);
    h_pt_photon[i]=new TH1D(hname,hname,300,0,3);
    sprintf(hname, "energy_neutron_%d", i);
    h_energy_neutron[i]=new TH1D(hname,hname,300,0,300);
    sprintf(hname, "pt_neutron_%d", i);
    h_pt_neutron[i]=new TH1D(hname,hname,300,0,3);
  }

  TH2D *h_pi0_acceptance[2];
  TH2D *h_eta_acceptance[2];
  for(int i=0; i<2; i++) {
    sprintf(hname, "pi0_acceptance_%d", i);
    h_pi0_acceptance[i]=new TH2D(hname,hname,300,0,300,300,0,3);
    sprintf(hname, "eta_acceptance_%d", i);
    h_eta_acceptance[i]=new TH2D(hname,hname,300,0,300,300,0,3);
  }

  TH1D *h_multiplicity[2];
  TH1D *h_energyflux[2];
  for(int i=0; i<2; i++) {
    sprintf(hname, "multiplicity%d", i);
    h_multiplicity[i] = new TH1D(hname, hname, 300, -15, 15);
    sprintf(hname, "energyflux%d", i);
    h_energyflux[i] = new TH1D(hname, hname, 300, -15, 15);
  }

  TH1D *h_photon[ntower];
  TH1D *h_neutron[ntower];
  for(int itower=0; itower<ntower; itower++) {
    sprintf(hname, "photon_tow%d",itower);
    h_photon[itower]=new TH1D(hname,hname,300,0,300);
    sprintf(hname, "neutron_tow%d",itower);
    h_neutron[itower]=new TH1D(hname,hname,300,0,300);
  }

  TH1D *h_photon_diffractive[ntower][6];
  TH1D *h_neutron_diffractive[ntower][6];
  for(int i=0; i<6; i++) {
    for(int itower=0; itower<ntower; itower++) {
      sprintf(hname, "photon_diffractive_tow%d_%d",itower,i);
      h_photon_diffractive[itower][i]=new TH1D(hname,hname,300,0,300);
      sprintf(hname, "neutron_diffractive_tow%d_%d",itower,i);
      h_neutron_diffractive[itower][i]=new TH1D(hname,hname,300,0,300);
    }
  }


  TChain* tevent=new TChain("EventInfo","");
  for(int ifile=first; ifile<=last; ifile++) {
    fs::path afile("run"+boost::lexical_cast<string>(ifile)+".root");
    string fname=(fs::absolute(indir) / afile).string();
    if(0) cout << "Add " << fname << endl;
    tevent->Add(fname.c_str());
  }
  tevent->SetMakeClass(1);

  CentralContainer* centralCont=new CentralContainer();
  ForwardContainer* forwardCont=new ForwardContainer();
  BBCContainer* bbcCont=new BBCContainer();
  ReconstructContainer* reconCont=new ReconstructContainer();

  tevent->SetBranchAddress("central", &centralCont);
  tevent->SetBranchAddress("forward", &forwardCont);
  tevent->SetBranchAddress("bbc", &bbcCont);
  //  tevent->SetBranchAddress("mc", &mcCont);
  //  tevent->SetBranchAddress("zdc", &zdcCont);
  tevent->SetBranchAddress("reconstruct", &reconCont);

  vector<Central*> central;
  vector<Forward*> forward;
  vector<vector<BBC*> > bbc;
  vector<vector<double> > plate;
  vector<vector<vector<vector<double> > > > bar;
  vector<double> fc;

  vector<int> zdc;
  vector<vector<double> > smd;

  int nevent;
  nevent=tevent->GetEntries();

  cout << nevent << endl;

  vector<vector<vector<vector<double> > > > position;

  for(int ievent=0; ievent<nevent; ievent++) {
    if(ievent%1000==0) cout << ievent << endl;
    tevent->GetEntry(ievent);

    central=centralCont->GetContainer();
    forward=forwardCont->GetContainer();

    plate=reconCont->GetPlate();
    bar=reconCont->GetBar();
    fc=reconCont->GetFC();

    zdc=reconCont->GetNphoton();
    smd=reconCont->GetSMD();

    position=reconCont->GetPosition();

    h_eventcounter->Fill(0);

    /// Central
    for(unsigned int i=0; i<central.size(); i++) {
      if(central[i]->isBoundary()) {
	h_multiplicity[0]->Fill(central[i]->GetMomentum().Eta());
	h_energyflux[0]->Fill(central[i]->GetMomentum().Eta(),central[i]->GetMomentum().E());
	int pid=central[i]->GetPDGcode();
	if(pid<10000 && TDatabasePDG::Instance()->GetParticle(pid)->Charge()/3==0) {
	  h_multiplicity[1]->Fill(central[i]->GetMomentum().Eta());
	  h_energyflux[1]->Fill(central[i]->GetMomentum().Eta(),central[i]->GetMomentum().E());
	}
      }

      if(central[i]->GetPosition2().z()<0) continue;

      h_pdg->Fill(central[i]->GetPDGcode());

      TVector3 proj=GetProjection(central[i]->GetPosition2(),
				  central[i]->GetMomentum());
      int hit=CheckHit(proj.x(), proj.y(), height, 0);

      if(central[i]->GetPDGcode()==111) {
	h_energy_pi0[0]->Fill(central[i]->GetMomentum().E());
	h_pt_pi0[0]->Fill(central[i]->GetMomentum().Pt());
	if(central[i]->GetMomentum().Eta()>0 &&
	   central[i]->GetMomentum().Eta()<3) {
	  h_energy_pi0[1]->Fill(central[i]->GetMomentum().E());
	  h_pt_pi0[1]->Fill(central[i]->GetMomentum().Pt());
	}else if(central[i]->GetMomentum().Eta()>6.0) {
	  h_energy_pi0[2]->Fill(central[i]->GetMomentum().E());
	  h_pt_pi0[2]->Fill(central[i]->GetMomentum().Pt());
	}
      }
      if(central[i]->GetPDGcode()==22 &&
	 central[i]->isBoundary()) {
	h_energy_photon[0]->Fill(central[i]->GetMomentum().E());
	h_pt_photon[0]->Fill(central[i]->GetMomentum().Pt());
	if(central[i]->GetMomentum().Eta()>0 &&
	   central[i]->GetMomentum().Eta()<3) {
	  h_energy_photon[1]->Fill(central[i]->GetMomentum().E());
	  h_pt_photon[1]->Fill(central[i]->GetMomentum().Pt());
	}else if(central[i]->GetMomentum().Eta()>6.0) {
	  h_energy_photon[2]->Fill(central[i]->GetMomentum().E());
	  h_pt_photon[2]->Fill(central[i]->GetMomentum().Pt());
	}
	if(hit!=2 && central[i]->GetMomentum().Z()>0) {
	  h_photon[hit]->Fill(central[i]->GetMomentum().E());
	  h_photon_diffractive[hit][centralCont->GetProcess()-101]->Fill(central[i]->GetMomentum().E());
	}
      }
      if(central[i]->GetPDGcode()==2112 &&
	 central[i]->isBoundary()) {
	h_energy_neutron[0]->Fill(central[i]->GetMomentum().E());
	h_pt_neutron[0]->Fill(central[i]->GetMomentum().Pt());
	if(central[i]->GetMomentum().Eta()>0 &&
	   central[i]->GetMomentum().Eta()<3) {
	  h_energy_neutron[1]->Fill(central[i]->GetMomentum().E());
	  h_pt_neutron[1]->Fill(central[i]->GetMomentum().Pt());
	}else if(central[i]->GetMomentum().Eta()>6.0) {
	  h_energy_neutron[2]->Fill(central[i]->GetMomentum().E());
	  h_pt_neutron[2]->Fill(central[i]->GetMomentum().Pt());
	}
	if(hit!=2 && central[i]->GetMomentum().Z()>0) {
	  h_neutron[hit]->Fill(central[i]->GetMomentum().E());
	  h_neutron_diffractive[hit][centralCont->GetProcess()-101]->Fill(central[i]->GetMomentum().E());
	}
      }     
    }

    for(unsigned int i=0; i<central.size(); i++) {
      if(central[i]->GetPDGcode()==111) {
	int type=Pi0Type(central, i, height);
	if(type!=0) {
	  vector<Central*> pair=GetPhotonPair(central,i);
	  h_pi0_acceptance[type-1]->Fill(pair[2]->GetMomentum().E(), pair[2]->GetMomentum().Pt());
	}
      }
      if(central[i]->GetPDGcode()==221) {
	int type=Pi0Type(central, i, height);
	if(type!=0) {
	  vector<Central*> pair=GetPhotonPair(central,i);
	  h_eta_acceptance[type-1]->Fill(pair[2]->GetMomentum().E(), pair[2]->GetMomentum().Pt());
	}
      }
    }
  }

  fout->Write();
  fout->Close();

  cout << "Finish" << endl;

  return 0;
}

