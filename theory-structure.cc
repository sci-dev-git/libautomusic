
#include <vector>
#include <cstdlib>

#include "util-randomize.h"
#include "knowledge-model.h"
#include "theory-structure.h"

namespace autocomp
{ namespace theory
  {

const StructureForm::FormType form_replacement_rules[][6] =
 {
   /* BLANK */    {StructureForm::FORM_VERSE11,  StructureForm::FORM_BLANK, StructureForm::FORM_INVALID},
   /* PRELUDE */  {StructureForm::FORM_INTERLUDE1, StructureForm::FORM_INVALID},
   /* VERSE11 */  {StructureForm::FORM_PRELUDE, StructureForm::FORM_INVALID},
   /* VERSE12 */  {StructureForm::FORM_VERSE11,  StructureForm::FORM_PRELUDE, StructureForm::FORM_INVALID},
   /* BRIDGE1 */  {StructureForm::FORM_VERSE11,  StructureForm::FORM_PRELUDE, StructureForm::FORM_INVALID},
   /* CHORUS11 */ {StructureForm::FORM_VERSE11,  StructureForm::FORM_BRIDGE1, StructureForm::FORM_INVALID},
   /* CHORUS12 */ {StructureForm::FORM_CHORUS11, StructureForm::FORM_VERSE11,   StructureForm::FORM_BRIDGE1, StructureForm::FORM_INVALID},
   /* INTERLU1 */ {StructureForm::FORM_PRELUDE,  StructureForm::FORM_VERSE11,   StructureForm::FORM_BRIDGE1, StructureForm::FORM_CHORUS11, StructureForm::FORM_INVALID},
   /* INTERLU2 */ {StructureForm::FORM_INTERLUDE1, StructureForm::FORM_PRELUDE, StructureForm::FORM_VERSE11, StructureForm::FORM_BRIDGE1, StructureForm::FORM_CHORUS11, StructureForm::FORM_INVALID},
   /* VERSE21 */  {StructureForm::FORM_VERSE11,  StructureForm::FORM_VERSE12, StructureForm::FORM_INVALID},
   /* VERSE22 */  {StructureForm::FORM_VERSE12,  StructureForm::FORM_VERSE11, StructureForm::FORM_INVALID},
   /* CHORUS21 */ {StructureForm::FORM_CHORUS11, StructureForm::FORM_CHORUS12, StructureForm::FORM_INVALID},
   /* CHORUS22 */ {StructureForm::FORM_CHORUS21, StructureForm::FORM_CHORUS12,  StructureForm::FORM_CHORUS11, StructureForm::FORM_INVALID},
   /* ENDING */   {StructureForm::FORM_PRELUDE,  StructureForm::FORM_INTERLUDE1, StructureForm::FORM_INTERLUDE2, StructureForm::FORM_INVALID},
   /* VERSE13 */  {StructureForm::FORM_VERSE11,  StructureForm::FORM_VERSE12, StructureForm::FORM_INVALID},
   /* VERSE23 */  {StructureForm::FORM_VERSE13,  StructureForm::FORM_VERSE11,  StructureForm::FORM_VERSE12, StructureForm::FORM_INVALID},
   /* VERSE31 */  {StructureForm::FORM_VERSE21,  StructureForm::FORM_VERSE11,  StructureForm::FORM_VERSE12, StructureForm::FORM_INVALID},
   /* VERSE32 */  {StructureForm::FORM_VERSE22,  StructureForm::FORM_VERSE31,  StructureForm::FORM_VERSE12, StructureForm::FORM_VERSE11, StructureForm::FORM_INVALID},
   /* VERSE33 */  {StructureForm::FORM_VERSE23,  StructureForm::FORM_ENDING,   StructureForm::FORM_VERSE11, StructureForm::FORM_VERSE12, StructureForm::FORM_INVALID},
   /* CHORUS31 */ {StructureForm::FORM_CHORUS21, StructureForm::FORM_CHORUS11, StructureForm::FORM_CHORUS12, StructureForm::FORM_INVALID},
   /* CHORUS32 */ {StructureForm::FORM_CHORUS22, StructureForm::FORM_CHORUS12, StructureForm::FORM_CHORUS11, StructureForm::FORM_INVALID},
   /* BRIDGE2 */  {StructureForm::FORM_BRIDGE1,  StructureForm::FORM_VERSE11,  StructureForm::FORM_PRELUDE, StructureForm::FORM_INVALID},
   /* TRANS1 */   {StructureForm::FORM_VERSE12,  StructureForm::FORM_VERSE11, StructureForm::FORM_INVALID},
   /* TRANS2 */   {StructureForm::FORM_TRANS1,   StructureForm::FORM_VERSE22,  StructureForm::FORM_VERSE21, StructureForm::FORM_VERSE12, StructureForm::FORM_VERSE11, StructureForm::FORM_INVALID},
   /* TRANS3 */   {StructureForm::FORM_TRANS2,   StructureForm::FORM_VERSE32,  StructureForm::FORM_VERSE31, StructureForm::FORM_VERSE12, StructureForm::FORM_VERSE11, StructureForm::FORM_INVALID},
   /* BRIDGE3 */  {StructureForm::FORM_BRIDGE2,  StructureForm::FORM_BRIDGE1,  StructureForm::FORM_VERSE11, StructureForm::FORM_PRELUDE, StructureForm::FORM_INVALID}
 };

struct form_template
{
  StructureForm::FormType type;
  int bars;
};

#define FORM_TEMPLATE_NUM 8
static const form_template form_templates[FORM_TEMPLATE_NUM][25] = {
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_CHORUS12, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK,1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_CHORUS12, 8},
    {StructureForm::FORM_INTERLUDE1, 8},
    {StructureForm::FORM_VERSE21, 8},
    {StructureForm::FORM_CHORUS21, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_INTERLUDE1, 8},
    {StructureForm::FORM_VERSE21, 8},
    {StructureForm::FORM_CHORUS21, 8},
    {StructureForm::FORM_VERSE22, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_TRANS1, 2},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_CHORUS12, 8},
    {StructureForm::FORM_INTERLUDE1, 8},
    {StructureForm::FORM_VERSE21, 8},
    {StructureForm::FORM_TRANS2, 2},
    {StructureForm::FORM_CHORUS21, 8},
    {StructureForm::FORM_CHORUS22, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_TRANS1, 2},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_CHORUS12, 8},
    {StructureForm::FORM_INTERLUDE1, 8},
    {StructureForm::FORM_VERSE21, 8},
    {StructureForm::FORM_TRANS2, 2},
    {StructureForm::FORM_CHORUS21, 8},
    {StructureForm::FORM_CHORUS22, 8},
    {StructureForm::FORM_INTERLUDE2, 8},
    {StructureForm::FORM_BRIDGE1, 8},
    {StructureForm::FORM_CHORUS31, 8},
    {StructureForm::FORM_CHORUS32, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_BRIDGE1, 8},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_CHORUS12, 8},
    {StructureForm::FORM_INTERLUDE1, 8},
    {StructureForm::FORM_VERSE21, 8},
    {StructureForm::FORM_BRIDGE2, 8},
    {StructureForm::FORM_CHORUS21, 8},
    {StructureForm::FORM_CHORUS22, 8},
    {StructureForm::FORM_INTERLUDE2, 8},
    {StructureForm::FORM_CHORUS31, 8},
    {StructureForm::FORM_CHORUS32, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  },
  {
    {StructureForm::FORM_BLANK, 1},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_PRELUDE, 8},
    {StructureForm::FORM_VERSE11, 8},
    {StructureForm::FORM_VERSE12, 8},
    {StructureForm::FORM_BRIDGE1, 8},
    {StructureForm::FORM_TRANS1, 2},
    {StructureForm::FORM_CHORUS11, 8},
    {StructureForm::FORM_CHORUS12, 8},
    {StructureForm::FORM_INTERLUDE1, 8},
    {StructureForm::FORM_VERSE21, 8},
    {StructureForm::FORM_VERSE22, 8},
    {StructureForm::FORM_BRIDGE2, 8},
    {StructureForm::FORM_TRANS1, 2},
    {StructureForm::FORM_CHORUS21, 8},
    {StructureForm::FORM_CHORUS22, 8},
    {StructureForm::FORM_INTERLUDE2, 8},
    {StructureForm::FORM_VERSE31, 8},
    {StructureForm::FORM_BRIDGE3, 8},
    {StructureForm::FORM_TRANS3, 2},
    {StructureForm::FORM_CHORUS31, 8},
    {StructureForm::FORM_CHORUS32, 8},
    {StructureForm::FORM_VERSE32, 8},
    {StructureForm::FORM_ENDING, 4},
    {StructureForm::FORM_INVALID, -1}
  }
};

