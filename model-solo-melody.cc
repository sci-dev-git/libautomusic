
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

  static bool wr = false;
  if(!wr)
    {
    wr=true;
    printf("key %d %d\n", src_track_key, key);
    FILE *fp = fopen("./figure.txt", "w");
    fprintf(fp, "[");
    for(std::size_t i=0; i < src_figure.size(); i++)
      {
        fprintf(fp, "[%d,%d,%d,%d]%s", src_figure[i].pitch, src_figure[i].start, src_figure[i].end, src_figure[i].velocity, i==src_figure.size()-1 ? "]]" : ",");
      }
    fclose(fp);

    fp = fopen("./chords.txt", "w");
    fprintf(fp, "[");
    for(std::size_t i=0; i < src_chords.size(); i++)
      {
        fprintf(fp, "'%s'%s", theory::chord_get_name(src_chords[i]).c_str(), i==src_chords.size()-1 ? "]\n" : ",");
      }
    fprintf(fp, "[");
      for(std::size_t i=0; i < dst_chords.size(); i++)
        {
          fprintf(fp, "'%s'%s", theory::chord_get_name(dst_chords[i]).c_str(), i==dst_chords.size()-1 ? "]" : ",");
        }
    fclose(fp);

    printf("parm = %d %d %d %d %d %d", new_offset, src_track_key, dst_barlen, key, scale, beats);
    }

  std::vector<PitchNote> dst_rhythm_figures;
  if(int err = transform_rhythm_solo(dst_rhythm_figures, src_figure, current_rhythm_figures, current_rhythm_barlen, dst_barlen, dst_form_id, src_chords) )
    return err;

  std::vector<PitchNote> dst_figures;

  if( int err = theory::transform_figure_chord(dst_figures, src_track_key, src_chords, dst_rhythm_figures, dst_barlen, key, dst_chords, scale, new_offset, beats) )
    return err;

  return transform_figure_solo(dst, dst_figures, dst_barlen, new_offset, dst_chords, key, scale, beats);
  return 0;
}

}
