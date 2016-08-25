#include "BBCContainer.hpp"

#include <iostream>

BBCContainer::BBCContainer()
{
}

BBCContainer::~BBCContainer()
{
}

void BBCContainer::Reset()
{
  bbcCont.clear();
  bbcCont.resize(nside);
  for(int iside=0; iside<nside; iside++) 
    for(int ibbc=0; ibbc<nbbc[iside]; ibbc++)
      bbcCont[iside][ibbc]=new BBC();
}

void BBCContainer::SetBBC(int aside, int aid, BBC* abbc)
{
  bbcCont[aside][aid]=abbc;
}

std::vector< std::vector<BBC*> > BBCContainer::GetContainer()
{
  return bbcCont;
}
