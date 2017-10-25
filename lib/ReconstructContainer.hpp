#ifndef RECONSTRUCTCONTAINER_HPP
#define RECONSTRUCTCONTAINER_HPP

#include "RecoDataContainer.hpp"
#include "ZDCContainer.hpp"
#include "Reconstruct.hpp"

class ReconstructContainer: public RecoDataContainer, public ZDCContainer, public Reconstruct
{
public:
  ReconstructContainer();
  ~ReconstructContainer();
  void Reset();

private:

  ClassDef(ReconstructContainer,1)
};

#endif
