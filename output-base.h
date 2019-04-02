#ifndef OUTPUT_BASE_H
#define OUTPUT_BASE_H

#include <vector>
#include <string>

#include "typedefs.h"

namespace autocomp
{
class OutputBase
{
public:
  virtual ~OutputBase()
  {}

public:
  virtual float tick_64p() const=0;
  virtual int outputPrepare(std::ofstream &stream)=0;
  virtual int outputTrack(std::ofstream &stream, const std::vector<PitchNote> &sequence, float tempo, int gm_timbre)=0;
  virtual int outputFinal(std::ofstream &stream)=0;
};

class Output
{
public:
  Output();

  int outputCompositionChain(const std::string &filename,
                             const std::vector<std::vector<CompositionChainNode *>> &compositionChain,
                             const std::vector<int> &timbres,
                             int beats, float tempo);
  void quantifyNoteSequence(std::vector<PitchNote> &dstSequence, float tick_64p);

};

}

#endif
