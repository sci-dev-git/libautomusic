
#include "knowledge-model.h"
#include "theory-harmonics.h"
#include "theory-structure.h"
#include "theory-orchestration.h"
#include "util-randomize.h"
#include "parameter-generator.h"

namespace autocomp
{

ParameterGenerator::ParameterGenerator(KnowledgeModel *knowledgeModel)
    : m_knowledgeModel(knowledgeModel),
      m_current_chord_knowledge_entry(0l),
      m_current_timbre_knowledge_entry(0l),
      m_key(-1),
      m_scale(-1),
      m_beats(0),
      m_character(-1),
      m_genre(-1)
{
}

int ParameterGenerator::gen(int character, int genre, int beats)
{
  using namespace std;

  /*
   * Generate candidate chords based on music character.
   * Above all else generating a compatible chord development is the beginning of our work.
   * This will select in-key chords as far as possible...
   */
  if( int err = m_knowledgeModel->getChord(m_candidate_chord_knowledge_entries, (m_character = character)) )
    return err;

  for(vector<const KnowledgeEntry *>::iterator iter = m_candidate_chord_knowledge_entries.begin();
      iter != m_candidate_chord_knowledge_entries.end(); )
    {
      if( (*iter) == m_current_chord_knowledge_entry )
        m_candidate_chord_knowledge_entries.erase(iter);
      else
        iter++;
    }

  std::vector<const KnowledgeEntry *> penality_list;
  std::vector<const FigureListEntry *> figure_list;

  for(std::size_t i=0; i < m_candidate_chord_knowledge_entries.size(); i++)
    {
      const KnowledgeEntry *knowledgeEntry = m_candidate_chord_knowledge_entries[i];

      const vector<const FigureListEntry *> &figureList = knowledgeEntry->m_knowledgeArrayEntries[0]->figure_list;
      int key = knowledgeEntry->key;
      int scale = knowledgeEntry->scale;
      int out_chord_num = 0;
      int chord_num = 0;
      for(std::size_t j=0; j <figureList.size(); j++)
        {
          const FigureListEntry *figure = figureList[j];

          for(std::size_t k=0; k < figure->chord.size(); k++)
            {
              if( !theory::chord_is_in_key(figure->chord[k], key, scale) )
                ++out_chord_num;
            }
          chord_num += figure->chord.size();
        }
      if( float(out_chord_num) / chord_num <= 1.0 / 20 )
        {
          penality_list.push_back(knowledgeEntry);
        }
    }

  if( penality_list.size() > 10 )
      m_current_chord_knowledge_entry = util::random_choice(penality_list);
  else
      m_current_chord_knowledge_entry = util::random_choice(m_candidate_chord_knowledge_entries);

  for(std::size_t i=0; i < m_current_chord_knowledge_entry->m_knowledgeArrayEntries[0]->figure_list.size(); i++)
    {
      figure_list.push_back(m_current_chord_knowledge_entry->m_knowledgeArrayEntries[0]->figure_list[i]);
    }

  /*
   * Generate key mode and time-beat for the whole music works, which is based on the selected chords.
   */
  m_key = m_current_chord_knowledge_entry->key;
  m_scale = m_current_chord_knowledge_entry->scale;

  /*
   * Generate development structure.
   * After it acquired a structure, coordinate chords with each forms.
   */
  if( int err = theory::get_form_template(m_forms, 7) )
    return err;

  int bar_pos = 0;
  for(std::size_t i=0; i < m_forms.size(); i++)
    {
      m_forms[i].setBegin(bar_pos);
      m_forms[i].setEnd( (bar_pos += m_forms[i].bars()) );
    }
  if( m_forms[0].type() != StructureForm::FORM_BLANK )
    m_forms.push_back(StructureForm(StructureForm::FORM_BLANK, 1, 0, 1));

  if( int rc = coordinateChordWithFormChain(m_chains, m_forms, figure_list, m_key, m_scale) )
    return rc;

  int dst_beats = beats;
  m_beats = 4;

  for(std::size_t i=0; i < m_chains.size(); i++)
    {
      const std::vector<ChordPair> &ori_chord_list = m_chains[i]->chords;
      if( m_beats == 4 && dst_beats == 3 )
        {
          std::vector<ChordPair> new_chord_list;
          for(std::size_t j=0; j < ori_chord_list.size(); j += 4)
            {
              for(unsigned int k=0; k < 3; k++)
                new_chord_list.push_back(ori_chord_list[j + 2]);
            }
          m_chains[i]->chords = new_chord_list;
        }
      else if( m_beats == 3 && dst_beats == 4 )
        {
          std::vector<ChordPair> new_chord_list;
          for(std::size_t j=0; j < ori_chord_list.size(); j += 3)
            {
              for(unsigned int k = j; k < j + 3; k++)
                new_chord_list.push_back(ori_chord_list[k]);
              new_chord_list.push_back(ori_chord_list[j + 2]);
            }
          m_chains[i]->chords = new_chord_list;
        }
    }

  m_beats = dst_beats;

  /*
   * Generate instrument table for the whole work
   */
  if( int rc = m_knowledgeModel->getTimbreBank(m_candidate_timbre_knowledge_entries, (m_genre = genre)) )
    return rc;

  for(vector<const KnowledgeEntry *>::iterator iter = m_candidate_timbre_knowledge_entries.begin();
        iter != m_candidate_timbre_knowledge_entries.end(); )
    {
      if( (*iter) == m_current_timbre_knowledge_entry )
        m_candidate_timbre_knowledge_entries.erase(iter);
      else
        iter++;
    }

  if( m_candidate_timbre_knowledge_entries.size() )
    {
      m_current_timbre_knowledge_entry = util::random_choice(m_candidate_timbre_knowledge_entries);
    }
  else
    return -1;

  std::vector<int> timbre_banks;
  std::vector<int> figure_banks;
  std::vector<int> figure_classes;

  if( int err = theory::get_timbres(m_current_timbre_knowledge_entry, timbre_banks, figure_banks, figure_classes) )
    return err;
  if( int err = theory::layout_timbres(m_timbre_banks, m_figure_banks, m_figure_classes,
                                       timbre_banks, figure_banks, figure_classes, false) )
    return err;

  return 0;
}


int ParameterGenerator::coordinateChordWithFormChain(std::vector<FormChainNode *> &dst,
                                      const std::vector<StructureForm> &forms,
                                      const std::vector<const FigureListEntry *> &src_figures,
                                      int key, int scale /*= 0*/)
{
  dst.clear();
  for(std::size_t i=0; i < forms.size(); i++)
    {
      const StructureForm &form = forms[i];
      StructureForm::FormType new_form_index = form.type();

      if( new_form_index == StructureForm::FORM_BLANK )
        continue;

      const FigureListEntry *target_figure;
      bool found = false;
      /* Firstly seeing if the target form type is already existing in forms vector. */
      for(std::size_t j=0; j < src_figures.size(); j++)
        {
          if( int(new_form_index) == src_figures[j]->segment )
            {
              target_figure = src_figures[j];
              found = true;
              break;
            }
        }

      if( !found )
        {
          /* Not found, trying it with replacement rules. */
          const StructureForm::FormType *candidate_form = theory::form_replacement_rules[form.type()];

          for(unsigned int j=0; candidate_form[j] != StructureForm::FORM_INVALID; j++)
            {
              for(std::size_t k=0; k < src_figures.size(); k++)
                {
                  if( int(candidate_form[j]) == src_figures[k]->segment )
                    {
                      target_figure = src_figures[k];
                      found = true;
                      break;
                    }
                }
            }
        }
      if( !found )
        {
          /* Not matched, randomly choose one from vector instead of making other efforts... */
          target_figure = util::random_choice(src_figures);
        }

      int src_barlen = target_figure->end - target_figure->begin;
      int dst_barlen = form.end() - form.begin();

      FormChainNode *chainNode = new FormChainNode;
      if( int err = theory::stretch_chord_sequence(chainNode->chords, target_figure->chord, src_barlen, dst_barlen) )
        {
          delete chainNode;
          return err;
        }
      chainNode->figure = target_figure;
      chainNode->form = form;
      chainNode->key = key;
      chainNode->offset = target_figure->offset;

      /*
       * Coordinate the regular chords with ending or interlude forms to make it not deviate from the theme,
       * that's to say, not deviate from the main chord or tonic...
       */
      if( i + 1 < forms.size() )
        {
          StructureForm::FormType next_form_index = forms[i + 1].type();
          if( next_form_index == StructureForm::FORM_ENDING ||
              next_form_index == StructureForm::FORM_INTERLUDE1 || next_form_index == StructureForm::FORM_INTERLUDE2 )
            {
              unsigned int chordlast = chainNode->chords.size();
              if( chainNode->chords[chordlast-3] != chainNode->chords[chordlast-2] )
                chainNode->chords[chordlast-2] = chainNode->chords[chordlast-1] = ChordPair(key, scale);
              else
                {
                  chainNode->chords[chordlast-4] = chainNode->chords[chordlast-3] = ChordPair(key, scale);
                  chainNode->chords[chordlast-2] = chainNode->chords[chordlast-1] = ChordPair(key, scale);
                }
            }
        }
      dst.push_back(chainNode);
    }

  if( dst.size() > 1 && dst[0]->form.type() == StructureForm::FORM_BLANK )
    {
      const ChordPair &main_chord = dst[1]->chords[0];
      dst[0]->chords.clear();
      for(unsigned int i=0; i < 4; i++)
        dst[0]->chords.push_back(main_chord);
    }

  return 0;
}

}
