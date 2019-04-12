#ifndef MODEL_INSTRUMENTAL_SOLO_H
#define MODEL_INSTRUMENTAL_SOLO_H

#include "model-base.h"

namespace autocomp
{

class ModelSoloInstrumental : public ModelBase
{
public:
  ModelSoloInstrumental();

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
  int transform_rhythm_solo(std::vector<PitchNote> &dst,
                            const std::vector<PitchNote> &src_figures,
                            const std::vector<PitchNote> &src_rhythm_figures,
                            int src_rhythm_figures_barlen, int barlen,
                            const std::vector<ChordPair> &chord_list,
                            int beats = 4);
  int transform_figure_solo(std::vector<PitchNote> &dst,
                            const std::vector<PitchNote> &src_figures,
                            int barlen,
                            int offset,
                            const std::vector<ChordPair> &chord_list,
                            int key, int scale, int beats = 4);
};

}

#endif
