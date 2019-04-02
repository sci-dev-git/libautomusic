#ifndef PARAMETER_GENERATOR_H
#define PARAMETER_GENERATOR_H

#include <vector>

#include "typedefs.h"

namespace autocomp
{

class KnowledgeModel;
class KnowledgeEntry;
class FigureListEntry;

class ParameterGenerator
{
public:
  ParameterGenerator(KnowledgeModel *knowledgeModel);

public:
  int gen(int form_template_index, int character, int genre, int beats);
  int gen(const char *imageFilename, int form_template_index, int beats);
public:
  inline int key() const { return m_key; }
  inline int scale() const { return m_scale; }
  inline int beats() const { return m_beats; }
  inline int character() const { return m_character; }
  inline int genre() const { return m_genre; }
  inline const KnowledgeEntry *chordKnowledgeEntry() const { return m_current_chord_knowledge_entry; }
  inline const KnowledgeEntry *timbreKnowledgeEntry() const { return m_current_timbre_knowledge_entry; }
  inline const std::vector<int> &timbreBanks() const { return m_timbre_banks; }
  inline const std::vector<int> &figureBanks() const { return m_figure_banks; }
  inline const std::vector<int> &figureClasses() const { return m_figure_classes; }
  inline const std::vector<StructureForm> &forms() const { return m_forms; }
  inline std::vector<FormChainNode *> &chains() { return m_chains; }
  inline bool generated() { return m_generated; }

private:
  int gen_inner(int form_template_index, int character, int genre, int beats, int rand_seed, double chord_factor, double timbre_factor);
  int coordinateChordWithFormChain(std::vector<FormChainNode *> &dst,
                                   const std::vector<StructureForm> &forms,
                                   const std::vector<const FigureListEntry *> &src_chords,
                                   int key, int scale = 0);
private:
  KnowledgeModel *m_knowledgeModel;
  std::vector<const KnowledgeEntry *> m_candidate_chord_knowledge_entries;
  std::vector<const KnowledgeEntry *> m_candidate_timbre_knowledge_entries;
  const KnowledgeEntry *m_current_chord_knowledge_entry;
  const KnowledgeEntry *m_current_timbre_knowledge_entry;
  std::vector<int> m_timbre_banks;
  std::vector<int> m_figure_banks;
  std::vector<int> m_figure_classes;
  std::vector<StructureForm> m_forms;
  std::vector<FormChainNode *> m_chains;

  int m_key;
  int m_scale;
  int m_beats;
  int m_character;
  int m_genre;
  bool m_generated;
};

}

#endif
