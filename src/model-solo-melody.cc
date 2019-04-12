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
#include "theory-harmonics.h"
#include "theory-structure.h"
#include "knowledge-model.h"
#include "composition-toplevel.h"
#include "model-solo-melody.h"

namespace autocomp
{

const char *ModelSoloMelody::model_name() const
{ return "solo-melody"; }

int ModelSoloMelody::generate(std::vector<PitchNote> &dst,
                       CompositionToplevel *composition,
                       int src_figure_bank,
                       const std::vector<ChordPair> &src_chords,
                       const std::vector<PitchNote> &src_figure,
                       StructureForm::FormType dst_form_id,
                       const std::vector<ChordPair> &dst_chords,
                       int32_t dst_offset,
                       int32_t dst_barlen,
                       int src_track_key,
                       int key, int scale, int beats)
{
  dst.clear();

  /*
   * Select primitive rhythm figure from the top-level composition
   */
  std::vector<const FigureListEntry *> candidate_rhythm_list;
  for(std::size_t i=0; i < composition->melodyRhythmEntries().size(); i++)
    {
      const FigureListEntry *form = theory::pick_form(dst_form_id, composition->melodyRhythmEntries()[i]->figure_list);
      candidate_rhythm_list.push_back(form);
    }
  const FigureListEntry *current_form = candidate_rhythm_list[0];

  std::vector<PitchNote> current_rhythm_figures(current_form->pitchs);
  int current_rhythm_barlen = current_form->end - current_form->begin;
  int new_offset = current_form->offset;

  std::vector<PitchNote> dst_rhythm_figures;
  if(int err = transform_rhythm_solo(dst_rhythm_figures, src_figure, current_rhythm_figures, current_rhythm_barlen, dst_barlen, src_chords) )
    return err;

  std::vector<PitchNote> dst_figures;

  if( int err = theory::transform_figure_chord(dst_figures, src_track_key, src_chords, dst_rhythm_figures, dst_barlen, key, dst_chords, scale, new_offset, beats) )
    return err;

  return transform_figure_solo(dst, dst_figures, dst_barlen, new_offset, dst_chords, key, scale, beats);
}

}
