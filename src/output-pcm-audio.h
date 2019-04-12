/*
 *  libautomusic (Library for Image-based Algorithmic Musical Composition)
 *  Copyright (C) 2018, automusic.
 *
 *  THIS PROJECT IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR
 *  MODIFY IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE(GPL)
 *  AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; EITHER VERSION 2.1
 *  OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 *
 *  THIS PROJECT IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 *  BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 *  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE GNU
 *  LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
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
