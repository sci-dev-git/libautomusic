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
