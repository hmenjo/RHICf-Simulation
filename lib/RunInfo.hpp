#ifndef RUNINFO_HPP
#define RUNINFO_HPP

#include <cstring>

#include "TObject.h"
#include "Flag.hpp"

class RunInfo: public TObject
{
public:
  RunInfo();
  ~RunInfo();
  void SetRunNumber(int aRunNumber);
  void SetNumberOfEvents(int aNumberOfEvents);
  void SetModel(std::string aModel);
  void SetSeed1(int aSeed1);
  void SetSeed2(int aSeed2);
  void SetFlag(Flag aflag);

  int GetRunNumber();
  int GetNumberOfEvents();
  std::string GetModel();
  int GetSeed1();
  int GetSeed2();
  Flag GetFlag();

private:
  int RunNumber;
  int NumberOfEvents;
  std::string Model;
  int Seed1;
  int Seed2;
  Flag flag;

  ClassDef(RunInfo, 1)
};

#endif