/**
 * @brief Zoom (stretch out and draw back) the figure sequence.
 * This will do linear scale, deleting the redundant part or appending new part when needed.
 */
int stretch_figure_sequence(std::vector<PitchNote> &dst_figure_list,
                         const std::vector<PitchNote> &src_figure_list,
                         int src_barlen, int dst_barlen, int beats /*= 4*/)
{
  dst_figure_list = src_figure_list;

  int bar_diff = ABS(dst_barlen - src_barlen);
  if( src_barlen < dst_barlen )
    {
      int cur_balen = src_barlen;
      while( bar_diff > 0 )
        {
          int need_bar_num = MIN(bar_diff, src_barlen);
          int start_add_64note = (src_barlen - need_bar_num) * beats * 2 * 2 * 2 * 2;
          int offset = cur_balen * beats * 2 * 2 * 2 * 2;

          for(std::size_t i=0; i < src_figure_list.size(); i++)
            {
              const PitchNote &figure = src_figure_list[i];
              if( figure.start >= start_add_64note )
                {
                  dst_figure_list.push_back(PitchNote(figure.pitch,
                                                figure.velocity,
                                                figure.start - start_add_64note + offset,
                                                figure.end - start_add_64note + offset));
                }
            }

          cur_balen += need_bar_num;
          bar_diff = dst_barlen - cur_balen;
        }
    }
  else if( src_barlen > dst_barlen )
    {
      int32_t delete_from_64note = (src_barlen - bar_diff) * beats / 2 * 2 * 2 * 2 * 2;
      int32_t delete_to_64note = (src_barlen + bar_diff) * beats / 2 * 2 * 2 * 2 * 2;
      int offset_64note = bar_diff * beats * 2 * 2 * 2 * 2;

      std::vector<int> remove_index;
      for(std::size_t i=0; i < dst_figure_list.size(); i++)
        {
          const PitchNote &figure = src_figure_list[i];
          if( delete_from_64note <= figure.start && figure.start < delete_to_64note )
            remove_index.push_back(i);
          else if( figure.start >= delete_to_64note )
            {
              dst_figure_list[i].start -= offset_64note;
              dst_figure_list[i].end -= offset_64note;
            }
        }

      for(std::vector<int>::iterator iter = remove_index.begin(); iter != remove_index.end(); iter++)
        dst_figure_list.erase(dst_figure_list.begin() + *iter);
    }
  return 0;
}

