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
#include <iostream>
#include <fstream>
#include <cstdio>

#include <yaml-cpp/yaml.h>
#include "libautomusic.h"
#include "knowledge-model.h"

namespace autocomp
{

KnowledgeModel::KnowledgeModel()
{
}

KnowledgeModel::~KnowledgeModel()
{
  removeEntries();
}

void KnowledgeModel::removeEntries()
{
  for(std::vector<const KnowledgeEntry *>::const_iterator iter = m_knowledgeEntries.begin(); iter != m_knowledgeEntries.end(); iter++)
    delete const_cast<KnowledgeEntry *>(*iter);
  m_knowledgeEntries.clear();
}

int KnowledgeModel::loadModels(const char *modelPath)
{
  char filename_buff[4096];
  char filename_pattern[256];
  try
    {
      std::snprintf(filename_buff, sizeof filename_buff, "%s/meta-data/index.meta", modelPath);

      YAML::Node root = YAML::LoadFile(filename_buff);
      YAML::Node index = root["index"];
      std::string pattern = index["filename-pattern"].as<std::string>();
      int index_start = index["index-start"].as<int>();
      int index_end = index["index-end"].as<int>();

      for(int i=index_start; i <= index_end; i++)
        {
          std::snprintf(filename_pattern, sizeof filename_pattern, "%%s/%s", pattern.c_str());
          std::snprintf(filename_buff, sizeof filename_buff, filename_pattern, modelPath, i);

          std::cout << "loadModels(): " << filename_buff << std::endl;

          if (int rc = loadModelFile(filename_buff))
            return rc;
        }
      return 0;
    }
  catch( YAML::Exception &excp )
    {
      std::cerr << "loadModels(): " << excp.what() << std::endl;
      return -RC_OPENFILE;
    }
}

int KnowledgeModel::loadModelFile(const char *filename)
{
  using namespace std;
  int rc = 0;
  try
    {
      int entry_count = 0;
      YAML::Node root = YAML::LoadFile(filename);
      YAML::Node knowledge_array = root["knowledge_array"];
      YAML::Node knowledge_constraint = root["knowledge_constraint"];

      if( knowledge_array.IsSequence() && knowledge_constraint.IsMap() )
        {
          KnowledgeEntry *entry = appendKnowledgeEntry();

          for(std::size_t i=0; i < knowledge_array.size(); i++)
            {
              YAML::Node knowledge = knowledge_array[i];

              KnowledgeArrayEntry *arrayEntry = entry->appendKnowledgeArrayEntry();

              arrayEntry->timbre_bank   = knowledge["timbre_bank"].as<int>();
              arrayEntry->figure_bank   = knowledge["figure_bank"].as<int>();
              arrayEntry->figure_class  = knowledge["class"].as<int>();
              YAML::Node figure_list    = knowledge["figure_list"];

              if( figure_list.IsSequence() )
                {
                  for(std::size_t j=0; j < figure_list.size(); j++)
                    {
                      YAML::Node figure = figure_list[j];
                      FigureListEntry *figureListEntry = arrayEntry->appendFigureListEntry();

                      /*
                       * Read-in and parse chords
                       */
                      YAML::Node chords = figure["chord"];
                      if( chords.IsSequence() )
                        {
                          for(std::size_t k=0; k < chords.size(); k++)
                            {
                              YAML::Node chordpair = chords[k];
                              if( chordpair.IsSequence() )
                                {
                                  figureListEntry->appendChord(ChordPair(chordpair[0].as<int>(),
                                                                         chordpair[1].as<int>()));
                                }
                            }
                        }

                      figureListEntry->segment  = figure["segment"].as<int>();
                      figureListEntry->offset   = figure["offset"].as<int>();
                      figureListEntry->begin    = figure["begin"].as<uint32_t>();
                      figureListEntry->end      = figure["end"].as<uint32_t>();

                      /*
                       * Read-in and parse pitches
                       */
                      YAML::Node pitchs = figure["pitch"];
                      if( pitchs.IsSequence() )
                        {
                          for(std::size_t k=0; k < pitchs.size(); k+=4)
                            {
                              uint32_t pitch = pitchs[k].as<uint32_t>();
                              uint32_t velocity = pitchs[k+1].as<uint32_t>();

                              if(pitch > 255 || velocity > 255)
                                throw YAML::RepresentationException(YAML::Mark(), "Pitch or velocity is out of range.");

                              figureListEntry->appendPitch(PitchNote(static_cast<uint8_t>(pitch),
                                                                     static_cast<uint8_t>(velocity),
                                                                     pitchs[k+2].as<int32_t>(),
                                                                     pitchs[k+3].as<int32_t>()));
                            }
                        }
                    }
                }
            }

          /*
           * Get extra parameters from knowledge constraint
           */
          entry->key            = knowledge_constraint["key"].as<int>();
          entry->scale          = knowledge_constraint["scale"].as<int>();
          entry->tempo          = knowledge_constraint["tempo"].as<float>();
          entry->time_beats     = knowledge_constraint["time_beats"].as<int>();
          entry->time_beat_type = knowledge_constraint["time_beat_type"].as<int>();
          entry->for_rhythm     = knowledge_constraint["for_rhythm"].as<bool>();
          entry->for_chord      = knowledge_constraint["for_chord"].as<bool>();
          entry->for_timbre     = knowledge_constraint["for_timbre"].as<bool>();

          YAML::Node character = knowledge_constraint["character"];
          if( character.IsSequence() )
            {
              for(std::size_t k=0; k < character.size(); k+=4)
                {
                  entry->appendCharacter(character[k].as<int>());
                }
            }
          YAML::Node genre = knowledge_constraint["genre"];
            if( genre.IsSequence() )
              {
                for(std::size_t k=0; k < genre.size(); k+=4)
                  {
                    entry->appendGenre(genre[k].as<int>());
                  }
              }

          entry_count++;
        }

      rc = entry_count > 0 ? 0 : -RC_PARSE_DATABASE;
    }
  catch( YAML::Exception &excp )
    {
      std::cerr << "loadModelFile(): " << excp.what() << std::endl;
      rc = -RC_OPENFILE;
    }
  if(rc) { removeEntries(); }
  return rc;
}

/**
 * @brief Get entries of chord knowledge by music character
 */
int KnowledgeModel::getChord(std::vector<const KnowledgeEntry *> &dst, int character)
{
  dst.clear();
  std::size_t size = models().size();
  for(std::size_t i=0; i < size; i++)
    {
      const KnowledgeEntry *entry = models()[i];
      std::size_t characterSize = entry->character.size();
      for(std::size_t k=0; k < characterSize; k++)
        {
          if( character == entry->character[k] && entry->for_chord )
            dst.push_back(entry);
        }
    }
  return dst.size() ? 0 : -RC_FAILED;
}

/**
 * @brief Get entries of timbre knowledge by music genre
 */
int KnowledgeModel::getTimbreBank(std::vector<const KnowledgeEntry *> &dst, int genre)
{
  dst.clear();
  std::size_t size = models().size();
  for(std::size_t i=0; i < size; i++)
    {
      const KnowledgeEntry *entry = models()[i];
      std::size_t characterSize = entry->character.size();
      for(std::size_t k=0; k < characterSize; k++)
        {
          if( genre == entry->genre[k] && entry->for_timbre )
            dst.push_back(entry);
        }
    }
  return dst.size() ? 0 : -RC_FAILED;
}

/**
 * @brief Get entries of timbre knowledge by music character
 */
int KnowledgeModel::getKnowledgeEntry(std::vector<const KnowledgeEntry *> &dst, int character)
{
  dst.clear();
  std::size_t size = models().size();
  for(std::size_t i=0; i < size; i++)
    {
      const KnowledgeEntry *entry = models()[i];
      for(std::size_t k=0; k < entry->character.size(); k++)
        {
          if( character == entry->character[k] )
            dst.push_back(entry);
        }
    }
  return dst.size() ? 0 : -RC_FAILED;
}

/**
 * @brief Get entries of timbre knowledge by character and genre
 */
int KnowledgeModel::getKnowledgeEntry(std::vector<const KnowledgeEntry *> &dst, int character, int genre)
{
  dst.clear();
  std::size_t size = models().size();
  for(std::size_t i=0; i < size; i++)
    {
      const KnowledgeEntry *entry = models()[i];
      bool matched = false;
      for(std::size_t k=0; k < entry->character.size(); k++)
        if( character == entry->character[k] )
          { matched = true; break; }
      if(matched)
        {
          matched = false;
          for(std::size_t k=0; k < entry->genre.size(); k++)
            if( genre == entry->genre[k] )
              { matched = true; break; }
        }
      if(matched)
        dst.push_back(entry);
    }
  return dst.size() ? 0 : -RC_FAILED;
}

}
