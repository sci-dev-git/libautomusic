#include <stdio.h>
#include "knowledge-model.h"
#include "parameter-generator.h"
#include "model-base.h"
#include "theory-structure.h"
#include "theory-orchestration.h"
#include "util-randomize.h"
#include "composition-toplevel.h"

namespace autocomp
{

CompositionToplevel::CompositionToplevel()
    : m_knowledgeModel(new KnowledgeModel),
      m_parameterGenerator(new ParameterGenerator(m_knowledgeModel)),
      m_modelLibrary(new ModelLibrary),
      m_rhythm_knowledge_entry(0l)
{
}

int CompositionToplevel::startup(int character, int genre, int beats)
{
  /*
   * Generator global parameters, i.e. key, scale, beats, chords, development structure and
   * instrument manifest, which is the foundation of the composition.
   */
  if( int err = m_parameterGenerator->gen(character, genre, beats) )
    return err;

  std::size_t num_instrument = m_parameterGenerator->timbreBanks().size();
  const std::vector<int> &figure_banks = m_parameterGenerator->figureBanks();
  const std::vector<int> &figure_classes = m_parameterGenerator->figureClasses();

  std::vector<const KnowledgeEntry *> candidate_knowledge_entries;
  std::vector<const KnowledgeEntry *> secondary_candidate_knowledge_entries;

  if( int err = m_knowledgeModel->getKnowledgeEntry(candidate_knowledge_entries, character, genre) )
    return err;
  if( int err = m_knowledgeModel->getKnowledgeEntry(secondary_candidate_knowledge_entries, character) )
    return err;

  /*
   * Generator the extremely primitive rhythm from the selected knowledge entries.
   */
  if( !m_rhythm_knowledge_entry )
    {
      std::vector<const KnowledgeEntry *> exclude_list;
      exclude_list.push_back(m_parameterGenerator->chordKnowledgeEntry());
      for(std::size_t i=0; i <m_exclude_rhythm_entries.size(); i++)
        exclude_list.push_back(m_exclude_rhythm_entries[i]);

      std::vector<const KnowledgeEntry *> candidate_rhythm_entries;
      generateCandidateList(candidate_rhythm_entries, candidate_knowledge_entries, secondary_candidate_knowledge_entries, exclude_list);
      narrowRhythm(candidate_rhythm_entries);
      m_rhythm_knowledge_entry = util::random_choice(candidate_rhythm_entries);
    }

  m_melody_rhythm_array_entries.clear();
  m_solo_rhythm_array_entries.clear();

  for(std::size_t i=0; i <m_rhythm_knowledge_entry->m_knowledgeArrayEntries.size(); i++)
    {
      const KnowledgeArrayEntry *knowledgeArrayEntry = m_rhythm_knowledge_entry->m_knowledgeArrayEntries[i];
      if( theory::FIGURE_CLASS_SOLO == knowledgeArrayEntry->figure_class )
        {
          if( theory::FIGURE_BANK_MELODY == knowledgeArrayEntry->figure_bank )
            m_melody_rhythm_array_entries.push_back(knowledgeArrayEntry);
          else
            m_solo_rhythm_array_entries.push_back(knowledgeArrayEntry);
        }
    }

  /*
   * Generate primitive figures for each tracks, mainly depended on orchestration theories
   */
  m_figure_entries.clear();
  m_figure_keys.clear();
  std::vector<const KnowledgeArrayEntry *> used_figure_banks;

  for(std::size_t i=0; i < num_instrument; i++)
    {
      if( m_timbre_knowledge_entries.size() > i && m_timbre_knowledge_entries[i] )
        {
          const std::vector<const FigureListEntry *> *figures = 0l;
          const KnowledgeEntry *figure_knowledge_entry = m_timbre_knowledge_entries[i];

          if( int err = getUnusedTimbreFigures(&figures, figure_knowledge_entry, figure_banks[i], figure_classes[i], used_figure_banks) )
            return err;

          m_figure_entries.push_back(figures);
          m_figure_keys.push_back(figure_knowledge_entry->key);
        }
      else
        {
          std::vector<const KnowledgeEntry *> exclude_list;

          exclude_list.push_back(m_parameterGenerator->chordKnowledgeEntry());
          exclude_list.push_back(m_rhythm_knowledge_entry);
          for(std::size_t j=0; j < m_timbre_knowledge_entries.size(); j++)
            exclude_list.push_back(m_timbre_knowledge_entries[j]);
          for(std::size_t j=0; j < MIN(i, m_exclude_figure_entries.size()); j++)
            exclude_list.push_back(m_exclude_figure_entries[j]);

          std::vector<const KnowledgeEntry *> candidate_timbre_entries;
          generateCandidateList(candidate_timbre_entries,
                                candidate_knowledge_entries, secondary_candidate_knowledge_entries,
                                exclude_list);

          int track;
          const KnowledgeEntry *timbre_knowledge_entry = 0l;
          if( int err = theory::get_timbre_figures(&timbre_knowledge_entry, &track, candidate_timbre_entries, figure_banks[i], figure_classes[i]) )
              return err;

          if( m_timbre_knowledge_entries.size() > i )
            m_timbre_knowledge_entries[i] = timbre_knowledge_entry;
          else
            m_timbre_knowledge_entries.push_back(timbre_knowledge_entry);

          const std::vector<const FigureListEntry *> *figures = &timbre_knowledge_entry->m_knowledgeArrayEntries[track]->figure_list;
          m_figure_entries.push_back(figures);
          m_figure_keys.push_back(timbre_knowledge_entry->key);
        }
    }

  /*
   * Startup composition process
   * Deep-copy all the chain nodes from parameter generator as the input of following composition.
   */
  m_compositionChainTracks.clear();
  for(std::size_t i=0; i < m_parameterGenerator->timbreBanks().size(); i++)
    {
      m_compositionChainTracks.push_back(std::vector<CompositionChainNode *>());
      for(std::size_t j=0; j < m_parameterGenerator->chains().size(); j++)
        {
          m_compositionChainTracks.back().push_back(new CompositionChainNode(*(m_parameterGenerator->chains()[j])));
        }
    }

  for(std::size_t i=0; i < m_timbre_knowledge_entries.size(); i++)
    {
      int track_index = i;
      const std::vector<CompositionChainNode *> compositionChain = m_compositionChainTracks[track_index];

      int track_key = this->trackFigureKeys()[track_index];
      int track_figure_bank = m_parameterGenerator->figureBanks()[track_index];
      int track_figure_classes = m_parameterGenerator->figureClasses()[track_index];
      const std::vector<const FigureListEntry *> &track_figures_entries = *(trackFigureEntries()[track_index]);

      /*
       * Composition for each structure form.
       */
      for(std::size_t j=0; j < compositionChain.size(); j++)
        {
          CompositionChainNode *compositionNode = compositionChain[j];
          StructureForm::FormType dst_form_type = compositionNode->form.type();
          const FigureListEntry *src_figure = theory::pick_form(dst_form_type, track_figures_entries);

          int src_bars = src_figure->end - src_figure->begin;
          int src_offset = src_figure->offset;
          const std::vector<ChordPair> &src_chords = src_figure->chord;
          const std::vector<PitchNote> &src_figures = src_figure->pitchs;

          int dst_bars = compositionNode->form.end() - compositionNode->form.begin();
          dst_form_type = compositionNode->form.type();
          const std::vector<ChordPair> &dst_chords = compositionNode->chords;
          if( src_figures.size() == 0 )
            continue;
          if( dst_form_type == StructureForm::FORM_BLANK )
            continue;
          int dst_offset = src_offset;

          compositionNode->offset = dst_offset;

          std::vector<ChordPair> stretched_chords;
          std::vector<PitchNote> stretched_figures;
          if( int err = theory::stretch_chord_sequence(stretched_chords, src_chords, src_bars, dst_bars) )
            return err;
          if( int err = theory::stretch_figure_sequence(stretched_figures, src_figures, src_bars, dst_bars) )
            return err;

          /*
           * Invoke a corresponding model that is appropriate to the current instrument track
           */
          printf("%d begin %d %d\n", track_index, track_figure_bank, track_figure_classes);
          ModelBase *modelInstance = m_modelLibrary->invokeModel(track_figure_bank, track_figure_classes);
          if( int err = modelInstance->generate(compositionNode->pitch, this,
                                                track_figure_bank, stretched_chords, stretched_figures,
                                                dst_form_type, dst_chords, dst_offset, dst_bars,
                                                track_key,
                                                m_parameterGenerator->key(), m_parameterGenerator->scale(),
                                                beats) )
          {
            return err;
          }

          static bool wr = false;

          if(!wr && track_figure_bank==8 && track_figure_classes==0)
            {
              wr = true;
              printf("tans/n");
              FILE *fp = fopen("./figure.txt", "w");
              fprintf(fp, "[");
              for(std::size_t i=0; i < compositionNode->pitch.size(); i++)
                {
                  fprintf(fp, "[%d,%d,%d,%d]%s", compositionNode->pitch[i].pitch, compositionNode->pitch[i].start, compositionNode->pitch[i].end, compositionNode->pitch[i].velocity, i==compositionNode->pitch.size()-1 ? "]" : ",");
                }
              fclose(fp);
            }
        }
    }
  return 0;
}

float CompositionToplevel::tempo() const
{
  return m_parameterGenerator->chordKnowledgeEntry()->tempo;
}

int CompositionToplevel::loadModel(const char *filename)
{ return m_knowledgeModel->loadModel(filename); }

void CompositionToplevel::generateCandidateList(std::vector<const KnowledgeEntry *> &dst,
                          const std::vector<const KnowledgeEntry *> &primary_candidate_list,
                          const std::vector<const KnowledgeEntry *> &secondary_candidate_list,
                          const std::vector<const KnowledgeEntry *> &exclude_list)
{
  bool do_exclude = true;
  dst.clear();

generate:
  for(unsigned int i=0; i < 10; i++)
    for(std::size_t j=0; j < primary_candidate_list.size(); j++)
      dst.push_back(primary_candidate_list[j]);
  for(std::size_t i=0; i < secondary_candidate_list.size(); i++)
    dst.push_back(secondary_candidate_list[i]);

  if(do_exclude)
    {
  for(std::size_t i=0; i < exclude_list.size(); i++)
    for(std::vector<const KnowledgeEntry *>::iterator iter = dst.begin(); iter != dst.end(); )
      {
        if( *iter == exclude_list[i] )
          dst.erase(iter);
        else
          iter++;
      }
    }
  if( dst.size() == 0 )
    {
      do_exclude = false;
      goto generate;
    }
}

void CompositionToplevel::narrowRhythm(std::vector<const KnowledgeEntry *> &dst_entries)
{
  for(std::vector<const KnowledgeEntry *>::iterator iter=dst_entries.begin(); iter != dst_entries.end(); )
    {
      const KnowledgeEntry *knowledgeEntry = *iter;
      if( !knowledgeEntry->for_rhythm )
        dst_entries.erase(iter);
      else
        iter++;
    }
}

/**
 * @brief Select a figure entry from the list according to the figure bank and class,
 * ensuring it is not selected before as far as possible .
 */
int CompositionToplevel::getUnusedTimbreFigures(const std::vector<const FigureListEntry *> **dst,
                                             const KnowledgeEntry *knowledge_entry,
                                             int figure_bank, int figure_class,
                                             std::vector<const KnowledgeArrayEntry *> used_entries)
{
  const std::vector<const FigureListEntry *> *figure_list = 0l;
  for(std::size_t i=0; i < knowledge_entry->m_knowledgeArrayEntries.size(); i++)
    {
      const KnowledgeArrayEntry *entry = knowledge_entry->m_knowledgeArrayEntries[i];
      if( figure_bank == entry->figure_bank && figure_class == entry->figure_class )
        {
          bool unused = true;
          for(std::size_t j=0; j < used_entries.size(); j++)
            if( entry == used_entries[j] )
              { unused = false; break; }
          if( unused )
            {
              figure_list = &knowledge_entry->m_knowledgeArrayEntries[i]->figure_list;
              used_entries.push_back(entry);
              break;
            }
        }
    }

  /*
   * Checking if there is not any item meet the condition and
   * attempting to do the selection within the whole knowledge library when needed.
   */
  if( !figure_list )
    {
      int track;
      const KnowledgeEntry *knowledgeEntry = 0l;

      std::vector<const KnowledgeEntry *> knowledgeEntries;
      for(std::size_t i=0; i < m_knowledgeModel->models().size(); i++)
        knowledgeEntries.push_back(m_knowledgeModel->models()[i]);

      if( int err = theory::get_timbre_figures(&knowledgeEntry, &track, knowledgeEntries, figure_bank, figure_class) )
        return err;

      *dst = &knowledgeEntry->m_knowledgeArrayEntries[track]->figure_list;
    }
  else
    {
      *dst = figure_list;
    }

  return 0;
}

}
