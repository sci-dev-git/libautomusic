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
#ifndef KNOWLEDGE_MODEL_H
#define KNOWLEDGE_MODEL_H

#include <string>
#include <list>
#include <vector>

#include "typedefs.h"

namespace autocomp
{

class FigureListEntry
{
public:
  FigureListEntry()
    : segment(0),
      offset(0),
      begin(0),
      end(0)
  {}
  void appendChord(const ChordPair &src_chord )
    {
      chord.push_back(src_chord);
    }
  void appendPitch(const PitchNote &src_pitch)
    {
      pitchs.push_back(src_pitch);
    }

public:
  std::vector<ChordPair> chord;
  int segment;
  int offset;
  unsigned int begin;
  unsigned int end;
  std::vector<PitchNote> pitchs;
};

class KnowledgeArrayEntry
{
public:
  KnowledgeArrayEntry()
    : timbre_bank(0),
      figure_bank(0),
      figure_class(0),
      figure_list(0l)
  {}

  ~KnowledgeArrayEntry()
    {
      for(std::vector<const FigureListEntry *>::const_iterator iter = figure_list.begin(); iter != figure_list.end(); iter++)
        delete const_cast<FigureListEntry *>(*iter);
    }
  FigureListEntry *appendFigureListEntry()
    {
      FigureListEntry *entry = new FigureListEntry;
      figure_list.push_back(entry);
      return entry;
    }

public:
  int timbre_bank;
  int figure_bank;
  int figure_class;
  std::vector<const FigureListEntry *> figure_list;
};

class KnowledgeEntry
{
public:
  KnowledgeEntry()
    : key(0),
      scale(0),
      tempo(0),
      time_beats(4),
      time_beat_type(4),
      for_rhythm(false),
      for_chord(false),
      for_timbre(false)
  {}

  ~KnowledgeEntry()
    {
      for(std::vector<const KnowledgeArrayEntry *>::const_iterator iter = m_knowledgeArrayEntries.begin(); iter != m_knowledgeArrayEntries.end(); iter++)
        delete const_cast<KnowledgeArrayEntry *>(*iter);
    }
  KnowledgeArrayEntry *appendKnowledgeArrayEntry()
    {
      KnowledgeArrayEntry *entry = new KnowledgeArrayEntry;
      m_knowledgeArrayEntries.push_back(entry);
      return entry;
    }
  void appendCharacter(int src_character)
    {
      character.push_back(src_character);
    }
  void appendGenre(int src_genre)
    {
      genre.push_back(src_genre);
    }

public:
  std::vector<const KnowledgeArrayEntry *> m_knowledgeArrayEntries;
  int key;
  int scale;
  float tempo;
  int time_beats;
  int time_beat_type;
  std::vector<int> character;
  std::vector<int> genre;
  bool for_rhythm;
  bool for_chord;
  bool for_timbre;
};

class KnowledgeModel
{
public:
  KnowledgeModel();
  ~KnowledgeModel();

public:
  int loadModel(const char *filename);
  inline std::vector<const KnowledgeEntry *> &models()
    {
      return m_knowledgeEntries;
    }
    
  int getChord(std::vector<const KnowledgeEntry *> &dst, int character);
  int getTimbreBank(std::vector<const KnowledgeEntry *> &dst, int genre);
  int getKnowledgeEntry(std::vector<const KnowledgeEntry *> &dst, int character);
  int getKnowledgeEntry(std::vector<const KnowledgeEntry *> &dst, int character, int genre);

private:
  KnowledgeEntry *appendKnowledgeEntry()
      {
        KnowledgeEntry *entry = new KnowledgeEntry;
        m_knowledgeEntries.push_back(entry);
        return entry;
      }
      
  void removeEntries();

private:
  std::vector<const KnowledgeEntry *> m_knowledgeEntries;
};

}

#define MAX_CHARACTER_INDEX 36

#endif
