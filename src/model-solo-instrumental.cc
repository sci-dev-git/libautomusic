
#include "theory-harmonics.h"
#include "theory-structure.h"
#include "knowledge-model.h"
#include "composition-toplevel.h"
#include "libautomusic.h"
#include "util-randomize.h"
#include "util-math.h"
#include "model-solo-instrumental.h"

namespace autocomp
{

ModelSoloInstrumental::ModelSoloInstrumental()
{}

const char *ModelSoloInstrumental::model_name() const
{ return "solo-instrumental"; }

int ModelSoloInstrumental::generate(std::vector<PitchNote> &dst,
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
  for(std::size_t i=0; i < composition->soloRhythmEntries().size(); i++)
    {
      const FigureListEntry *form = theory::pick_form(dst_form_id, composition->soloRhythmEntries()[i]->figure_list);
      candidate_rhythm_list.push_back(form);
    }
  const FigureListEntry *current_form = candidate_rhythm_list[0];
  for(std::size_t i=0; i < candidate_rhythm_list.size(); i++)
    {
      if( candidate_rhythm_list[i]->pitchs.size() )
        {
          current_form = candidate_rhythm_list[i];
          break;
        }
    }

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

/**
 * @brief Transform the source figures into new figures according to current rhythm.
 */
int ModelSoloInstrumental::transform_rhythm_solo(std::vector<PitchNote> &dst,
                          const std::vector<PitchNote> &src_figures,
                          const std::vector<PitchNote> &src_rhythm_figures,
                          int src_rhythm_figures_barlen, int barlen,
                          const std::vector<ChordPair> &chord_list,
                          int beats /*= 4*/)
{
  std::vector<PitchNote> current_rhythm_figures;
  if( src_rhythm_figures_barlen != barlen )
    {
      if( int err = theory::stretch_figure_sequence(current_rhythm_figures, src_rhythm_figures, src_rhythm_figures_barlen, barlen) )
        return err;
    }
  else
    current_rhythm_figures = src_rhythm_figures;

  dst.clear();

  for(int i=0; i < barlen; i++)
    {
      std::vector<PitchNote> reg_rhythm_figure_list;
      std::vector<PitchNote> reg_pitch_figure_list;

      /*
       * Data Window
       * Only does it process notes that is in the current bar.
       */
      int32_t bar_start = i * 2 * 2 * 2 * 2 * beats;
      int32_t bar_end = (i + 1) * 2 * 2 * 2 * 2 * beats;
      for(std::size_t j=0; j < src_figures.size(); j++)
        {
          if( bar_start <= src_figures[j].start && src_figures[j].start < bar_end )
            reg_pitch_figure_list.push_back(src_figures[j]);
        }
      for(std::size_t j=0; j < current_rhythm_figures.size(); j++)
        {
          if( bar_start <= current_rhythm_figures[j].start && current_rhythm_figures[j].start < bar_end )
            reg_rhythm_figure_list.push_back(current_rhythm_figures[j]);
        }

      for(std::size_t j=0; j < reg_rhythm_figure_list.size(); j++)
        {
          PitchNote &rhythm_figure = reg_rhythm_figure_list[j];
          int32_t start_figure = rhythm_figure.start;
          int32_t end_figure = rhythm_figure.end;

          std::vector<PitchNote> candidate_pitch_figure_list;
          for(std::size_t k=0; k < reg_pitch_figure_list.size(); k++)
            {
              const PitchNote &pitch_figure = reg_pitch_figure_list[k];
              if( pitch_figure.end > start_figure || end_figure < pitch_figure.start )
                candidate_pitch_figure_list.push_back(pitch_figure);
            }

          if( candidate_pitch_figure_list.size() )
            {
              if( candidate_pitch_figure_list.size() == 1 )
                  rhythm_figure.pitch = candidate_pitch_figure_list[0].pitch;
              else
                {
                  /*
                   * Statistics the quantity of in-chord notes and point out the average pitch.
                   * The average pitch will be considered as the center (main) pitch.
                   */
                  std::vector<int> in_chord_index;
                  int sum_pitch_no = 0;
                  for(std::size_t k=0; k < candidate_pitch_figure_list.size(); k++)
                    {
                      int pitch_no = candidate_pitch_figure_list[k].pitch;
                      if(theory::pitch_is_in_chord(pitch_no, chord_list[i]))
                        in_chord_index.push_back(k);
                      sum_pitch_no += pitch_no;
                    }
                  int avg_pitch_no = int(float(sum_pitch_no) / candidate_pitch_figure_list.size());

                  /*
                   * Set the pitch of current note as the nearest pitch related to the center pitch,
                   * which eliminates the possibility that rhythm becomes out of tone.
                   */
                  if( in_chord_index.size() == 1 )
                      rhythm_figure.pitch = candidate_pitch_figure_list[in_chord_index[0]].pitch;
                  else if( in_chord_index.size() > 1 )
                    {
                      int nearest_pitch_no = avg_pitch_no;
                      int nearest_pitch_dis = 129;
                      for(std::size_t m=0; m < in_chord_index.size(); m++)
                        {
                          int index = in_chord_index[m];
                          if( ABS(candidate_pitch_figure_list[index].pitch - avg_pitch_no) < nearest_pitch_dis )
                            {
                              nearest_pitch_dis = ABS(candidate_pitch_figure_list[index].pitch - avg_pitch_no);
                              nearest_pitch_no = candidate_pitch_figure_list[index].pitch;
                            }
                        }
                        rhythm_figure.pitch = nearest_pitch_no;
                    }
                  else
                    {
                      int nearest_pitch_no = avg_pitch_no;
                      int nearest_pitch_dis = 129;
                      for(std::size_t index=0; index < candidate_pitch_figure_list.size(); index++)
                        {
                          const PitchNote &figure_pitch = candidate_pitch_figure_list[index];
                          if( ABS(figure_pitch.pitch - avg_pitch_no) < nearest_pitch_dis )
                            {
                              nearest_pitch_dis = ABS(figure_pitch.pitch - avg_pitch_no);
                              nearest_pitch_no = figure_pitch.pitch;
                            }
                        }
                        rhythm_figure.pitch = nearest_pitch_no;
                    }
                }
            }
          if( (candidate_pitch_figure_list.size() == 0 || j == reg_rhythm_figure_list.size() - 1) && reg_pitch_figure_list.size() > 0 )
            rhythm_figure.pitch = reg_pitch_figure_list[reg_pitch_figure_list.size()-1].pitch;
        }

      for(std::size_t j=0; j < reg_rhythm_figure_list.size(); j++)
        dst.push_back(reg_rhythm_figure_list[j]);
    }
  return 0;
}

int ModelSoloInstrumental::transform_figure_solo(std::vector<PitchNote> &dst,
                          const std::vector<PitchNote> &src_figures,
                          int barlen,
                          int offset,
                          const std::vector<ChordPair> &chord_list,
                          int key, int scale, int beats /*= 4*/)
{
  int32_t offset_64 = offset * 2 * 2 * 2 * 2;
  int sum_note_pitch = 0;
  int avg_pitch_no = 0;
  if( src_figures.size() > 0 )
    {
      for(std::size_t i=0; i < src_figures.size(); i++)
        sum_note_pitch += src_figures[i].pitch;
      avg_pitch_no = int(float(sum_note_pitch) / src_figures.size());
    }

  dst.clear();
  for(int n=0; n < barlen; n +=2 )
    {
      std::vector<PitchNote> reg_figure_list;
      int32_t reg_start = n * 2 * 2 * 2 * 2 * beats;
      int32_t reg_end = (n + 2) * 2 * 2 * 2 * 2 * beats;
      for(std::size_t j=0; j < src_figures.size(); j++)
        {
          PitchNote a_figure = src_figures[j];
          if( reg_start <= a_figure.start && a_figure.start < reg_end )
            {
              if( a_figure.end > reg_end )
                  a_figure.end = reg_end;
              if( j + 1 < src_figures.size() && a_figure.end > src_figures[j + 1].start )
                  a_figure.end = src_figures[j + 1].start;
              if( a_figure.start < a_figure.end )
                  reg_figure_list.push_back(a_figure);
            }
        }
      if( reg_figure_list.size() == 0 )
        continue;
      if( avg_pitch_no != 0 )
        {
          reg_figure_list[0].pitch = theory::pitch_get_in_octave_1(reg_figure_list[0].pitch, avg_pitch_no);
        }
      dst.push_back(reg_figure_list[0]);
      for(std::size_t i=1; i < reg_figure_list.size(); i++)
          reg_figure_list[i].pitch = theory::pitch_get_in_octave_1(reg_figure_list[i].pitch, avg_pitch_no);

      for(std::size_t i=1; i < reg_figure_list.size() - 1; i++)
        {
          int pre_pitch = reg_figure_list[i - 1].pitch;
          int pre_start = reg_figure_list[i - 1].start, pre_end = reg_figure_list[i - 1].end;
          int cur_pitch = reg_figure_list[i].pitch;
          int cur_start = reg_figure_list[i].start, cur_end = reg_figure_list[i].end;
          int next_pitch = reg_figure_list[i + 1].pitch;
          int next_start = reg_figure_list[i + 1].start, next_end = reg_figure_list[i + 1].end;

          bool is_strong = false;
          if( ((cur_start >= offset_64) && (util::floor_mod(cur_start - offset_64, 32) < 2)) || util::floor_mod(cur_start - offset_64, 32) > 30 )
              is_strong = true;
          if( !is_strong )
            {
              if( ABS(pre_end - cur_start) <= 4 && ABS(cur_end - next_start) <= 4 &&
                  ABS(next_pitch - pre_pitch) >= 3 && ABS(next_pitch - pre_pitch) <= 4 &&
                  pre_end - pre_start >= cur_end - cur_start &&
                  next_end - next_start >= cur_end - cur_start )
                {
                  int pitch_dis = next_pitch - pre_pitch;
                  if( pitch_dis > 0 )
                      reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, 1, key, scale);
                  else if( pitch_dis < 0 )
                      reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, -1, key, scale);
                }
              /*
               * Randomly insert pitch variety, or insert when the development is too steady
               */
              else if( ABS(pre_end - cur_start) <= 4 && ABS(cur_end - next_start) <= 4 &&
                        next_pitch == pre_pitch && next_pitch == cur_pitch && pre_pitch == cur_pitch &&
                        (cur_start > (reg_figure_list[-1].end - reg_figure_list[0].start) / 4 + reg_figure_list[0].start ||
                        util::random_range(101) < 50) )
                {
                  if( ABS(avg_pitch_no - cur_pitch) > 5 )
                    {
                      if( cur_pitch < avg_pitch_no )
                          reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, 1, key, scale);
                      else
                          reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, -1, key, scale);
                    }
                  else
                    {
                      int rand_diff = util::random_range(101) < 50 ? -1 : 1;
                      reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, rand_diff, key, scale);
                    }
                }
              else if( ABS(pre_end - cur_start) < 4 && ABS(cur_end - next_start) < 4 &&
                      ((cur_pitch - next_pitch > 4 && cur_pitch - pre_pitch > 4) || (next_pitch - cur_pitch > 4 && pre_pitch - cur_pitch > 4) ) )
                {
                  if( reg_figure_list[i].pitch - MAX(pre_pitch, next_pitch) > 4 )
                    reg_figure_list[i].pitch = theory::pitch_get_in_scale(reg_figure_list[i].pitch, -2, key, scale);
                  if( reg_figure_list[i].pitch - MIN(pre_pitch, next_pitch) < -4 )
                    reg_figure_list[i].pitch = theory::pitch_get_in_scale(reg_figure_list[i].pitch, 2, key, scale);
                }
            }
          else
            {
              int cur_chord_index = int((cur_start - offset_64) / 2 / 2 / 2 / 2);
              const ChordPair &cur_chord = chord_list[cur_chord_index];
              reg_figure_list[i].pitch = theory::pitch_get_in_chord(cur_pitch, cur_chord);
              if( reg_figure_list[i].pitch - pre_pitch > 4 )
                reg_figure_list[i].pitch = theory::pitch_get_in_scale(reg_figure_list[i].pitch, -2, key, scale);
              else if( reg_figure_list[i].pitch - pre_pitch < -4 )
                reg_figure_list[i].pitch = theory::pitch_get_in_scale(reg_figure_list[i].pitch, 2, key, scale);
            }
          cur_pitch = reg_figure_list[i].pitch;
          if( !is_strong && ABS(pre_end - cur_start) < 4 && ABS(cur_end - next_start) < 4 &&
              !theory::pitch_is_in_chord(cur_pitch, ChordPair(key, scale)) &&
              !theory::pitch_is_in_chord(pre_pitch, ChordPair(key, scale)) )
            {
              if( cur_pitch < avg_pitch_no )
                  reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, 1, key, scale);
              else
                  reg_figure_list[i].pitch = theory::pitch_get_in_scale(pre_pitch, -1, key, scale);
            }
          dst.push_back(reg_figure_list[i]);
        }
      if( reg_figure_list.size() > 1 )
        {
          int last_chord_index = MIN((n + 2) * beats - 1, (int)chord_list.size() - 1);
          const ChordPair &last_chord = chord_list[last_chord_index];
          if( n + 2 >= barlen && reg_figure_list.size() > 2 && key == last_chord.root )
            reg_figure_list[reg_figure_list.size()-1].pitch = theory::pitch_get_in_chord(reg_figure_list[reg_figure_list.size()-1].pitch, last_chord, 0, avg_pitch_no);
          else
            reg_figure_list[reg_figure_list.size()-1].pitch = theory::pitch_get_in_chord(reg_figure_list[reg_figure_list.size()-1].pitch, last_chord);
          dst.push_back(reg_figure_list[reg_figure_list.size()-1]);
        }
    }
  return 0;
}

}
