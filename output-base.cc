
#include <algorithm>

#include "libautomusic.h"

#include "output-midi.h"
#include "output-base.h"

namespace autocomp
{

Output::Output()
{
}

static int trackCompositionChainToNotes(std::vector<PitchNote> &dst,
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
                  dst.push_back(PitchNote(note.pitch, note.velocity,
                                          note.start + offset,
                                          note.end + offset));
                }
            }
        }
    }
  return 0;
}

static bool sequence_cmp(const PitchNote &a, const PitchNote &b)
{
  return a.start < b.start;
}

int Output::outputCompositionChain(const std::string &filename, const std::vector<std::vector<CompositionChainNode *>> &compositionChain, int beats, float tempo)
{
  using namespace autocomp;
  int rc = -RC_OPENFILE;
  std::ofstream stream(filename.c_str(), std::ofstream::binary);

  if( stream.is_open() )
    {
      OutputMIDI *midi = new OutputMIDI;

      if( (rc = midi->outputPrepare(stream)) )
        return rc;

      for(std::size_t track=0; track < compositionChain.size(); track++)
        {
          std::vector<PitchNote> sequence;
          if( int rc = trackCompositionChainToNotes(sequence, compositionChain[track], beats) )
            return rc;

          quantifyNoteSequence(sequence, midi->tick_64p());

          std::sort(sequence.begin(), sequence.end(), sequence_cmp);

          if( (rc = midi->outputTrack(stream, sequence, tempo, 0)) )
            return rc;
        }

      rc = midi->outputFinal(stream);

      delete midi;
    }
  return rc;
}

void Output::quantifyNoteSequence(std::vector<PitchNote> &dstSequence, float tick_64p)
{
  std::size_t seqlen = dstSequence.size();
  for(std::size_t j=0; j<seqlen; j++)
    {
      dstSequence[j].start = int(dstSequence[j].start * tick_64p);
      dstSequence[j].end = int(dstSequence[j].end * tick_64p);
      dstSequence[j].velocity = MIN(127, dstSequence[j].velocity);
    }
}

}
