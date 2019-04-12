
#include "model-percussion.h"

namespace autocomp
{

const char *ModelPercussion::model_name() const
{ return "percussion"; }

int ModelPercussion::generate(std::vector<PitchNote> &dst,
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
  if( beats == 4 )
    dst = src_figures;
  else
    {
      int k = 0;
      int32_t from_beat_in64;
      int32_t to_beat_in64;

      dst.clear();
      for(std::size_t i=0; i < (std::size_t)dst_barlen * 4; i++)
        {
          if( i % 4 == 2 )
            {
              ++k;
              continue;
            }
          if( !i )
            {
              from_beat_in64 = 0;
              to_beat_in64 = 16 * (i + dst_offset + 1);
            }
          else
            {
              from_beat_in64 = 16 * (i + dst_offset);
              to_beat_in64 = 16 * (i + dst_offset + 1);
            }
          for(std::size_t j=0; j < src_figures.size(); j++)
            {
              if( from_beat_in64 <= src_figures[j].start && src_figures[j].start < to_beat_in64 )
                {
                  dst.push_back(PitchNote(src_figures[j].pitch, src_figures[j].velocity,
                                          src_figures[j].start - 16 * k,
                                          src_figures[j].end - 16 * k));
                }
            }
        }
    }
  return 0;
}

}
