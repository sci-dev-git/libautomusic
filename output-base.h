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

  struct Event {
    uint8_t pitch;
    uint8_t velocity;
    int32_t tick;
    bool off;
  };

  struct Track {
    int gm_timbre;
    std::vector<Event> events;
  };

public:
  virtual float tick_64p() const=0;
  virtual int outputPrepare(std::ofstream &stream, float tempo)=0;
  virtual int outputTracks(std::ofstream &stream, const std::vector<Track> &sequence, float tempo)=0;
  virtual int outputFinal(std::ofstream &stream)=0;
};

#define MAX_OUTPUT_MODS 2 /* the number of output modules */

class Output
{
public:
  Output();
  ~Output();

  int outputCompositionChain(const std::string &filename,
                             int filetype,
                             const std::vector<std::vector<CompositionChainNode *>> &compositionChain,
                             const std::vector<int> &timbres,
                             int beats, float tempo);
  void quantifyNoteSequence(std::vector<OutputBase::Event> &dstSequence, float tick_64p);

private:
  OutputBase *m_outputInstances[MAX_OUTPUT_MODS];
  int m_numOutputMod;
};

}

#endif
