#include <iostream>

#include "MCDataContainer.hpp"

MCDataContainer::MCDataContainer()
{
}

MCDataContainer::~MCDataContainer()
{
}


void MCDataContainer::Reset()
{
  RecoDataContainer::Reset();

  for(int i=0; i<2; i++) {
    bar_truth[i].clear();
    bar_truth[i].resize(ntower);
    for(int itower=0; itower<ntower; itower++) {
      bar_truth[i][itower].resize(nbelt);
      for(int ibelt=0; ibelt<nbelt; ibelt++) {
	bar_truth[i][itower][ibelt].resize(nxy);
	for(int ixy=0; ixy<nxy; ixy++) {
	  bar_truth[i][itower][ibelt][ixy].resize(nbar[itower]);
	}
      }
    }
  }

  plate_truth.clear();
  plate_truth.resize(ntower);
  for(int itower=0; itower<ntower; itower++) {
    plate_truth[itower].resize(nplate);
  }
}

void MCDataContainer::SetBarTruth(int atower, int abelt, int axy, int abar, double aedep1, double aedep2)
{
  bar_truth[0][atower][abelt][axy][abar]=aedep1;
  bar_truth[1][atower][abelt][axy][abar]=aedep2;
}

std::vector<std::vector<std::vector<std::vector<double> > > > MCDataContainer::GetBarTruth(int flag=0)
{
  if(flag==0) return bar_truth[0];
  else        return bar_truth[1];
}

void MCDataContainer::SetPlateTruth(int atower, int aplate, double aedep)
{
  plate_truth[atower][aplate]=aedep;
}

std::vector<std::vector<double> > MCDataContainer::GetPlateTruth()
{
  return plate_truth;
}

