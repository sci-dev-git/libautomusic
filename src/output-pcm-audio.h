#ifndef OUTPUT_PCM_AUDIO_H
#define OUTPUT_PCM_AUDIO_H

#include <fstream>
#include <vector>

#include "output-base.h"

struct tsf;

namespace autocomp
{

class OutputPcmAudio : public OutputBase
{
public:
  OutputPcmAudio();
  ~OutputPcmAudio();
  
public:
  virtual float tick_64p() const { return m_time_64p; }
  virtual int outputPrepare(std::ofstream &stream, int beat_time, int beats, float tempo);
  virtual int outputTracks(std::ofstream &stream, const std::vector<OutputBase::Track> &sequence);
  virtual int outputFinal(std::ofstream &stream);

private:
  tsf *m_tsf;
  float m_time_64p;
};

}

#endif
