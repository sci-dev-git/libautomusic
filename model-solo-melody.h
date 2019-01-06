#ifndef MODEL_SOLO_MELODY_H
#define MODEL_SOLO_MELODY_H

#include "model-solo-instrumental.h"

namespace autocomp
{

class ModelSoloMelody : public ModelSoloInstrumental
{
public:
  virtual const char *model_name() const;
  virtual int generate(std::vector<PitchNote> &dst,
                       CompositionToplevel *composition,
                       int src_figure_bank,
                       const std::vector<ChordPair> &src_chords,
                       const std::vector<PitchNote> &src_figure,
                       StructureForm::FormType dst_form_id,
                       const std::vector<ChordPair> &dst_chords,
                       int32_t dst_offset,
                       int32_t dst_barlen,
                       int src_track_key,
                       int key, int scale,
                       int beats);

};

}

#endif
