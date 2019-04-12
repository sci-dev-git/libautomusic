#ifndef MODEL_CHORD_H
#define MODEL_CHORD_H

#include "model-base.h"

namespace autocomp
{

class ModelChord : public ModelBase
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

protected:
  int transform_figure_guitar(std::vector<PitchNote> &dst, const std::vector<PitchNote> &src_figures);
};

}

#endif
