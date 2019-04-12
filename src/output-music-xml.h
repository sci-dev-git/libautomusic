#ifndef OUTPUT_MUSIC_XML_H
#define OUTPUT_MUSIC_XML_H

#include <fstream>
#include <vector>

#include "output-base.h"

struct tsf;

namespace autocomp
{

class OutputMusicXML : public OutputBase
{
public:
  OutputMusicXML();
  ~OutputMusicXML();
  
public:
  virtual float tick_64p() const { return 1.0f; }
  virtual int outputPrepare(std::ofstream &stream, int beat_time, int beats, float tempo);
  virtual int outputTracks(std::ofstream &stream, const std::vector<OutputBase::Track> &sequence);
  virtual int outputFinal(std::ofstream &stream);

protected:
  int getNoteIndex(int duration);

private:
  int m_beat_time;
  int m_beats;
  float m_tempo;
};

}

#endif
