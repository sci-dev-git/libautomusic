
#include <cassert>

#include "util-randomize.h"
#include "knowledge-model.h"
#include "theory-orchestration.h"

namespace autocomp
{ namespace theory
  {

#define GM_TIMBRE_BANK_NUM 11
static const int gm_timbre_banks[GM_TIMBRE_BANK_NUM][27] =
  {
    {0, 1, 2, 3, 4, 5, 6, 7, -1},           /* Piano */
    {-1},                                   /* Melody */
    {24, 25, 26, 27, 28, 29, 30, 31, -1},   /* Guitar */
    {32, 33, 34, 35, 36, 37, 38, 39, -1},   /* Bass */
    {16, 17, 18, 19, 20, 21, 22, 23, -1},   /* Organ */
    {128, -1},                              /* Drums */
    {40, 41, 42, 43, 44, 45, 48, 49, 50, 51, 52, 53, 54, 55, -1}, /* Strings */
    {56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, -1}, /* Wind */
    {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, -1}, /* Effect */
    {104, 105, 106, 107, 108, 109, 110, 111, -1}, /* National */
    {8, 9, 10, 11, 12, 13, 14, 15, 46, 47, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, -1} /* Unsorted */
  };

#define SOLO_INSTRUMENTS_NUM 18
static const int solo_instruments[SOLO_INSTRUMENTS_NUM] =
    {0, 1, 3, 4, 5, 11, 18, 21, 22, 23, 24, 29, 65, 68, 71, 72, 73, 80};

#define RELATED_TIMBRES_NUM 11
static const int related_timbre_banks[RELATED_TIMBRES_NUM][8] =
  {
    {FIGURE_BANK_GUITAR, FIGURE_BANK_PIANO, FIGURE_BANK_STRINGS, FIGURE_BANK_WIND, FIGURE_BANK_ORGAN, FIGURE_BANK_EFFECT, FIGURE_BANK_UNSORTED, -1},    /* Piano */
    {FIGURE_BANK_MELODY, -1},           /* Melody */
    {FIGURE_BANK_GUITAR, FIGURE_BANK_PIANO, FIGURE_BANK_STRINGS, FIGURE_BANK_WIND, FIGURE_BANK_ORGAN, FIGURE_BANK_EFFECT, FIGURE_BANK_UNSORTED, -1},    /* Guitar */
    {FIGURE_BANK_BASS, -1},             /* Bass */
    {FIGURE_BANK_STRINGS, FIGURE_BANK_WIND, FIGURE_BANK_ORGAN, FIGURE_BANK_EFFECT, FIGURE_BANK_PIANO, -1},      /* Strings */
    {FIGURE_BANK_DRUMS, -1},            /* Drums */
    {FIGURE_BANK_WIND, FIGURE_BANK_STRINGS, FIGURE_BANK_ORGAN, FIGURE_BANK_EFFECT, FIGURE_BANK_PIANO, -1},      /* Wind */
    {FIGURE_BANK_ORGAN, FIGURE_BANK_STRINGS, FIGURE_BANK_WIND, FIGURE_BANK_EFFECT, FIGURE_BANK_PIANO, -1},      /* Organ */
    {FIGURE_BANK_EFFECT, FIGURE_BANK_STRINGS, FIGURE_BANK_WIND, FIGURE_BANK_ORGAN, FIGURE_BANK_PIANO, -1},      /* Effect */
    {FIGURE_BANK_NATIONAL, FIGURE_BANK_PIANO, FIGURE_BANK_STRINGS, FIGURE_BANK_WIND, FIGURE_BANK_ORGAN, FIGURE_BANK_EFFECT, FIGURE_BANK_UNSORTED, -1},  /* National */
    {FIGURE_BANK_UNSORTED, FIGURE_BANK_PIANO, FIGURE_BANK_WIND, FIGURE_BANK_ORGAN, FIGURE_BANK_EFFECT, FIGURE_BANK_NATIONAL, -1}        /* Unsorted */
  };

#define MAX_TIMBRE_TRACK_NUM 6

static bool is_grouped_in_timbre_bank(int timbre_bank, int gm_timbre)
{
  if(gm_timbre_banks[timbre_bank][0] == -1 && gm_timbre != 128)
    return true;
  for(unsigned int j=0; gm_timbre_banks[timbre_bank][j] != -1; j++)
    {
      if(gm_timbre == gm_timbre_banks[timbre_bank][j])
        return true;
    }
  return false;
}
static std::size_t timbre_bank_count(int timbre_bank)
{
  std::size_t i = 0;
  while(gm_timbre_banks[timbre_bank][i] != -1) ++i;
  return i;
}

/**
 * @brief Get the bank (FIGURE_BANK_*) of specified GM timbre index
 */
static int get_timbre_figure_bank(int gm_timbre_index)
{
  int i;
  for(i=0; i < GM_TIMBRE_BANK_NUM; i++)
    {
      if(i == FIGURE_BANK_MELODY)
        continue;
      if(is_grouped_in_timbre_bank(i, gm_timbre_index))
        return i;
    }
  return FIGURE_BANK_UNSORTED;
}

bool is_timbre_bank_related(int figure_bank, int dst_figure_bank)
{
  for(unsigned int i=0; related_timbre_banks[figure_bank][i] != -1; i++)
    if( related_timbre_banks[figure_bank][i] == dst_figure_bank )
      return true;
  return false;
}

/**
 * @brief Layout all the instrument tracks (parts) of the music works.
 * Build up an available manifest of instruments with their figure banks and figure classes.
 */
int layout_timbres(std::vector<int> &dst_timbre_banks,
                   std::vector<int> &dst_figure_banks,
                   std::vector<int> &dst_figure_classes,
                   const std::vector<int> &src_timbre_banks,
                   const std::vector<int> &src_figure_banks,
                   const std::vector<int> &src_figure_classes,
                   bool has_melody)
{
  std::size_t timbre_banks_num = src_timbre_banks.size();
  dst_timbre_banks.clear();
  dst_figure_banks.clear();
  dst_figure_classes.clear();

  /*
   * First of all, settle down the solo instrument.
   * If there is not any melody required, randomly select a instrument from solo types,
   * Otherwise, ensuring we have a melody-grouped instrument.
   */
  int cur_timbre_bank = util::random_choice(solo_instruments, SOLO_INSTRUMENTS_NUM);

  if(has_melody)
    {
      for(std::size_t i=0; i < timbre_banks_num; i++)
        if( src_figure_banks[i] == FIGURE_BANK_MELODY and src_figure_classes[i] == FIGURE_CLASS_SOLO )
          {
            cur_timbre_bank = src_timbre_banks[i];
            break;
          }
    }

  dst_timbre_banks.push_back(cur_timbre_bank);
  dst_figure_banks.push_back(FIGURE_BANK_MELODY);
  dst_figure_classes.push_back(FIGURE_CLASS_SOLO);

  /*
   * Have a look at any non melody instruments.
   * If the required is not existing, randomly select a instruments from solo types.
   */
  int knowledge_solo_non_melody_index = -1;
  for(std::size_t i=0; i < timbre_banks_num; i++)
    {
      if( src_figure_banks[i] != FIGURE_BANK_MELODY && src_figure_classes[i] == FIGURE_CLASS_SOLO )
        {
          knowledge_solo_non_melody_index = i;
          break;
        }
    }
  if( knowledge_solo_non_melody_index >= 0 )
    {
      int cur_figure_bank = src_figure_banks[knowledge_solo_non_melody_index];
      int cur_timbre_bank = src_timbre_banks[knowledge_solo_non_melody_index];

      if(!is_grouped_in_timbre_bank(cur_figure_bank, cur_timbre_bank)) /* there is no pair of current timbre */
        {
          cur_figure_bank = get_timbre_figure_bank(cur_timbre_bank);
        }
      dst_figure_banks.push_back(cur_figure_bank);
      dst_timbre_banks.push_back(cur_timbre_bank);
    }
  else
    {
      int rand_instrument_index = util::random_choice(solo_instruments, SOLO_INSTRUMENTS_NUM);
      int rand_figure_bank = get_timbre_figure_bank(rand_instrument_index);
      dst_figure_banks.push_back(rand_figure_bank);
      dst_timbre_banks.push_back(rand_instrument_index);
    }
  dst_figure_classes.push_back(FIGURE_CLASS_SOLO);

  /*
   * Insert other accompaniment instruments, e.g drums and chord accompaniment.
   */
  for(std::size_t i=0; i < timbre_banks_num; i++)
    {
      if( src_figure_banks[i] == FIGURE_BANK_DRUMS )
        {
          bool found = false;
          for(std::size_t k=0; k < dst_figure_banks.size(); k++)
            if( dst_figure_banks[k] == FIGURE_BANK_DRUMS )
              { found = true; break; }
          if(found)
            continue;
          if(is_grouped_in_timbre_bank(FIGURE_BANK_DRUMS, src_timbre_banks[i]))
              dst_timbre_banks.push_back(src_timbre_banks[i]);
          else
              dst_timbre_banks.push_back(util::random_choice(gm_timbre_banks[FIGURE_BANK_DRUMS], timbre_bank_count(FIGURE_BANK_DRUMS)));
          dst_figure_banks.push_back(FIGURE_BANK_DRUMS);
          dst_figure_classes.push_back(FIGURE_CLASS_CHORD);
        }
      else if( src_figure_classes[i] == FIGURE_CLASS_CHORD )
        {
          int cur_figure_bank = src_figure_banks[i];
          int cur_timbre_bank = src_timbre_banks[i];
          dst_timbre_banks.push_back(cur_timbre_bank);

          if(!is_grouped_in_timbre_bank(cur_figure_bank, cur_timbre_bank)) /* there is no pair of current timbre */
            {
              cur_figure_bank = get_timbre_figure_bank(cur_figure_bank);
            }
          dst_figure_banks.push_back(cur_figure_bank);
          dst_figure_classes.push_back(FIGURE_CLASS_CHORD);
        }
      if( dst_timbre_banks.size() >= MAX_TIMBRE_TRACK_NUM )
        break;
    }
  return 0;
}

int get_timbres(const KnowledgeEntry *knowledgeEntry,
                   std::vector<int> &timbre_banks,
                   std::vector<int> &figure_banks,
                   std::vector<int> &figure_classes)
{
  timbre_banks.clear();
  figure_banks.clear();
  figure_classes.clear();
  for(std::size_t i=0; i < knowledgeEntry->m_knowledgeArrayEntries.size(); i++)
    {
      const KnowledgeArrayEntry *knowledgeArrayEntry = knowledgeEntry->m_knowledgeArrayEntries[i];
      int timbre_bank = knowledgeArrayEntry->timbre_bank;
      int figure_bank = knowledgeArrayEntry->figure_bank;
      int figure_class = knowledgeArrayEntry->figure_class;

      if( figure_class != FIGURE_CLASS_SOLO && figure_bank != FIGURE_BANK_MELODY && figure_bank != FIGURE_BANK_DRUMS )
        {
          figure_bank = get_timbre_figure_bank(timbre_bank);
        }
      timbre_banks.push_back(timbre_bank);
      figure_banks.push_back(figure_bank);
      figure_classes.push_back(figure_class);
    }
  return 0;
}

static int get_timbre_figures_helper(std::vector<const KnowledgeEntry *> &dst_entries,
                       std::vector<int> &dst_tracks,
                       const std::vector<const KnowledgeEntry *> &knowledge_entries, int figure_bank, int figure_class)
{
  dst_entries.clear(); dst_tracks.clear();
  std::vector<const KnowledgeEntry *> related_entries;
  std::vector<int> related_tracks;

  for(std::size_t i=0; i < knowledge_entries.size(); i++)
    {
      std::vector<int> timbre_banks;
      std::vector<int> figure_banks;
      std::vector<int> figure_classes;

      if( int err = get_timbres(knowledge_entries[i], timbre_banks, figure_banks, figure_classes) )
        return err;

      bool not_found = true;
      for(std::size_t j=0; j < timbre_banks.size(); j++)
        {
          if( figure_bank == figure_banks[j] && figure_class == figure_classes[j] )
            {
              not_found = false;
              dst_entries.push_back(knowledge_entries[i]);
              dst_tracks.push_back(j);
              break;
            }
        }
      if( not_found )
        {
          for(std::size_t j=0; j < timbre_banks.size(); j++)
            {
              if( figure_class == figure_classes[j] )
                {
                  if( is_timbre_bank_related(figure_bank, figure_banks[j]) )
                    {
                      related_entries.push_back(knowledge_entries[i]);
                      related_tracks.push_back(j);
                      break;
                    }
                }
            }
        }
    }

  /*
   * Utilize related figures when the quantity of matched figures is poor.
   */
  if( dst_entries.size() < 10 )
    {
      for(std::size_t i=0; i < related_entries.size(); i++)
        dst_entries.push_back(related_entries[i]);
      for(std::size_t i=0; i < related_tracks.size(); i++)
        dst_tracks.push_back(related_tracks[i]);
    }

  return 0;
}

/*
 * @brief Get the compatible figures according to specified figure bank and class.
 * When it returns -1, you should consider enlarge the range of target knowledge entries.
 */
int get_timbre_figures(const KnowledgeEntry **ppKnowledgeEntry,
                       int *pTrack,
                       const std::vector<const KnowledgeEntry *> &knowledge_entries, int figure_bank, int figure_class)
{
  std::vector<const KnowledgeEntry *> figure_entries;
  std::vector<int> figure_tracks;
  if( int err = get_timbre_figures_helper(figure_entries, figure_tracks, knowledge_entries, figure_bank, figure_class) )
    return err;

  if( figure_entries.size() )
    {
      std::size_t idx = util::random_range(figure_entries.size());

      *ppKnowledgeEntry = figure_entries[idx];
      *pTrack = figure_tracks[idx];
      return 0;
    }
  return -1;
}

#define MAX_VELOCITY 127

static const float randomVelocityFactor = 0.2;
static const float randomVelocityThreshold = 0.3;
static const float soloVelocityProportion = 1.2;

/**
 * @brief Process the velocity of each notes for all the different instruments.
 * @param compositionChainTrack Target chains to be processed.
 * @param figureBanks Instrument figure banks.
 * @param figureClasses Figure classes.
 * @param velocityFactorModu Optional, Modulation coefficient of Randomization factor for velocity, ranged from [0, 1].
 * @param soloProportionModu Optional, Modulation coefficient of Proportion: velocity of solo track / chord track.
 */
int processVelocity(std::vector<std::vector<CompositionChainNode *>> &compositionChainTrack, const std::vector<int> &figureBanks, const std::vector<int> &figureClasses, float velocityFactorModu, float soloProportionModu)
{
  velocityFactorModu *= randomVelocityFactor;
  soloProportionModu *= soloVelocityProportion;
  /*
   * Randomize the velocity of each note when all the notes has the same velocity.
   */
  for(std::size_t trackNum=0; trackNum < compositionChainTrack.size(); trackNum++)
    {
      if (figureBanks[trackNum] != FIGURE_BANK_DRUMS)
        {
          int dullVelocity = 0, count = 0;
          for(std::size_t form=0; form < compositionChainTrack[trackNum].size(); form++)
            {
              if (compositionChainTrack[trackNum][form]->pitch.size())
                {
                  uint8_t velocity = compositionChainTrack[trackNum][form]->pitch[0].velocity;
                  for(std::size_t j=0; j < compositionChainTrack[trackNum][form]->pitch.size(); j++)
                    {
                      if (compositionChainTrack[trackNum][form]->pitch[j].velocity != velocity)
                        {
                          ++dullVelocity;
                        }
                      ++count;
                    }
                }
            }
          if (count && (float)dullVelocity / count < randomVelocityThreshold)
            {
              int8_t benchmark = MAX_VELOCITY * velocityFactorModu;
              for(std::size_t form=0; form < compositionChainTrack[trackNum].size(); form++)
                {
                  for(std::size_t j=0; j < compositionChainTrack[trackNum][form]->pitch.size(); j++)
                    {
                      int8_t lowmark = (int8_t)compositionChainTrack[trackNum][form]->pitch[j].velocity - benchmark;
                      uint8_t highmark = compositionChainTrack[trackNum][form]->pitch[j].velocity + benchmark;

                      highmark = highmark > MAX_VELOCITY ? MAX_VELOCITY : highmark; /* clip */
                      lowmark = lowmark < 0 ? 0 : lowmark;

                      compositionChainTrack[trackNum][form]->pitch[j].velocity = util::random_range<uint8_t>(lowmark, highmark);
                    }
                }
            }
        }
    }

  /*
   * Normalization. Adopt the velocity of current track to benchmark velocity.
   */
  int avreageVelocity = 0, sumCount = 0;
  for(std::size_t trackNum=0; trackNum < compositionChainTrack.size(); trackNum++)
      {
        for(std::size_t form=0; form < compositionChainTrack[trackNum].size(); form++)
          {
            for(std::size_t j=0; j < compositionChainTrack[trackNum][form]->pitch.size(); j++)
              {
                avreageVelocity += compositionChainTrack[trackNum][form]->pitch[j].velocity;
                ++sumCount;
              }
          }
      }
  avreageVelocity /= sumCount;
  assert(avreageVelocity <= 127);

  int chordBenchmark = avreageVelocity;
  int soloBenchmark = int(avreageVelocity * soloProportionModu);
  if (soloBenchmark > MAX_VELOCITY) soloBenchmark = MAX_VELOCITY;

  for(std::size_t trackNum=0; trackNum < compositionChainTrack.size(); trackNum++)
    {
      /* Calculate the DC (Direct Current) offset of velocity */
      int DC_offset = 0, DC_count = 0;
      for(std::size_t form=0; form < compositionChainTrack[trackNum].size(); form++)
        {
          for(std::size_t j=0; j < compositionChainTrack[trackNum][form]->pitch.size(); j++)
            {
              DC_offset += compositionChainTrack[trackNum][form]->pitch[j].velocity;
              ++DC_count;
            }
        }
      DC_offset /= DC_count;

      /* Apply new offset to the original velocity */
      for(std::size_t form=0; form < compositionChainTrack[trackNum].size(); form++)
        {
          for(std::size_t j=0; j < compositionChainTrack[trackNum][form]->pitch.size(); j++)
            {
              uint8_t vel;
              switch(figureClasses[trackNum])
                {
                  case FIGURE_CLASS_SOLO:
                    vel = (uint8_t)soloBenchmark + (compositionChainTrack[trackNum][form]->pitch[j].velocity - DC_offset);
                    break;
                  case FIGURE_CLASS_CHORD:
                  default:
                    vel = (uint8_t)chordBenchmark + (compositionChainTrack[trackNum][form]->pitch[j].velocity - DC_offset);
                }

              vel = (vel > MAX_VELOCITY ? MAX_VELOCITY : vel); /* clip */
              vel = (vel < 0 ? DC_offset/2 : vel);

              compositionChainTrack[trackNum][form]->pitch[j].velocity = vel;
            }
        }
    }
  return 0;
}

  }
}
