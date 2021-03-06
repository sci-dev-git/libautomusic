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
#include <algorithm>

#include "libautomusic.h"

#include "output-midi.h"
#include "output-pcm-audio.h"
#include "output-music-xml.h"
#include "output-base.h"

namespace autocomp
{

Output::Output() : m_numOutputMod(0)
{
  m_outputInstances[m_numOutputMod++] = new OutputMIDI;
  m_outputInstances[m_numOutputMod++] = new OutputPcmAudio;
  m_outputInstances[m_numOutputMod++] = new OutputMusicXML;
}

Output::~Output()
{
  while (m_numOutputMod)
    delete m_outputInstances[--m_numOutputMod];
}

static int trackCompositionChainToNotes(std::vector<OutputBase::Event> &dst,
                                 const std::vector<CompositionChainNode *> &compositionChainTrack, int beats)
{
  dst.clear();
  for(std::size_t form_index=0; form_index < compositionChainTrack.size(); form_index++)
    {
      if(compositionChainTrack[form_index]->pitch.size())
        {
          int offset = int(compositionChainTrack[form_index]->form.begin() * beats - compositionChainTrack[form_index]->offset) * 2 * 2 * 2 * 2;
          for(std::size_t j=0; j < compositionChainTrack[form_index]->pitch.size(); j++)
            {
              const PitchNote &note = compositionChainTrack[form_index]->pitch[j];
              if( note.start + offset >= 0 )
                {
                  OutputBase::Event event;
                  event.pitch = note.pitch;
                  event.velocity = note.velocity;

                  event.off = false; /* Note on */
                  event.tick = note.start + offset;
                  event.duration = note.end - note.start;
                  dst.push_back(event);
                  event.off = true; /* Note off */
                  event.tick = note.end + offset;
                  dst.push_back(event);
                }
            }
        }
    }
  return 0;
}

static bool sequence_cmp(const OutputBase::Event &a, const OutputBase::Event &b)
{
  return a.tick < b.tick;
}

int Output::outputCompositionChain(const std::string &filename,
                                   int filetype,
                                   const std::vector<std::vector<CompositionChainNode *>> &compositionChain,
                                   const std::vector<int> &timbres,
                                   const std::vector<int> &figureBanks, const std::vector<int> &figureClasses,
                                   int beat_type, int beats, float tempo)
{
  using namespace autocomp;
  if (filetype < 0 || filetype >= m_numOutputMod)
    return -RC_FAILED; /* Unknown file type (unknown output module index) */

  /* Open/create the target file */
  int rc = -RC_OPENFILE;
  std::ofstream stream(filename.c_str(), std::ofstream::binary);
  if( stream.is_open() )
    {
      OutputBase *outputInstance = m_outputInstances[filetype];

      if( (rc = outputInstance->outputPrepare(stream, beat_type, beats, tempo)) )
        return rc;

      std::vector<OutputBase::Track> sequence;
      for(std::size_t track=0; track < compositionChain.size(); track++)
        {
          OutputBase::Track trackseq;
          /* Wrap the basic parameter of this track */
          trackseq.gm_timbre = timbres[track];
          trackseq.figureBank = figureBanks[track];
          trackseq.figureClass = figureClasses[track];

          /* Get the list of sorted note events */
          if( int rc = trackCompositionChainToNotes(trackseq.events, compositionChain[track], beats) )
            return rc;
          quantifyNoteSequence(trackseq.events, outputInstance->tick_64p());
          std::sort(trackseq.events.begin(), trackseq.events.end(), sequence_cmp);

          sequence.push_back(trackseq);
        }
      /* Start to write the output file. */
      if( (rc = outputInstance->outputTracks(stream, sequence)) )
        return rc;

      rc = outputInstance->outputFinal(stream);
    }
  return rc;
}

void Output::quantifyNoteSequence(std::vector<OutputBase::Event> &dstSequence, float tick_64p)
{
  std::size_t seqlen = dstSequence.size();
  for(std::size_t j=0; j<seqlen; j++)
    {
      dstSequence[j].tick = int(dstSequence[j].tick * tick_64p);
      dstSequence[j].velocity = MIN(127, dstSequence[j].velocity);
    }
}

}
