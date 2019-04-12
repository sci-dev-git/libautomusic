
#include "theory-harmonics.h"
#include "theory-structure.h"
#include "theory-orchestration.h"
#include "model-chord.h"

namespace autocomp
{

const char *ModelChord::model_name() const
{ return "chord"; }

int ModelChord::generate(std::vector<PitchNote> &dst,
                     CompositionToplevel *composition,
                     int src_figure_bank,
                     const std::vector<ChordPair> &src_chords,
                     const std::vector<PitchNote> &src_figures,
                     StructureForm::FormType dst_form_id,
                     const std::vector<ChordPair> &dst_chords,
                     int32_t dst_offset,
                     int32_t dst_barlen,
                     int src_track_key,
                     int key, int scale,
                     int beats)
{
  dst.clear();

  std::vector<PitchNote> dst_figures;
  if( int err = theory::transform_figure_chord(dst_figures, src_track_key, src_chords, src_figures, dst_barlen, key, dst_chords, scale, 0, beats) )
    return err;

  if( beats == 3 && src_figure_bank == theory::FIGURE_BANK_GUITAR )
    {
      std::vector<PitchNote> mid_figures;
      if( int err = theory::transform_figure_4_3(mid_figures, dst_figures, dst_barlen) )
        return err;
      return transform_figure_guitar(dst, mid_figures);
    }
  else if( src_figure_bank == theory::FIGURE_BANK_GUITAR )
    {
      return transform_figure_guitar(dst, dst_figures);
    }
  else if( beats == 3 )
    {
      return theory::transform_figure_4_3(dst, dst_figures, dst_barlen);
    }
  else
    {
      dst = dst_figures;
      return 0;
    }
}

/*
 * @brief a special port for guitar figures.
 */
int ModelChord::transform_figure_guitar(std::vector<PitchNote> &dst, const std::vector<PitchNote> &src_figures)
{
  dst.clear();

  std::size_t start_index = 0;
  while( start_index < src_figures.size() )
    {
      const PitchNote &start_note = src_figures[start_index];
      std::vector<PitchNote> same_note_list;
      same_note_list.push_back(start_note);
      std::size_t same_pitch_count = 1;

      while( start_index + same_pitch_count < src_figures.size() )
        {
          const PitchNote &next_note = src_figures[start_index + same_pitch_count];
          if( next_note.start == start_note.start )
            {
              same_note_list.push_back(next_note);
              same_pitch_count += 1;
            }
          else
            break;
        }

      if( 2 < same_note_list.size() && same_note_list.size() <= 6 )
        {
          for(std::size_t i=0; i < same_note_list.size() - 1; i++)
            {
              for(std::size_t j=0; j < same_note_list.size() - i - 1; j++ )
                {
                  if( same_note_list[j].pitch > same_note_list[j + 1].pitch )
                    {
                      same_note_list[j].pitch = same_note_list[j + 1].pitch;
                      same_note_list[j + 1].pitch = same_note_list[j].pitch;
                    }
                }
            }
          for(std::size_t i=0; i < same_note_list.size(); i++)
            {
              same_note_list[i].start += int(i / 2);
            }
        }
      for(std::size_t j=0; j < same_note_list.size(); j++)
        dst.push_back(same_note_list[j]);
      start_index += same_pitch_count;
    }
  return 0;
}

}
