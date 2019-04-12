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
  int startup();

  inline const std::vector<const KnowledgeArrayEntry *> &melodyRhythmEntries() const { return m_melody_rhythm_array_entries; }
  inline const std::vector<const KnowledgeArrayEntry *> &soloRhythmEntries() const { return m_solo_rhythm_array_entries; }
  inline const std::vector<const std::vector<const FigureListEntry *> *> &trackFigureEntries() const { return m_figure_entries; }
  inline const std::vector<int> &trackFigureKeys() const { return m_figure_keys; }
  inline const std::vector<std::vector<CompositionChainNode *>> &chains() const { return m_compositionChainTracks; }
  inline ParameterGenerator *generator() { return m_parameterGenerator; }
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
