#ifndef COMPOSITION_TOPLEVEL_H
#define COMPOSITION_TOPLEVEL_H

#include "parameter-generator.h"

#include <vector>

namespace autocomp
{

class KnowledgeModel;
class KnowledgeEntry;
class KnowledgeArrayEntry;
class ParameterGenerator;
class ModelLibrary;
class CompositionToplevel
{
public:
  CompositionToplevel();

  int loadModel(const char *filename);
  int startup(int character, int genre, int beats);

  inline const std::vector<const KnowledgeArrayEntry *> &melodyRhythmEntries() const { return m_melody_rhythm_array_entries; }
  inline const std::vector<const KnowledgeArrayEntry *> &soloRhythmEntries() const { return m_solo_rhythm_array_entries; }
  inline const std::vector<const std::vector<const FigureListEntry *> *> &trackFigureEntries() const { return m_figure_entries; }
  inline const std::vector<int> &trackFigureKeys() const { return m_figure_keys; }
  inline const std::vector<std::vector<CompositionChainNode *>> &chains() const { return m_compositionChainTracks; }
  float tempo() const;

private:
  void generateCandidateList(std::vector<const KnowledgeEntry *> &dst,
                            const std::vector<const KnowledgeEntry *> &primary_candidate_list,
                            const std::vector<const KnowledgeEntry *> &secondary_candidate_list,
                            const std::vector<const KnowledgeEntry *> &exclude_list = std::vector<const KnowledgeEntry *> () );
  void narrowRhythm(std::vector<const KnowledgeEntry *> &dst_entries);
  int getUnusedTimbreFigures(const std::vector<const FigureListEntry *> **dst,
                          const KnowledgeEntry *knowledge_entry,
                          int figure_bank, int figure_class,
                          std::vector<const KnowledgeArrayEntry *> used_entries);

private:
  KnowledgeModel *m_knowledgeModel;
  ParameterGenerator *m_parameterGenerator;
  ModelLibrary *m_modelLibrary;
  const KnowledgeEntry *m_rhythm_knowledge_entry;
  std::vector<const KnowledgeArrayEntry *> m_melody_rhythm_array_entries;
  std::vector<const KnowledgeArrayEntry *> m_solo_rhythm_array_entries;
  std::vector<const std::vector<const FigureListEntry *> *> m_figure_entries;
  std::vector<int> m_figure_keys;

  std::vector<const KnowledgeEntry *> m_exclude_rhythm_entries;
  std::vector<const KnowledgeEntry *> m_exclude_figure_entries;
  std::vector<const KnowledgeEntry *> m_timbre_knowledge_entries;

  std::vector<std::vector<CompositionChainNode *>> m_compositionChainTracks;
};


}

#endif