/**
 * @brief Zoom (stretch out and draw back) the chord sequence.
 * Delete the redundant chords or append chords (repetitive in source list) when needed.
 */
int stretch_chord_sequence(std::vector<ChordPair> &dst,
                        const std::vector<ChordPair> &src_chord_list,
                        int src_barlen, int dst_barlen,
                        int beats /*= 4*/)
{
  int bar_diff = ABS(dst_barlen - src_barlen);

  if( src_barlen < dst_barlen )
    {
      dst = src_chord_list;

      while( bar_diff > 0 )
        {
          for(std::size_t i= src_chord_list.size()-(bar_diff * beats); i < src_chord_list.size(); i++)
            dst.push_back(src_chord_list[i]);

          bar_diff = dst_barlen - /* current bars */ (dst.size() / beats);
        }
    }
  else if( src_barlen > dst_barlen )
    {
      dst.clear();
      unsigned int boundary = src_barlen * beats / 2 - bar_diff * beats / 2;
      for(unsigned int i=0; i < boundary; i++)
        dst.push_back(src_chord_list[i]);

      for(std::size_t i=src_barlen * beats / 2 + bar_diff * beats / 2; i < src_chord_list.size(); i++)
        dst.push_back(src_chord_list[i]);
    }
  else /* no need to stretch */
    {
      dst = src_chord_list;
    }
  return 0;
}

int get_form_template(std::vector<StructureForm> &dst, unsigned int id)
{
  dst.clear();
  if( id < FORM_TEMPLATE_NUM )
    {
      for(unsigned int i=0; form_templates[id][i].type != StructureForm::FORM_INVALID; i++)
        dst.push_back(StructureForm(form_templates[id][i].type, form_templates[id][i].bars));
      return 0;
    }
  return -1;
}

const FigureListEntry *pick_form(StructureForm::FormType form, const std::vector<const FigureListEntry *> &forms_vector)
{
  /* Firstly seeing if the target form type is already existing in forms vector. */
  for(std::size_t i=0; i < forms_vector.size(); i++)
    if( int(form) == forms_vector[i]->segment )
      return forms_vector[i];

  /* Not found, trying it with replacement rules. */
  const StructureForm::FormType *candidate_form = form_replacement_rules[form];

  for(unsigned int i=0; candidate_form[i] != StructureForm::FORM_INVALID; i++)
    for(std::size_t j=0; j < forms_vector.size(); j++)
      if( int(candidate_form[i]) == forms_vector[j]->segment )
        return forms_vector[j];

  /* Not matched, randomly choose one from vector instead of making other efforts... */
  return util::random_choice(forms_vector);
}

/*
 * @brief Transform a 4/4 figure into 4/3, ensuring that there is no notes exceeding the subsection.
 */
int transform_figure_4_3(std::vector<PitchNote> &dst, const std::vector<PitchNote> &figures, int bars)
{
  dst.clear();
  for(int i=0; i < bars; i++)
    {
      int32_t from_beat_64 = i * 2 * 2 * 2 * 2 * 3;
      int32_t end_beat_64 = (i + 1) * 2 * 2 * 2 * 2 * 3;
      for(std::size_t j=0; j < figures.size(); j++)
        {
          if( from_beat_64 <= figures[j].start && figures[j].start < end_beat_64 )
            {
              dst.push_back(PitchNote(figures[j].pitch,
                                      figures[j].velocity,
                                      figures[j].start, MIN(figures[j].end, end_beat_64)));
            }
        }
    }
  return 0;
}

  }
}
